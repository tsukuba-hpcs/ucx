#include "utofu_def.h"

static ucs_config_field_t uct_utofu_md_config_table[] = {
    {NULL}
};

uct_component_t uct_utofu_component = {
    .query_md_resources = NULL,
    .md_open = NULL,
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
    //.tl_list = UCT_COMPONENT_TL_LIST_INITIALIZER(&uct_utofu_component),
    .flags = 0,
    .md_vfs_init = NULL,
};

UCT_COMPONENT_REGISTER(&uct_utofu_component);
