#include "utofu_def.h"

static ucs_config_field_t uct_utofu_md_config_table[] = {
    {NULL}
};

ucs_status_t uct_utofu_query_md_resources(uct_component_h component,
										  uct_md_resource_desc_t **resources_p,
										  unsigned *num_resources_p) {
    uct_md_resource_desc_t *resources;
    resources = ucs_malloc(sizeof(*resources), "uct_md_resource_desc_t");
    ucs_debug("uct_utofu_query_md_resources\n");
    ucs_snprintf_zero(resources->md_name, sizeof(resources->md_name), "utofu");
	*num_resources_p = 1;
	*resources_p = resources;

	return UCS_OK;
}

ucs_status_t uct_utofu_md_query(uct_md_h tl_md, 
								uct_md_attr_v2_t *md_attr)
{
    //uct_utofu_md_t *md = ucs_derived_of(tl_md, uct_utofu_md_t);
    ucs_debug("uct_utofu_md_query tl_md=%p\n", tl_md);
    md_attr->rkey_packed_size = sizeof(uct_utofu_rkey_t);
    md_attr->flags =
        UCT_MD_FLAG_ALLOC     |
		UCT_MD_FLAG_REG       |
		UCT_MD_FLAG_INVALIDATE|
		UCT_MD_FLAG_RKEY_PTR  |
		UCT_MD_FLAG_NEED_RKEY;
    md_attr->reg_mem_types    = UCS_BIT(UCS_MEMORY_TYPE_HOST);
    md_attr->cache_mem_types  = UCS_BIT(UCS_MEMORY_TYPE_HOST);
    md_attr->alloc_mem_types  = UCS_BIT(UCS_MEMORY_TYPE_HOST);
    md_attr->access_mem_types = UCS_BIT(UCS_MEMORY_TYPE_HOST);
    md_attr->detect_mem_types = UCS_BIT(UCS_MEMORY_TYPE_HOST);
    md_attr->dmabuf_mem_types = UCS_BIT(UCS_MEMORY_TYPE_HOST);
    md_attr->max_alloc        = SIZE_MAX;
    return (UCS_OK);
}

void uct_utofu_md_close(uct_md_h tl_md) {
    uct_utofu_md_t *md;
    int rc;
    ucs_debug("uct_utofu_md_close");
    md = ucs_derived_of(tl_md, uct_utofu_md_t); 
    rc = utofu_free_vcq(md->vcq_hdl);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("error on utofu_free_vcq rc=%d", rc);
    }
    free(md);
}

UCS_PROFILE_FUNC(ucs_status_t, uct_utofu_mem_reg,
				 (tl_md, address, length, params, memh_p),
				 uct_md_h tl_md, void *address, size_t length,
                         const uct_md_mem_reg_params_t *params,
                         uct_mem_h *memh_p) {
    uct_utofu_md_t *md;
    uct_utofu_rkey_t *rkey;
    int rc;
    md = ucs_derived_of(tl_md, uct_utofu_md_t);
    rkey = ucs_malloc(sizeof(*rkey), "uct_utofu_rkey_t");
    rkey->buf = address;
    rkey->length = length;
    rc = utofu_reg_mem(md->vcq_hdl, address, length, 0, &rkey->stadd);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_reg_mem failed with %d", rc);
        return UCS_ERR_INVALID_PARAM;
    }
    ucs_debug("utofu_reg_mem: VCQ_ID: %lu, buf: %p, size: %lu, stadd: %lu", md->vcq_id, rkey->buf, length, rkey->stadd);
    *memh_p = rkey;
    return UCS_OK;
}

UCS_PROFILE_FUNC(ucs_status_t, uct_utofu_mem_dereg,
				 (tl_md, params),
				 uct_md_h tl_md,
				 const uct_md_mem_dereg_params_t *params) {
    uct_utofu_md_t *md;
    uct_utofu_rkey_t *rkey;
    int rc;
    md = ucs_derived_of(tl_md, uct_utofu_md_t);
    rkey = (uct_utofu_rkey_t *)(params->memh);
    rc = utofu_dereg_mem(md->vcq_hdl, rkey->stadd, 0);
    if (rc != UTOFU_SUCCESS) {
        ucs_error("utofu_dereg_mem failed with %d", rc);
        return UCS_ERR_INVALID_PARAM;
    }
    ucs_debug("utofu_dereg_mem: VCQ ID: %lu, buf: %p, stadd: %lu", md->vcq_id, rkey->buf, rkey->stadd);
    return UCS_OK;
}

ucs_status_t uct_utofu_mkey_pack(uct_md_h md, uct_mem_h memh, void *address, size_t length,
                   const uct_md_mkey_pack_params_t *params,
                   void *mkey_buffer) {
    uct_utofu_rkey_t *rkey = memh; 
    memcpy(mkey_buffer, rkey, sizeof(*rkey));
    return (UCS_OK);
}

ucs_status_t uct_utofu_rkey_unpack(uct_component_t *component,
                                   const void *rkey_buffer,
                                   uct_rkey_t *rkey_p,
                                   void **handle_p) {
    uct_utofu_rkey_t *rkey;
    rkey = ucs_malloc(sizeof(*rkey), "uct_utofu_rkey_t");
    memcpy(rkey, rkey_buffer, sizeof(*rkey));
    *rkey_p = (uct_rkey_t)rkey;
    *handle_p = NULL;
    return (UCS_OK);
}

ucs_status_t uct_utofu_rkey_release(uct_component_t *component, uct_rkey_t tl_rkey, void *handle) {
    uct_utofu_rkey_t *rkey = (uct_utofu_rkey_t *)tl_rkey;
    free(rkey);
    return (UCS_OK);
}

uct_md_ops_t md_ops = {
    .close = uct_utofu_md_close,
    .query = uct_utofu_md_query,
    .mem_alloc = NULL,
    .mem_free = NULL,
    .mem_reg = uct_utofu_mem_reg,
    .mem_dereg = uct_utofu_mem_dereg,
    .mkey_pack = uct_utofu_mkey_pack,
    .detect_memory_type = NULL,
};

static __thread int md_num = 0;

ucs_status_t uct_utofu_md_open(uct_component_h component,
							   const char *md_name,
							   const uct_md_config_t *md_config,
							   uct_md_h *md_p) {
    int rc;
    uct_utofu_md_t *md;
    ucs_status_t status = UCS_OK;
    utofu_tni_id_t *tni_ids;
    size_t num_tnis;
    uint32_t cpu_id, numa_id;
    rc = utofu_get_onesided_tnis(&tni_ids, &num_tnis);
    if (rc != UTOFU_SUCCESS) {
		ucs_error("error on utofu_get_onesided_tnis rc=%d", rc);
		return UCS_ERR_UNSUPPORTED;
    }
    syscall(SYS_getcpu, &cpu_id, &numa_id, NULL, NULL);
    md = ucs_malloc(sizeof(*md), "uct_utofu_md_t");
    md->super.ops = &md_ops;
    md->super.component = &uct_utofu_component;
    md->tni_id = tni_ids[(cpu_id + (md_num++)) % num_tnis];
    free(tni_ids);
    rc = utofu_create_vcq(md->tni_id, 0, &md->vcq_hdl);
    if (rc != UTOFU_SUCCESS) {
		ucs_error("error on utofu_create_vcq rc=%d", rc);
        ucs_free(md);
		return UCS_ERR_UNSUPPORTED;
    }
    rc = utofu_query_vcq_id(md->vcq_hdl, &md->vcq_id);
    if (rc != UTOFU_SUCCESS) {
		ucs_error("error on utofu_query_vcq_id rc=%d", rc);
        utofu_free_vcq(md->vcq_hdl);
        ucs_free(md);
        return UCS_ERR_UNSUPPORTED;
    }
    //uct_utofu_md_config_t *config = NULL;
    //config = ucs_derived_of(md_config, uct_utofu_md_config_t); 
    *md_p = &md->super;
    ucs_debug("uct_utofu_md_open uct_md_h=%p tni_id=%d vcq_id=%zu\n", *md_p, md->tni_id, md->vcq_id); 
    return (status);
}

uct_component_t uct_utofu_component = {
    .query_md_resources = uct_utofu_query_md_resources,
    .md_open = uct_utofu_md_open,
    .cm_open = ucs_empty_function_return_unsupported,
    .rkey_unpack = uct_utofu_rkey_unpack,
    .rkey_ptr = NULL,
    .rkey_release = uct_utofu_rkey_release,
    .name = UCT_UTOFU_MD_NAME,
    .md_config = {
        .name = "UTOFU memory domain",
		.prefix = UCT_UTOFU_CONFIG_PREFIX,
		.table = uct_utofu_md_config_table,
		.size = sizeof(uct_utofu_md_config_t)
    },
    .cm_config =  UCS_CONFIG_EMPTY_GLOBAL_LIST_ENTRY,
    .tl_list = UCT_COMPONENT_TL_LIST_INITIALIZER(&uct_utofu_component),
    .flags = 0,
    .md_vfs_init = (uct_component_md_vfs_init_func_t)ucs_empty_function,
};

UCT_COMPONENT_REGISTER(&uct_utofu_component);
