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

typedef struct uct_utofu_iface_addr {
    utofu_vcq_id_t vcq_id;
} uct_utofu_iface_addr_t;

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

typedef struct uct_utofu_ep {
    uct_base_ep_t super;
    utofu_vcq_id_t peer_vcq_id;
    uct_utofu_iface_t *iface;
} uct_utofu_ep_t;

typedef struct uct_utofu_ep_address {
	utofu_vcq_id_t vcq_id;
} uct_utofu_ep_address_t;


ucs_status_t uct_utofu_ep_get_address(uct_ep_h tl_ep,	  
									  uct_ep_addr_t *addr);
ucs_status_t uct_utofu_iface_query(uct_iface_h tl_iface, uct_iface_attr_t *iface_attr);
ucs_status_t uct_utofu_iface_get_address(uct_iface_h tl_iface,
										 uct_iface_addr_t *addr);
ucs_status_t uct_utofu_ep_flush(uct_ep_h tl_ep, unsigned flags, uct_completion_t *comp);

UCS_CLASS_DECLARE(uct_utofu_ep_t, const uct_ep_params_t *);
UCS_CLASS_DECLARE_NEW_FUNC(uct_utofu_ep_t, uct_ep_t, const uct_ep_params_t *);
UCS_CLASS_DECLARE_DELETE_FUNC(uct_utofu_ep_t, uct_ep_t);
UCS_CLASS_DECLARE_DELETE_FUNC(uct_utofu_iface_t, uct_iface_t);


#endif