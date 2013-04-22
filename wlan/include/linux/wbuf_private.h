/*
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef _WBUF_LINUX_H
#define _WBUF_LINUX_H

/*
 * Linux Definition and Implementation for wbuf
 */
#include <osdep.h>

typedef struct sk_buff *__wbuf_t;

/*
 * WBUF private API's for Linux
 */

/*
 * NB: This function shall only be called for wbuf
 * with type WBUF_RX or WBUF_RX_INTRENAL.
 */
#define __wbuf_init(_skb, _pktlen)  do {    \
    skb_put(_skb, _pktlen);                 \
    (_skb)->protocol = ETH_P_CONTROL;       \
} while (0)

#define __wbuf_header(_skb)                 ((_skb)->data)

/*
 * NB: The following two API's only work when skb's header
 * has not been ajusted, i.e., no one calls skb_push or skb_pull
 * on this skb yet.
 */
#define __wbuf_raw_data(_skb)               ((_skb)->data)
#define __wbuf_get_len(_skb)                skb_tailroom(_skb)

#define __wbuf_get_pktlen(_skb)             ((_skb)->len)
#define __wbuf_get_tailroom(_skb)           skb_tailroom(_skb)
#define __wbuf_get_priority(_skb)           ((_skb)->priority)
#define __wbuf_set_priority(_skb, _p)       ((_skb)->priority = (_p))

#define __wbuf_hdrspace(_skb)               skb_headroom(_skb)

#define __wbuf_next(_skb)                   ((_skb)->next)
#define __wbuf_set_next(_skb,_next)         ((_skb)->next = (_next))

#define __wbuf_complete(_skb)               dev_kfree_skb(_skb)
#define __wbuf_free(_skb)                   dev_kfree_skb(_skb)
#define __wbuf_push(_skb, _len)             skb_push(_skb, _len)
#define __wbuf_clone(_skb)                  skb_clone(_skb, GFP_ATOMIC)
#define __wbuf_trim(_skb, _len)             skb_trim(_skb, ((_skb)->len - (_len)))
#define __wbuf_pull(_skb, _len)             skb_pull(_skb, _len)
#define __wbuf_set_age(_skb,v)              ((_skb)->csum = v)
#define __wbuf_get_age(_skb)                ((_skb)->csum)

#define __wbuf_copydata(_skb, _offset, _len, _to)   \
    skb_copy_bits((_skb), (_offset), (_to), (_len))

struct ieee80211_node * __wbuf_get_node(struct sk_buff *skb);
void __wbuf_set_node(struct sk_buff *skb, struct ieee80211_node *ni);
void * __wbuf_get_context(struct sk_buff *skb);
int __wbuf_get_tid(struct sk_buff *skb);
void __wbuf_set_tid(struct sk_buff *skb, u_int8_t tid);
int __wbuf_is_eapol(struct sk_buff *skb);
void __wbuf_set_amsdu(struct sk_buff *skb);
int __wbuf_is_amsdu(struct sk_buff *skb);
void __wbuf_set_fastframe(struct sk_buff *skb);
int __wbuf_is_fastframe(struct sk_buff *skb);
void __wbuf_set_pwrsaveframe(struct sk_buff *skb);
int __wbuf_is_pwrsaveframe(struct sk_buff *skb);
int __wbuf_is_smpsframe(struct sk_buff *skb);
void __wbuf_set_qosframe(struct sk_buff *skb);
int __wbuf_is_qosframe(struct sk_buff *skb);
int __wbuf_get_exemption_type(__wbuf_t wbuf);
void __wbuf_set_exemption_type(__wbuf_t wbuf, int type);
#ifdef ATH_SUPPORT_UAPSD
int  __wbuf_is_uapsd(struct sk_buff *skb);
void __wbuf_set_uapsd(struct sk_buff *skb);
void __wbuf_clear_uapsd(struct sk_buff *skb);
#endif

void __wbuf_map_sg(osdev_t osdev, struct sk_buff *skb, dma_context_t context, void *arg);
void __wbuf_unmap_sg(osdev_t osdev, struct sk_buff *skb, dma_context_t context);
dma_addr_t __wbuf_map_single(osdev_t osdev, struct sk_buff *skb, int direction, dma_context_t context);
void __wbuf_unmap_single(osdev_t osdev, struct sk_buff *skb, int direction, dma_context_t context);

int wbuf_start_dma(struct sk_buff *skb, sg_t *sg, u_int32_t n_sg, void *arg);

static INLINE int
__wbuf_append(struct sk_buff *skb, u_int16_t size)
{
    skb_put(skb, size);
    return 0;
}

static INLINE __wbuf_t 
__wbuf_coalesce(osdev_t os_handle, struct sk_buff *skb)
{
    /* The sk_buff is already contiguous in memory, no need to coalesce. */
    return skb;
}

#endif
