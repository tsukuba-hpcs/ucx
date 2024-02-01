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

struct uct_utofu_iface {
    uct_base_iface_t super;
};

typedef struct uct_utofu_iface uct_utofu_iface_t;

extern uct_component_t uct_utofu_component;

typedef struct uct_utofu_md_config {
    uct_md_config_t super;
} uct_utofu_md_config_t;

typedef struct uct_utofu_md {
    uct_md_t super;
} uct_utofu_md_t;

ucs_status_t uct_utofu_query_devices(uct_md_h md,
									 uct_tl_device_resource_t **tl_devices_p,
									 unsigned *num_tl_devices_p);

#endif