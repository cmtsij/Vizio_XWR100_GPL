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
 * 
 */

/*
 * Defintions for the Atheros Wireless LAN controller driver.
 */
#ifndef _DEV_ATH_ATHVAR_H
#define _DEV_ATH_ATHVAR_H

#include <osdep.h>

#include "ieee80211_interfaces.h"
#include "ieee80211_proto.h"

#include "ath_dev.h"
#include "ath_internal.h"
#include "if_ath_limit_legacy.h"

struct ath_softc_net80211 {
    struct ieee80211com     sc_ic;      /* NB: base class, must be first */
    ath_dev_t               sc_dev;     /* Atheros device handle */
    struct ath_ops          *sc_ops;    /* Atheros device callback table */

    void                    (*sc_node_cleanup)(struct ieee80211_node *);
    void                    (*sc_node_free)(struct ieee80211_node *);

    osdev_t                 sc_osdev;   /* handle to use OS-independent services */
    int                     sc_debug;

    int                     sc_nstavaps; /* # of station vaps */
    u_int                   sc_syncbeacon:1;    /* sync/resync beacon timers */
    u_int                   sc_isscan:1;        /* scanning */
    u_int                   sc_splitmic:1;      /* split TKIP MIC keys */
    u_int                   sc_mcastkey:1;      /* mcast key cache search */

    int                     sc_ac2q[WME_NUM_AC];/* WMM AC -> h/w qnum */
    int                     sc_uapsd_qnum;      /* UAPSD h/w qnum */
    int                     sc_beacon_qnum;     /* beacon h/w qnum */
    u_int                   sc_fftxqmin;        /* aggregation threshold */
    u_int32_t               cached_ic_flags;    
    
    struct ieee80211_node	*sc_keyixmap[ATH_KEYMAX];/* key ix->node map */
    
    struct ath_cwm          *sc_cwm;            /* Channel Width Management */
    
#ifdef ATH_TX99_DIAG
    struct ath_txrx99       sc_txrx99;
#endif
    TAILQ_HEAD(ath_amsdu_txq,ath_amsdu_tx)    sc_amsdu_txq;    /* amsdu tx requests */
    struct ath_timer        sc_amsdu_flush_timer; /* amsdu flush timer */
    spinlock_t              amsdu_txq_lock;  /* amsdu spinlock */

    TAILQ_HEAD(ath_limit_legacy_txq, ath_limit_legacy_buf)    sc_limit_legacy_txq; /* holdingQ for legacy frames */
    TAILQ_HEAD(ath_limit_legacy_freeq, ath_limit_legacy_buf)    sc_limit_legacy_freeq; /* freeQ to feed holdingQ */
    int                     sc_limit_legacy_txq_count;/* holdingQ count*/    
    os_timer_t              sc_limit_legacy_flush_timer; /* holdingQ flush timer */
    spinlock_t              sc_limit_legacy_txq_lock;  /* holdingQ spinlock */
    spinlock_t              sc_limit_legacy_freeq_lock;  /* freeQ spinlock */
};

#define ATH_SOFTC_NET80211(_ic)     ((struct ath_softc_net80211 *)(_ic))

struct ath_vap_net80211 {
    struct ieee80211vap         av_vap;     /* NB: base class, must be first */
    struct ath_softc_net80211   *av_sc;     /* back pointer to softc */
    int                         av_if_id;   /* interface id */

    struct ieee80211_beacon_offsets av_beacon_offsets;

    int                         (*av_newstate)(struct ieee80211vap *,
                                               enum ieee80211_state, int);
};
#define ATH_VAP_NET80211(_vap)      ((struct ath_vap_net80211 *)(_vap))

/* 
 * Units in kbps for the an_lasttxrate and an_lastrxrate fields. These 
 * two fields are only 16 bits wide and the max. rate is 600,000 kbps.
 * LAST_RATE_UNITS is used to scale these two fields to fit into 16 bits.
 */
#define LAST_RATE_UNITS     16

struct ath_node_net80211 {
    struct ieee80211_node       an_node;     /* NB: base class, must be first */
    ath_node_t                  an_sta;
    struct ath_ff               *an_ff;       /* fast frame buf */
    struct ath_amsdu            *an_amsdu;    /* AMSDU frame buf */
    int32_t      an_avgbrssi;    /* average beacon rssi */
    int32_t      an_avgrssi;     /* average rssi of all received frames */
    int32_t      an_avgchainrssi[ATH_MAX_ANTENNA]; /* avg rssi of all rx frames per antenna */
    int32_t      an_avgchainrssiext[ATH_MAX_ANTENNA]; /* avg rssi of all rx on ext chan */
    int32_t      an_avgtxrssi;   /* average aggr rssi over all tx acks */
    int32_t      an_avgtxchainrssi[ATH_MAX_ANTENNA];/* avg rssi of all tx acks per antenna */
    int32_t      an_avgtxchainrssiext[ATH_MAX_ANTENNA];/* avg rssi of all tx acks on ext chan */
    u_int32_t    an_lasttxrate;  /* last packet tx rate (LAST_RATE_UNITS Kbps) */
    u_int32_t    an_lastrxrate;  /* last packet rx rate (LAST_RATE_UNITS Kbps) */
    u_int32_t    an_avgrxrate;   /* average rx rate (Kbps) */
};

#if event-mechanism
enum ath_event_type {
    ATH_DFS_WAIT_CLEAR_EVENT,
    ATH_END_EVENT,
};

typedef struct ath_net80211_event {

    enum ath_event_type ath_event_id;
    void *ath_event_data;
} ATH_NET80211_EVENT;
#endif

#define ATH_NODE_NET80211(_ni)      ((struct ath_node_net80211 *)(_ni))

#define NET80211_HANDLE(_ieee)      ((struct ieee80211com *)(_ieee))

int ath_attach(u_int16_t devid, void *base_addr, struct ath_softc_net80211 *scn, osdev_t osdev,
               struct ath_reg_parm *ath_conf_parm, struct hal_reg_parm *hal_conf_parm);
int ath_detach(struct ath_softc_net80211 *scn);
int ath_resume(struct ath_softc_net80211 *scn);
int ath_newstate(struct ieee80211vap *vap, enum ieee80211_state nstate, int arg);
int ath_net80211_rx(ieee80211_handle_t ieee, wbuf_t wbuf, ieee80211_rx_status_t *rx_status, u_int16_t keyix);
int ath_get_netif_settings(ieee80211_handle_t);
void ath_mcast_merge(ieee80211_handle_t, u_int32_t mfilt[2]);
int ath_tx_send(wbuf_t wbuf);
int ath_tx_mgt_send(struct ieee80211com *ic, wbuf_t wbuf);
int ath_tx_prepare(struct ath_softc_net80211 *scn, wbuf_t wbuf, int nextfraglen, ieee80211_tx_control_t *txctl);
WIRELESS_MODE ath_ieee2wmode(enum ieee80211_phymode mode);
//int16_t ath_net80211_get_noisefloor(struct ieee80211com *ic, struct ieee80211_channel *chan);
void ath_net80211_dfs_test_return(ieee80211_handle_t ieee, u_int8_t ieeeChan);
void ath_net80211_mark_dfs(ieee80211_handle_t, struct ieee80211_channel *);
void ath_net80211_change_channel(ieee80211_handle_t ieee, struct ieee80211_channel *chan);
void ath_net80211_switch_mode_static20(ieee80211_handle_t ieee);
void ath_net80211_switch_mode_dynamic2040(ieee80211_handle_t ieee);
u_int8_t * ath_net80211_get_macaddr(ieee80211_node_t node);
#ifdef NODE_FREE_DEBUG
void ath_net80211_node_print(ieee80211_node_t node);
void ath_net80211_add_trace(ieee80211_node_t node, char *funcp, char *descp,
                            u_int64_t value);
#endif

#ifdef ATHR_RNWF
#define ATH_DEFINE_TXCTL(_txctl, _wbuf)    \
    ieee80211_tx_control_t *_txctl = (ieee80211_tx_control_t *)wbuf_get_context(_wbuf)
#else
#define ATH_DEFINE_TXCTL(_txctl, _wbuf)    \
    ieee80211_tx_control_t ltxctl;         \
    ieee80211_tx_control_t *_txctl = &ltxctl
#endif

#endif /* _DEV_ATH_ATHVAR_H  */
