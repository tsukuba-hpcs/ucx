#ifndef __UTOFU_DEF_H_INCLUDED__
#define __UTOFU_DEF_H_INCLUDED__

#include <uct/api/uct.h>
#include <uct/base/uct_iface.h>
#include <uct/base/uct_md.h>
#include <ucs/profile/profile.h>
#include <ucs/sys/string.h>

#include <utofu.h>

#define UCT_UTOFU_MD_NAME "utofu"
#define UCT_UTOFU_CONFIG_PREFIX "UTOFU_"
#define UCT_UTOFU_RINGBUF_ITEM_SIZE  (1024)
#define UCT_UTOFU_RINGBUF_ITEM_COUNT (128*1024)

typedef struct uct_utofu_iface_config {
    uct_iface_config_t super;
} uct_utofu_iface_config_t;

typedef struct uct_utofu_md uct_utofu_md_t;

struct uct_utofu_iface {
    uct_base_iface_t super;
    uct_utofu_md_t *md;
	ucs_arbiter_t arbiter;

    struct {
		ucs_mpool_t buf;
	} mp;

    // Active Message
    ucs_mpool_t am_desc_mpool;
    uint64_t am_rb_head;
    uint64_t am_rb_tail;
    utofu_stadd_t am_rb_tail_stadd;
    uint8_t *am_rb;
    utofu_stadd_t am_rb_stadd;
};

typedef struct uct_utofu_iface uct_utofu_iface_t;

typedef struct uct_utofu_iface_addr {
    utofu_vcq_id_t vcq_id;
    // Active Message
    utofu_stadd_t am_rb_tail_stadd;
    utofu_stadd_t am_rb_stadd;
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

typedef struct __attribute__((packed)) {
	utofu_stadd_t stadd;
	void          *buf;
    size_t        length;
} uct_utofu_rkey_t;

typedef struct uct_utofu_ep {
    uct_base_ep_t super;
    utofu_vcq_id_t vcq_id;
    utofu_stadd_t am_rb_tail_stadd;
    utofu_stadd_t am_rb_stadd;
    uct_utofu_iface_t *iface;
    ucs_arbiter_group_t arb_group;
} uct_utofu_ep_t;

typedef struct uct_utofu_ep_address {
	utofu_vcq_id_t vcq_id;
    utofu_stadd_t am_rb_tail_stadd;
    utofu_stadd_t am_rb_stadd;
} uct_utofu_ep_address_t;


ucs_status_t uct_utofu_ep_get_address(uct_ep_h tl_ep,	  
									  uct_ep_addr_t *addr);
ucs_status_t uct_utofu_iface_query(uct_iface_h tl_iface, uct_iface_attr_t *iface_attr);
ucs_status_t uct_utofu_iface_get_address(uct_iface_h tl_iface,
										 uct_iface_addr_t *addr);
void uct_utofu_ep_pending_purge(uct_ep_h tl_ep, uct_pending_purge_callback_t cb, void *arg);
ucs_status_t uct_utofu_ep_flush(uct_ep_h tl_ep, unsigned flags, uct_completion_t *comp);

typedef struct {
    uint8_t complete;
    uint8_t am_id;
    size_t length;
    uint8_t data[];
} uct_utofu_am_bcopy_buf;

ssize_t uct_utofu_ep_am_bcopy(uct_ep_h tl_ep,
                              uint8_t id,
                              uct_pack_callback_t pack_cb,
                              void *arg,
                              unsigned flags);
ucs_status_t uct_utofu_ep_get_bcopy(uct_ep_h ep,
                                    uct_unpack_callback_t unpack_cb,
                                    void *arg,
                                    size_t length,
                                    uint64_t remote_addr,
                                    uct_rkey_t rkey,
                                    uct_completion_t *comp);

UCS_CLASS_DECLARE(uct_utofu_ep_t, const uct_ep_params_t *);
UCS_CLASS_DECLARE_NEW_FUNC(uct_utofu_ep_t, uct_ep_t, const uct_ep_params_t *);
UCS_CLASS_DECLARE_DELETE_FUNC(uct_utofu_ep_t, uct_ep_t);
UCS_CLASS_DECLARE_DELETE_FUNC(uct_utofu_iface_t, uct_iface_t);


#endif
