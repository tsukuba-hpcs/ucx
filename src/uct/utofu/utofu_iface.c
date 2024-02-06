#include "utofu_def.h"


ucs_status_t uct_utofu_iface_query(uct_iface_h tl_iface, uct_iface_attr_t *iface_attr) {
    ucs_debug("uct_utofu_iface_query");
    iface_attr->cap.flags = 
		UCT_IFACE_FLAG_CONNECT_TO_EP;

    iface_attr->device_addr_len = 0;
    iface_attr->iface_addr_len = sizeof(uct_utofu_iface_addr_t);
    iface_attr->ep_addr_len = sizeof(uct_utofu_iface_addr_t);
    iface_attr->max_conn_priv = 0;

    // Aactive Message
	iface_attr->cap.am.max_iov          =  1;
	iface_attr->cap.am.opt_zcopy_align  =  1;
	iface_attr->cap.am.align_mtu        =  1;
    iface_attr->cap.am.max_short        =  32;
	iface_attr->cap.am.max_bcopy        =  64;
    iface_attr->cap.am.max_zcopy        =  128;

    iface_attr->cap.flags =
        UCT_IFACE_FLAG_AM_SHORT  | 
		UCT_IFACE_FLAG_AM_BCOPY  |
        UCT_IFACE_FLAG_AM_ZCOPY  |
        UCT_IFACE_FLAG_CB_SYNC  |
		UCT_IFACE_FLAG_CB_ASYNC  |
		UCT_IFACE_FLAG_PENDING	 |
		UCT_IFACE_FLAG_EP_CHECK  |
		UCT_IFACE_FLAG_CONNECT_TO_IFACE |
		UCT_IFACE_FLAG_CONNECT_TO_EP;

	iface_attr->cap.flags |= UCT_IFACE_FLAG_ATOMIC_CPU;

    iface_attr->overhead = 50e-9;
	iface_attr->latency = ucs_linear_func_make(70e-9, 0);
	iface_attr->priority = 0;

	iface_attr->bandwidth.dedicated = 6800 * UCS_MBYTE;
	iface_attr->bandwidth.shared = 0;

	iface_attr->dev_num_paths = 1;

    return (UCS_OK);
}

ucs_status_t uct_utofu_iface_get_address(uct_iface_h tl_iface,
										 uct_iface_addr_t *addr) {
    uct_utofu_iface_t *iface = ucs_derived_of(tl_iface, uct_utofu_iface_t);
    uct_utofu_iface_addr_t *iface_addr = (uct_utofu_iface_addr_t *)addr;
    ucs_debug("uct_utofu_iface_get_address");

    iface_addr->vcq_id = iface->md->vcq_id;

    return (UCS_OK);
}

ucs_status_t uct_utofu_iface_get_device_address(uct_iface_t *tl_iface,
												uct_device_addr_t *addr)
{
	return UCS_OK;
}

ucs_status_t uct_utofu_iface_flush(uct_iface_h tl_iface, unsigned flags, uct_completion_t *comp) {
    ucs_debug("uct_utofu_iface_flush");
    return (UCS_ERR_NOT_IMPLEMENTED);
}

void uct_utofu_iface_progress_enable(uct_iface_h tl_iface, unsigned flags) {
    ucs_debug("uct_utofu_iface_progress_enable");
    uct_base_iface_progress_enable(tl_iface, flags);
}

void uct_utofu_iface_progress_disable(uct_iface_h tl_iface, unsigned flags) {
    ucs_debug("uct_utofu_iface_progress_disable");
}

unsigned uct_utofu_iface_progress(uct_iface_h tl_iface) {
    ucs_debug("uct_utofu_iface_progress");
    return (0);
}

int uct_utofu_iface_is_reachable(uct_iface_h tl_iface, const uct_device_addr_t *device_addr, const uct_iface_addr_t *iface_addr) {
    ucs_debug("uct_utofu_iface_is_reachable");
    return (1);
}

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
    .ep_pending_purge = uct_utofu_ep_pending_purge,
    .ep_pending_add = NULL,
    .ep_flush = uct_utofu_ep_flush,
    .ep_fence = uct_base_ep_fence,
    .ep_check = NULL,
    .ep_create = UCS_CLASS_NEW_FUNC_NAME(uct_utofu_ep_t),
    .ep_destroy = UCS_CLASS_DELETE_FUNC_NAME(uct_utofu_ep_t),
    .ep_get_address = uct_utofu_ep_get_address,
    .ep_connect_to_ep = NULL,
    .iface_flush = uct_utofu_iface_flush,
    .iface_fence = uct_base_iface_fence,
    .iface_progress_enable = uct_utofu_iface_progress_enable,
    .iface_progress_disable = uct_utofu_iface_progress_disable,
    .iface_progress = uct_utofu_iface_progress,
    .iface_event_fd_get = NULL,
    .iface_event_arm = NULL,
    .iface_close = UCS_CLASS_DELETE_FUNC_NAME(uct_utofu_iface_t),
    .iface_query = uct_utofu_iface_query,
    .iface_get_address = uct_utofu_iface_get_address,
    .iface_get_device_address = uct_utofu_iface_get_device_address,
    .iface_is_reachable       = uct_utofu_iface_is_reachable,
};

static uct_iface_internal_ops_t uct_utofu_iface_internal_ops = {
	.iface_estimate_perf = uct_base_iface_estimate_perf,
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
    return UCS_OK;
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
    uct_utofu_md_t *md;
	uct_tl_device_resource_t *devices;
    ucs_debug("uct_utofu_query_devices\n");
    md = ucs_derived_of(tl_md, uct_utofu_md_t);
	devices = ucs_malloc(sizeof(*devices), "uct_tl_device_resource_t");
	ucs_snprintf_zero(devices->name, 14, "Tofu-D(TNI=%d)", md->tni_id);
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