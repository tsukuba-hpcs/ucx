#ifndef __UTOFU_DEF_H_INCLUDED__
#define __UTOFU_DEF_H_INCLUDED__

#include <uct/api/uct.h>
#include <uct/base/uct_iface.h>
#include <uct/base/uct_md.h>
#include <ucs/sys/string.h>

#include <utofu.h>

#define UCT_UTOFU_MD_NAME "utofu"
#define UCT_UTOFU_CONFIG_PREFIX "UTOFU_"

typedef struct uct_utofu_iface_config {
    uct_iface_config_t super;
} uct_utofu_iface_config_t;

typedef struct uct_utofu_md uct_utofu_md_t;

struct uct_utofu_iface {
    uct_base_iface_t super;
    uct_utofu_md_t *md;
};

typedef struct uct_utofu_iface uct_utofu_iface_t;

extern uct_component_t uct_utofu_component;

typedef struct uct_utofu_md_config {
    uct_md_config_t super;
} uct_utofu_md_config_t;

typedef struct uct_utofu_md {
    uct_md_t super;
    utofu_tni_id_t tni_id;
    utofu_vcq_hdl_t vcq_hdl;
    utofu_vcq_id_t vcq_id;
} uct_utofu_md_t;

#endif