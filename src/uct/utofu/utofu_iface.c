#include "utofu_def.h"

static ucs_config_field_t uct_utofu_iface_config_table[] = {
    {NULL}
};

static uct_iface_ops_t uct_utofu_iface_ops = {
    .ep_am_short = NULL,
    .ep_am_short_iov = NULL,
    .ep_am_bcopy = NULL,
    .ep_am_zcopy = NULL,
    .ep_put_short = NULL,
    .ep_put_bcopy = NULL,
    .ep_put_zcopy = NULL,
    .ep_get_bcopy = NULL,
    .ep_get_zcopy = NULL,
    .ep_atomic_cswap64 = NULL,
    .ep_atomic64_post = NULL,
    .ep_atomic64_fetch = NULL,
    .ep_atomic_cswap32 = NULL,
    .ep_atomic32_post = NULL,
    .ep_atomic32_fetch = NULL,
    .ep_pending_purge = NULL,
    .ep_pending_add = NULL,
    .ep_flush = NULL,
    .ep_fence = NULL,
    .ep_check = NULL,
    .ep_create = NULL,
    .ep_destroy = NULL,
    .ep_get_address = NULL,
    .ep_connect_to_ep = NULL,
    .iface_flush = NULL,
    .iface_fence = NULL,
    .iface_progress_enable = NULL,
    .iface_progress_disable = NULL,
    .iface_progress = NULL,
    .iface_event_fd_get = NULL,
    .iface_event_arm = NULL,
    .iface_close = NULL,
    .iface_query = NULL,
    .iface_get_address = NULL,
    .iface_get_device_address = NULL,
    .iface_is_reachable       = NULL,
};

static uct_iface_internal_ops_t uct_utofu_iface_internal_ops = {
	.iface_estimate_perf = NULL,
};

static UCS_CLASS_INIT_FUNC(uct_utofu_iface_t,
						   uct_md_h tl_md,
						   uct_worker_h worker,
						   const uct_iface_params_t *params,
						   const uct_iface_config_t *tl_config) {
    uct_utofu_md_t *md;
    ucs_debug("UCS_CLASS_INIT_FUNC(uct_utofu_iface_t)\n");
    md = ucs_derived_of(tl_md, uct_utofu_md_t);
    UCS_CLASS_CALL_SUPER_INIT(uct_base_iface_t,
        &uct_utofu_iface_ops,
        &uct_utofu_iface_internal_ops,
        tl_md,
        worker,
        params,
        tl_config
        UCS_STATS_ARG(params->stats_root)
        UCS_STATS_ARG(UCT_UTOFU_MD_NAME));
    self->md = md;
    return UCS_ERR_NOT_IMPLEMENTED;
}

static UCS_CLASS_CLEANUP_FUNC(uct_utofu_iface_t)
{
    return;
}

UCS_CLASS_DEFINE(uct_utofu_iface_t, uct_base_iface_t);
UCS_CLASS_DEFINE_DELETE_FUNC(uct_utofu_iface_t, uct_iface_t);


UCS_CLASS_DEFINE_NEW_FUNC(uct_utofu_iface_t, uct_iface_t, uct_md_h,
						  uct_worker_h, const uct_iface_params_t*,
						  const uct_iface_config_t*);

ucs_status_t uct_utofu_query_devices(uct_md_h tl_md,
									 uct_tl_device_resource_t **tl_devices_p,
									 unsigned *num_tl_devices_p) {
	uct_tl_device_resource_t *devices;
    ucs_debug("uct_utofu_query_devices\n");
	devices = ucs_malloc(sizeof(*devices), "uct_tl_device_resource_t");
	ucs_snprintf_zero(devices->name, 14, "Tofu-D");
	devices->type = UCT_DEVICE_TYPE_NET;
	devices->sys_device = UCS_SYS_DEVICE_ID_UNKNOWN;
	*tl_devices_p = devices;
	*num_tl_devices_p = 1;
    return UCS_OK;
}


UCT_TL_DEFINE(&uct_utofu_component,
			  utofu,
			  uct_utofu_query_devices,
			  uct_utofu_iface_t,
			  UCT_UTOFU_CONFIG_PREFIX,
			  uct_utofu_iface_config_table,
			  uct_utofu_iface_config_t);