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

ucs_status_t uct_utofu_iface_query(uct_iface_h tl_iface, uct_iface_attr_t *iface_attr) {
    // uct_utofu_iface_t *iface = ucs_derived_of(tl_iface, uct_utofu_iface_t);
    iface_attr->device_addr_len = 0;
    iface_attr->iface_addr_len = sizeof(uct_utofu_iface_addr_t);
    iface_attr->ep_addr_len = sizeof(uct_utofu_iface_addr_t);
    iface_attr->max_conn_priv = 0;

    return (UCS_OK);
}

UCS_CLASS_INIT_FUNC(uct_utofu_ep_t, const uct_ep_params_t *params) {
    uct_utofu_iface_t *iface;
    uct_utofu_iface_addr_t *remote_iface_addr;

    iface = ucs_derived_of(params->iface, uct_utofu_iface_t);
    remote_iface_addr = (uct_utofu_iface_addr_t *)params->iface_addr;

    UCS_CLASS_CALL_SUPER_INIT(uct_base_ep_t, &iface->super);

    self->iface = iface;
    self->peer_vcq_id = remote_iface_addr->vcq_id;

    return (UCS_OK);
}

static UCS_CLASS_CLEANUP_FUNC(uct_utofu_ep_t) {
    ucs_debug("UCS_CLASS_CLEANUP_FUNC(uct_utofu_ep_t)");
}

UCS_CLASS_DEFINE(uct_utofu_ep_t, uct_base_ep_t);
UCS_CLASS_DEFINE_NEW_FUNC(uct_utofu_ep_t, uct_ep_t, const uct_ep_params_t *);
UCS_CLASS_DEFINE_DELETE_FUNC(uct_utofu_ep_t, uct_ep_t);

ucs_status_t uct_utofu_ep_flush(uct_ep_h tl_ep, unsigned flags, uct_completion_t *comp) {
    ucs_debug("uct_utofu_ep_flush");
    return (UCS_ERR_NOT_IMPLEMENTED);
}