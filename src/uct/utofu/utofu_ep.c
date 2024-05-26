#include "utofu_def.h"

ucs_status_t uct_utofu_ep_get_address(uct_ep_h tl_ep,	  
									  uct_ep_addr_t *addr) {
    uct_utofu_ep_address_t *utofu_address;
    uct_utofu_ep_t *ep;

    utofu_address = (uct_utofu_ep_address_t*)addr;
    ep = ucs_derived_of(tl_ep, uct_utofu_ep_t);

    utofu_address->vcq_id = ep->vcq_id;
    utofu_address->am_rb_tail_stadd = ep->am_rb_tail_stadd;
    utofu_address->am_rb_stadd = ep->am_rb_stadd;

    return (UCS_OK);
}

ucs_arbiter_cb_result_t uct_utofu_ep_arbiter_purge_cb(ucs_arbiter_t *arbiter,
													  ucs_arbiter_group_t *group,
													  ucs_arbiter_elem_t *elem,
													  void *arg)
{
	uct_utofu_ep_t *ep = ucs_container_of(group, uct_utofu_ep_t, arb_group);
	uct_utofu_iface_t *iface = ep->iface;
	uct_pending_req_t *req = ucs_container_of(elem, uct_pending_req_t, priv);
	uct_purge_cb_args_t *cb_args = arg;
    ucs_debug("uct_utofu_ep_arbiter_purge_cb");

	if (arg != NULL) {
		cb_args->cb(req, cb_args->arg);
        free(cb_args);
	} else {
        ucs_warn("ep=%p cancelling user pending request %p", ep, req);
	}

	uct_worker_progress_remove(iface->super.worker, &iface->super.prog);
	return UCS_ARBITER_CB_RESULT_REMOVE_ELEM;
}

void uct_utofu_ep_pending_purge(uct_ep_h tl_ep,
								uct_pending_purge_callback_t cb,
								void *arg)
{
	uct_utofu_ep_t *ep = ucs_derived_of(tl_ep, uct_utofu_ep_t);
	uct_utofu_iface_t *iface = ep->iface;
	uct_purge_cb_args_t *args;
    ucs_debug("uct_utofu_ep_pending_purge");

    args = ucs_malloc(sizeof(*args), "uct_purge_cb_args_t");
    args->arg = arg;
    args->cb = cb;

	ucs_arbiter_group_purge(&iface->arbiter,
							&ep->arb_group,
							uct_utofu_ep_arbiter_purge_cb,
							args);
}

UCS_CLASS_INIT_FUNC(uct_utofu_ep_t, const uct_ep_params_t *params) {
    uct_utofu_iface_t *iface;
    uct_utofu_iface_addr_t *remote_iface_addr;
    iface = ucs_derived_of(params->iface, uct_utofu_iface_t);
    remote_iface_addr = (uct_utofu_iface_addr_t *)params->iface_addr;

    UCS_CLASS_CALL_SUPER_INIT(uct_base_ep_t, &iface->super);

    self->iface = iface;
    self->vcq_id = remote_iface_addr->vcq_id;
    self->am_rb_tail_stadd = remote_iface_addr->am_rb_tail_stadd;
    self->am_rb_stadd = remote_iface_addr->am_rb_stadd;

    ucs_debug("UCS_CLASS_INIT_FUNC(uct_utofu_ep_t, const uct_ep_params_t *params) vcq_id=%zu", self->vcq_id);

    ucs_arbiter_group_init(&self->arb_group);

    return (UCS_OK);
}

static UCS_CLASS_CLEANUP_FUNC(uct_utofu_ep_t) {
    uct_utofu_iface_t *iface = self->iface;
    ucs_debug("UCS_CLASS_CLEANUP_FUNC(uct_utofu_ep_t)");
    ucs_arbiter_group_purge(&iface->arbiter,
							&self->arb_group,
							uct_utofu_ep_arbiter_purge_cb,
							NULL);
}

UCS_CLASS_DEFINE(uct_utofu_ep_t, uct_base_ep_t);
UCS_CLASS_DEFINE_NEW_FUNC(uct_utofu_ep_t, uct_ep_t, const uct_ep_params_t *);
UCS_CLASS_DEFINE_DELETE_FUNC(uct_utofu_ep_t, uct_ep_t);

ucs_status_t uct_utofu_ep_flush(uct_ep_h tl_ep, unsigned flags, uct_completion_t *comp) {
    ucs_debug("uct_utofu_ep_flush");
    return (UCS_OK);
}

ssize_t uct_utofu_ep_am_bcopy(uct_ep_h tl_ep,
                              uint8_t id,
                              uct_pack_callback_t pack_cb,
                              void *arg,
                              unsigned flags) {
    int rc;
    struct utofu_mrq_notice mnotice;
    uct_utofu_ep_t *ep = ucs_derived_of(tl_ep, uct_utofu_ep_t);
    unsigned int length = 0;
    uct_utofu_am_bcopy_buf *buf = NULL;
    utofu_stadd_t buf_stadd;
    utofu_stadd_t target_stadd;
    void *cbdata;

    UCT_TL_IFACE_GET_TX_DESC(&ep->iface->super, &ep->iface->mp.buf, buf, return UCS_ERR_NO_RESOURCE);
    ucs_assert(buf != NULL);
    buf->complete = 0;
    buf->am_id = id;
    length = pack_cb(&buf->data, arg);
    buf->length = length;
    ucs_debug("packed length=%u", length);
    rc = utofu_reg_mem(ep->iface->md->vcq_hdl, buf, sizeof(uct_utofu_am_bcopy_buf) + length, 0, &buf_stadd);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_ref_mem error rc=%d", rc);
        return (-1);
    }
    ucs_assert(sizeof(uct_utofu_am_bcopy_buf) + length <= UCT_UTOFU_RINGBUF_ITEM_SIZE);

    ucs_debug("uct_utofu_ep_am_bcopy vcq_id=%zu", ep->vcq_id);
    rc = utofu_armw8(ep->iface->md->vcq_hdl, ep->vcq_id, UTOFU_ARMW_OP_ADD, 1, ep->am_rb_tail_stadd, 0, UTOFU_ONESIDED_FLAG_LOCAL_MRQ_NOTICE, NULL);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_armw4 error rc=%d", rc);
        return (-1);
    }
    do {
        rc = utofu_poll_mrq(ep->iface->md->vcq_hdl, 0, &mnotice);
    } while (rc == UTOFU_ERR_NOT_FOUND);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_poll_mrq error rc=%d", rc);
        return (-1);
    }
    if (mnotice.notice_type != UTOFU_MRQ_TYPE_LCL_ARMW) {
        ucs_error("notice.notice_type != UTOFU_MRQ_TYPE_LCL_ARMW");
        return (-1);
    }
    ucs_debug("remote value=%zu", mnotice.rmt_value);
    target_stadd = ep->am_rb_stadd + (mnotice.rmt_value % UCT_UTOFU_RINGBUF_ITEM_COUNT) * (UCT_UTOFU_RINGBUF_ITEM_SIZE);
    rc = utofu_put(ep->iface->md->vcq_hdl, ep->vcq_id, buf_stadd, target_stadd,
        sizeof(uct_utofu_am_bcopy_buf) + length, mnotice.rmt_value % 256,
        UTOFU_ONESIDED_FLAG_TCQ_NOTICE | UTOFU_ONESIDED_FLAG_REMOTE_MRQ_NOTICE, NULL);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_put error %d", rc);
        return (-1);
    }
    do {
        rc = utofu_poll_tcq(ep->iface->md->vcq_hdl, 0, &cbdata);
    } while (rc == UTOFU_ERR_NOT_FOUND);
    ucs_mpool_put(buf);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_poll_tcq error rc=%d", rc);
        return (-1);
    }
    return (length);
}

ucs_status_t uct_utofu_ep_get_bcopy(uct_ep_h ep,
                                    uct_unpack_callback_t unpack_cb,
                                    void *arg,
                                    size_t length,
                                    uint64_t remote_addr,
                                    uct_rkey_t rkey,
                                    uct_completion_t *comp) {
    ucs_debug("uct_utofu_ep_get_bcopy");
    return (UCS_ERR_NOT_IMPLEMENTED);
}