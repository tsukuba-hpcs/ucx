#include "utofu_def.h"

ucs_status_t uct_utofu_ep_get_address(uct_ep_h tl_ep,	  
									  uct_ep_addr_t *addr) {
    uct_utofu_ep_address_t *utofu_address;
    uct_utofu_ep_t *ep;

    utofu_address = (uct_utofu_ep_address_t*)addr;
    ep = ucs_derived_of(tl_ep, uct_utofu_ep_t);

    utofu_address->vcq_id = ep->peer_vcq_id;

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
    ucs_debug("UCS_CLASS_INIT_FUNC(uct_utofu_ep_t, const uct_ep_params_t *params)");
    iface = ucs_derived_of(params->iface, uct_utofu_iface_t);
    remote_iface_addr = (uct_utofu_iface_addr_t *)params->iface_addr;

    UCS_CLASS_CALL_SUPER_INIT(uct_base_ep_t, &iface->super);

    self->iface = iface;
    self->peer_vcq_id = remote_iface_addr->vcq_id;

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
    return (UCS_ERR_NOT_IMPLEMENTED);
}