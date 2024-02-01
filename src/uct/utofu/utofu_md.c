#include "utofu_def.h"

static ucs_config_field_t uct_utofu_md_config_table[] = {
    {NULL}
};

ucs_status_t uct_utofu_query_md_resources(uct_component_h component,
										  uct_md_resource_desc_t **resources_p,
										  unsigned *num_resources_p) {
    uct_md_resource_desc_t *resources;
	unsigned int num_devices = 1;

	resources = ucs_calloc(num_devices, sizeof(*resources), "uct_md_resource_desc_t");

	for (unsigned int i=0; i<num_devices; i++) {
		ucs_snprintf_zero(resources[i].md_name, sizeof(resources[i].md_name), "utofu");
	}

	*num_resources_p = num_devices;
	*resources_p = resources;

	return UCS_OK;
}

ucs_status_t uct_utofu_md_query(uct_md_h tl_md, 
								uct_md_attr_t *md_attr)
{
    //uct_utofu_md_t *md = ucs_derived_of(tl_md, uct_utofu_md_t);

    return (UCS_OK);
}

uct_md_ops_t md_ops = {
    .close = ucs_empty_function,
    .query = uct_utofu_md_query,
    .mem_alloc = NULL,
    .mem_free = NULL,
    .mem_reg = NULL,
    .mem_dereg = NULL,
    .mkey_pack = NULL,
    .detect_memory_type = NULL,
};

uct_utofu_md_t md = {
    .super = {
        .ops = &md_ops,
        .component = &uct_utofu_component,
    },
};

ucs_status_t uct_utofu_md_open(uct_component_h component,
							   const char *md_name,
							   const uct_md_config_t *md_config,
							   uct_md_h *md_p) {
    ucs_status_t status = UCS_OK;

    //uct_utofu_md_config_t *config = NULL;

    //config = ucs_derived_of(md_config, uct_utofu_md_config_t);
    
    *md_p = &md.super;

    return (status);
}

uct_component_t uct_utofu_component = {
    .query_md_resources = uct_utofu_query_md_resources,
    .md_open = uct_utofu_md_open,
    .cm_open = NULL,
    .rkey_unpack = NULL,
    .rkey_ptr = NULL,
    .rkey_release = NULL,
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
