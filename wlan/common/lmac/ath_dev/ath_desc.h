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

#ifndef ATH_DESC_H
#define ATH_DESC_H

#include "ah.h"
#include "ah_desc.h"
#include "sys/queue.h"

struct ath_softc;
struct ath_txq;

#define ATH_BF_MORE_MPDU    1   /* if there's more fragment for this MSDU */

struct ath_buf_state {
    int                     bfs_nframes;        /* # frames in aggregate */
    u_int16_t               bfs_al;             /* length of aggregate */
    u_int16_t               bfs_frmlen;         /* length of frame */
    int                     bfs_seqno;          /* sequence number */
    int                     bfs_tidno;          /* tid of this frame */
    int                     bfs_retries;        /* current retries */
    struct ath_rc_series    bfs_rcs[4];         /* rate series */
    int                     bfs_isdata: 1;      /* is a data frame/aggregate */
    int                     bfs_isaggr: 1;      /* is an aggregate */
    int                     bfs_isampdu: 1;     /* is an a-mpdu, aggregate or not */
    int                     bfs_ht: 1;          /* is an HT frame */
    int                     bfs_isretried: 1;   /* is retried */
    int                     bfs_isxretried: 1;  /* is excessive retried */
    int                     bfs_shpreamble: 1;  /* is short preamble */
    int                     bfs_isbar: 1;       /* is a BAR */
    int                     bfs_ispspoll: 1;    /* is a PS-Poll */
    int                     bfs_aggrburst: 1;   /* is a aggr burst */
    int                     bfs_calcairtime:1;  /* requests airtime be calculated when set for tx frame */
    int                     bfs_isn:1;          /* 11n frame */
#ifdef ATH_SUPPORT_UAPSD
    int                     bfs_qosnulleosp:1;  /* is QoS null EOSP frame */
#endif
#ifdef ATH_SWRETRY
    int                     bfs_isswretry: 1;   /* is the frame marked for swretry*/
    int                     bfs_swretries;      /* number of swretries made*/
    int                     bfs_totaltries;     /* total tries including hw retries*/
    int                     bfs_qnum;           /* h/w queue number */
#endif       
    int                     bfs_rifsburst_elem; /* RIFS burst/bar */
    int                     bfs_nrifsubframes;  /* # of elements in burst */
    HAL_KEY_TYPE            bfs_keytype;        /* key type use to encrypt this frame */
};

#define bf_nframes      bf_state.bfs_nframes
#define bf_al           bf_state.bfs_al
#define bf_frmlen       bf_state.bfs_frmlen
#define bf_retries      bf_state.bfs_retries
#define bf_seqno        bf_state.bfs_seqno
#define bf_tidno        bf_state.bfs_tidno
#define bf_rcs          bf_state.bfs_rcs
#define bf_isdata       bf_state.bfs_isdata
#define bf_isaggr       bf_state.bfs_isaggr
#define bf_isampdu      bf_state.bfs_isampdu
#define bf_ht           bf_state.bfs_ht
#define bf_isretried    bf_state.bfs_isretried
#define bf_isxretried   bf_state.bfs_isxretried
#define bf_shpreamble   bf_state.bfs_shpreamble
#define bf_rifsburst_elem  bf_state.bfs_rifsburst_elem
#define bf_nrifsubframes  bf_state.bfs_nrifsubframes
#define bf_keytype      bf_state.bfs_keytype
#define bf_isbar        bf_state.bfs_isbar
#define bf_ispspoll     bf_state.bfs_ispspoll
#define bf_aggrburst    bf_state.bfs_aggrburst
#define bf_calcairtime  bf_state.bfs_calcairtime
#define bf_isn          bf_state.bfs_isn
#ifdef ATH_SUPPORT_UAPSD
#define bf_qosnulleosp  bf_state.bfs_qosnulleosp
#endif
#ifdef ATH_SWRETRY
#define bf_isswretry    bf_state.bfs_isswretry
#define bf_swretries    bf_state.bfs_swretries
#define bf_totaltries   bf_state.bfs_totaltries
#define bf_qnum         bf_state.bfs_qnum
#endif
/*
 * Abstraction of a contiguous buffer to transmit/receive.  There is only
 * a single hw descriptor encapsulated here.
 */
struct ath_buf {
    TAILQ_ENTRY(ath_buf)    bf_list;

    struct ath_buf         *bf_lastbf;   /* last buf of this unit (a frame or an aggregate) */
    struct ath_buf         *bf_lastfrm;  /* last buf of this frame */
    struct ath_buf         *bf_next;     /* next subframe in the aggregate */
    struct ath_buf         *bf_rifslast; /* last buf for RIFS burst */
    void                   *bf_mpdu;     /* enclosing frame structure */
    void                   *bf_node;     /* pointer to the node */
    struct ath_desc        *bf_desc;     /* virtual addr of desc */
    dma_addr_t              bf_daddr;    /* physical addr of desc */
    dma_addr_t              bf_buf_addr; /* physical addr of data buffer */
    u_int32_t               bf_status;
    u_int16_t               bf_flags;    /* tx descriptor flags */
#ifdef ATH_SUPPORT_IQUE
	u_int32_t				bf_txduration;/* Tx duration of this buf */
#endif
	struct ath_buf_state    bf_state;    /* buffer state */

    OS_DMA_MEM_CONTEXT(bf_dmacontext)    /* OS Specific DMA context */
};

typedef TAILQ_HEAD(ath_bufhead_s, ath_buf) ath_bufhead;

#define ATH_TXBUF_RESET(_bf) do {               \
    (_bf)->bf_status = 0;                       \
    (_bf)->bf_lastbf = NULL;                    \
    (_bf)->bf_lastfrm = NULL;                   \
    (_bf)->bf_next = NULL;                      \
    OS_MEMZERO(&((_bf)->bf_state),              \
               sizeof(struct ath_buf_state));   \
} while(0)

#ifdef ATH_SWRETRY
#define ATH_TXBUF_SWRETRY_RESET(_bf) do {       \
    (_bf)->bf_swretries = 0;                    \
    (_bf)->bf_isswretry = 0;                    \
    (_bf)->bf_totaltries = 0;                   \
}while(0)                               
#endif                            

/*
 * reset the rx buffer.
 * any new fields added to the athbuf and require 
 * reset need to be added to this macro.
 * currently bf_status is the only one requires that
 * requires reset.
 */
#define ATH_RXBUF_RESET(_bf)    (_bf)->bf_status=0

#define ATH_BUFSTATUS_DONE      0x00000001  /* hw processing complete, desc processed by hal */
#define ATH_BUFSTATUS_STALE     0x00000002  /* hw processing complete, desc hold for hw */
#define ATH_BUFSTATUS_FREE      0x00000004  /* Rx-only: OS is done with this packet and it's ok to queued it to hw */
#ifdef ATH_SWRETRY
#define ATH_BUFSTATUS_MARKEDSWRETRY 0x00000008 /*Marked for swretry, do not stale it*/
#endif
/*
 * DMA state for tx/rx descriptors.
 */
struct ath_descdma {
    const char*         dd_name;
    struct ath_desc    *dd_desc;        // descriptors 
    dma_addr_t          dd_desc_paddr;  // physical addr of dd_desc 
    u_int32_t           dd_desc_len;    // size of dd_desc 
    struct ath_buf     *dd_bufptr;      // associated buffers 

    OS_DMA_MEM_CONTEXT(dd_dmacontext)   // OS Specific DMA context
};

/*
 * Abstraction of a received RX MPDU/MMPDU, or a RX fragment
 */
struct ath_rx_context {
    struct ath_buf          *ctx_rxbuf;     /* associated ath_buf for rx */
};

#define ATH_RX_CONTEXT(_wbuf)               ((struct ath_rx_context *)wbuf_get_context(_wbuf))

/* MPDU/Descriptor API's */
int
ath_descdma_setup(
    struct ath_softc *sc,
    struct ath_descdma *dd, ath_bufhead *head,
    const char *name, int nbuf, int ndesc);


int ath_desc_alloc(struct ath_softc *sc);
void ath_desc_free(struct ath_softc *sc);

/*
 * Athero's extension to TAILQ macros
 */
#ifndef TAILQ_CONCAT

#define	TAILQ_CONCAT(head1, head2, field) do {				\
	if (!TAILQ_EMPTY(head2)) {					\
		*(head1)->tqh_last = (head2)->tqh_first;		\
		(head2)->tqh_first->field.tqe_prev = (head1)->tqh_last;	\
		(head1)->tqh_last = (head2)->tqh_last;			\
		TAILQ_INIT((head2));					\
	}								\
} while (0)

#endif

#define	TAILQ_REMOVE_HEAD_UNTIL(head1, head2, elm, field) do {              \
	TAILQ_FIRST(head2) = TAILQ_FIRST(head1);                                \
	TAILQ_FIRST(head1)->field.tqe_prev = &TAILQ_FIRST(head2);               \
	if ((TAILQ_FIRST(head1) = TAILQ_NEXT((elm), field)) == NULL)            \
		(head1)->tqh_last = &TAILQ_FIRST(head1);                            \
	else                                                                    \
		TAILQ_NEXT((elm), field)->field.tqe_prev = &TAILQ_FIRST(head1);     \
	TAILQ_NEXT((elm), field) = NULL;                                        \
	(head2)->tqh_last = &TAILQ_NEXT((elm), field);                          \
} while (0)

#define TAILQ_REMOVE_HEAD_BEFORE(head1, head2, elm, headname, field) do {   \
	if (TAILQ_PREV((elm), headname, field) == NULL) {                       \
		TAILQ_INIT(head2);                                                  \
	} else {                                                                \
		TAILQ_FIRST(head2) = TAILQ_FIRST(head1);                            \
		(head2)->tqh_last = (elm)->field.tqe_prev;                          \
		*((head2)->tqh_last) = NULL;                                        \
		TAILQ_FIRST(head1)->field.tqe_prev = &TAILQ_FIRST(head2);           \
		TAILQ_FIRST(head1) = (elm);                                         \
		(elm)->field.tqe_prev = &TAILQ_FIRST(head1);                        \
	}                                                                       \
} while (0)

#define TAILQ_INSERTQ_HEAD(head, tq, field) do {                            \
	if ((head)->tqh_first) {                                                \
		*(tq)->tqh_last = (head)->tqh_first;                                \
		(head)->tqh_first->field.tqe_prev = (tq)->tqh_last;                 \
	} else {                                                                \
		(head)->tqh_last = (tq)->tqh_last;                                  \
	}                                                                       \
	(head)->tqh_first = (tq)->tqh_first;                                    \
	(tq)->tqh_first->field.tqe_prev = &(head)->tqh_first;                   \
} while (0)

#endif /* end of ATH_DESC_H */
