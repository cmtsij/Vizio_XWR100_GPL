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
 * Definitions for the ATH layer internal API's.
 */
#ifndef ATH_INTERNAL_H
#define ATH_INTERNAL_H

#include <osdep.h>

#include "ah.h"
#include "if_athioctl.h"
#include "if_athrate.h"
#include "ath_desc.h"

/*
 * XXX: ath layer still use some IEEE80211_XXX macros,
 * need to resolve those across layer references.
 */
#include <ieee80211_var.h>

#include "ath_dev.h"
#include "ath_led.h"
#include "ath_power.h"
#include "ath_rfkill.h"
#include "ath_cfg.h"
#include "ath_ppm.h"
#include "ath_rb.h"
#include "ath_antdiv.h"

#if defined(SLOW_ANT_DIV) || defined(AP_SLOW_ANT_DIV)
#include "ath_antdiv.h"
#endif
#ifdef ATH_BT_COEX
#include "ath_bt.h"
#endif

#ifdef ATH_SUPPORT_DFS

struct ath_dfs;

#define ATH_DFS_HANG_WAR_MIN_TIMEOUT_S   30 // 30 seconds
#define ATH_DFS_HANG_WAR_MIN_TIMEOUT_MS  (ATH_DFS_HANG_WAR_MIN_TIMEOUT_S * 1000)
#define ATH_DFS_HANG_WAR_MIN_TIMEOUT_US  (ATH_DFS_HANG_WAR_MIN_TIMEOUT_MS * 1000)
#define ATH_DFS_HANG_WAR_MAX_TIMEOUT_S   300 // 5 minutes
#define ATH_DFS_HANG_WAR_MAX_TIMEOUT_MS  (ATH_DFS_HANG_WAR_MAX_TIMEOUT_S * 1000)
#define ATH_DFS_HANG_WAR_MAX_TIMEOUT_US  (ATH_DFS_HANG_WAR_MAX_TIMEOUT_MS * 1000)
#define ATH_DFS_HANG_FREQ_S              120 // once every 2 minutes
#define ATH_DFS_HANG_FREQ_MS             (ATH_DFS_HANG_FREQ_S * 1000)
#define ATH_DFS_HANG_FREQ_US             (ATH_DFS_HANG_FREQ_MS * 1000)

#endif
/*
 * Macro to expand scalars to 64-bit objects
 */
#define	ito64(x) (sizeof(x)==8) ? (((unsigned long long int)(x)) & (0xff)) : \
		 (sizeof(x)==16) ? (((unsigned long long int)(x)) & 0xffff) : \
		 ((sizeof(x)==32) ? (((unsigned long long int)(x)) & 0xffffffff): (unsigned long long int)(x))


#define ATH_TXMAXTRY               13     /* max number of transmit attempts (tries) */
#define ATH_11N_TXMAXTRY           10     /* max number of 11n transmit attempts (tries) */
#define ATH_MGT_TXMAXTRY            4     /* max number of tries for management and control frames */

typedef enum {
/* MCS Rates */
	ATH_MCS0 = 0x80,
	ATH_MCS1,
	ATH_MCS2,
	ATH_MCS3,
	ATH_MCS4,
	ATH_MCS5,
	ATH_MCS6,
	ATH_MCS7,
	ATH_MCS8,
	ATH_MCS9,
	ATH_MCS10,
	ATH_MCS11,
	ATH_MCS12,
	ATH_MCS13,
	ATH_MCS14,
	ATH_MCS15,
} ATH_MCS_RATES;

// Do we need to enable the DFS BB hang SW WAR ? Check for DFS channel.
#define ATH_DFS_BB_HANG_WAR_REQ(_sc) \
	(((_sc)->sc_hang_war & HAL_DFS_BB_HANG_WAR) && \
	 ((_sc)->sc_curchan.privFlags & CHANNEL_DFS))
// Do we need to enable the RIFS BB hang SW WAR ?
#define ATH_RIFS_BB_HANG_WAR_REQ(_sc) \
	((_sc)->sc_hang_war & HAL_RIFS_BB_HANG_WAR)
// Do we need to enable the RIFS BB hang early reset SW WAR ?
// If disabling RIFS when TKIP/WEP enabled didn't work, preemptively reset.
#define ATH_RIFS_BB_HANG_RESET_WAR_REQ(_sc) \
	(ATH_RIFS_BB_HANG_WAR_REQ(_sc) &&   \
	 !(ath_hal_bb_rifs_rx_enabled((_sc)->sc_ah)))
// Do we need to enable the BB rx_clear stuck low hang SW WAR ?
#define ATH_BB_RX_STUCK_LOW_WAR_REQ(_sc) \
	(((_sc)->sc_hang_war & HAL_RX_STUCK_LOW_BB_HANG_WAR) && \
	 ((_sc)->sc_noise >= 3))
// Do we need a generic BB hang SW WAR?
#define ATH_BB_GENERIC_HANG_WAR_REQ(_sc) \
	(ATH_BB_RX_STUCK_LOW_WAR_REQ(_sc) || \
	 ATH_RIFS_BB_HANG_RESET_WAR_REQ(_sc))
// Do we need to enable either the RIFS early reset or DFS BB hang SW WAR ?
#define ATH_BB_HANG_WAR_REQ(_sc) \
	(ATH_DFS_BB_HANG_WAR_REQ(_sc) || \
	 ATH_BB_GENERIC_HANG_WAR_REQ(_sc))
// Do we need to enable the MAC hang SW WAR ?
#define ATH_MAC_HANG_WAR_REQ(_sc) \
	((_sc)->sc_hang_war & HAL_MAC_HANG_WAR)
// Do we need to enable any hang SW WAR ?
#define ATH_HANG_WAR_REQ(_sc) \
	(ATH_BB_HANG_WAR_REQ(_sc) || \
	 ATH_MAC_HANG_WAR_REQ(_sc))
// We've found a BB hang, increment stats, and note the hang
#define ATH_BB_GENERIC_HANG(_sc)	\
do {						\
	(_sc)->sc_stats.ast_bb_hang++;		\
	(_sc)->sc_hang_war |= HAL_BB_HANG_DETECTED;	\
} while (0)
// We've found a MAC hang, increment stats, and note the hang
#define ATH_MAC_GENERIC_HANG(_sc)	\
do {						\
	(_sc)->sc_stats.ast_mac_hang++;		\
	(_sc)->sc_hang_war |= HAL_MAC_HANG_DETECTED;	\
} while (0)
// We've detected a BB hang
#define  ATH_BB_GENERIC_HANG_DETECTED(_sc)	\
	((_sc)->sc_hang_war & HAL_BB_HANG_DETECTED)
// We've detected a MAC hang
#define  ATH_MAC_GENERIC_HANG_DETECTED(_sc)	\
	((_sc)->sc_hang_war & HAL_MAC_HANG_DETECTED)
// Do we need to reset the HW?
#define ATH_HANG_DETECTED(_sc) \
	(ATH_BB_GENERIC_HANG_DETECTED(_sc) || \
	 ATH_MAC_GENERIC_HANG_DETECTED(_sc))
// Unset all hangs detected
#define ATH_CLEAR_HANGS(_sc)	\
	((_sc)->sc_hang_war &= ~(HAL_BB_HANG_DETECTED | HAL_MAC_HANG_DETECTED))

#define ATH_RSSI_EP_MULTIPLIER     (1<<7)  /* pow2 to optimize out * and / */
#define ATH_RATE_EP_MULTIPLIER     (1<<7)  /* pow2 to optimize out * and / */

#define ATH_RSSI_LPF_LEN           10
#define ATH_RSSI_DUMMY_MARKER      0x127
#define ATH_EP_MUL(x, mul)         ((x) * (mul))
#define ATH_EP_RND(x, mul)         ((((x)%(mul)) >= ((mul)/2)) ? ((x) + ((mul) - 1)) / (mul) : (x)/(mul))

#define	ATH_DEFAULT_NOISE_FLOOR     -95
#define RSSI_LPF_THRESHOLD          -20

#define ATH_RSSI_OUT(x)            (((x) != ATH_RSSI_DUMMY_MARKER) ? (ATH_EP_RND((x), ATH_RSSI_EP_MULTIPLIER)) : ATH_RSSI_DUMMY_MARKER)
#define ATH_RSSI_IN(x)             (ATH_EP_MUL((x), ATH_RSSI_EP_MULTIPLIER))
#define ATH_LPF_RSSI(x, y, len) \
    ((x != ATH_RSSI_DUMMY_MARKER) ? (((x) * ((len) - 1) + (y)) / (len)) : (y))
#define ATH_RSSI_LPF(x, y) do {                     \
    if ((y) >= RSSI_LPF_THRESHOLD)                         \
        x = ATH_LPF_RSSI((x), ATH_RSSI_IN((y)), ATH_RSSI_LPF_LEN);  \
} while (0)
#define ATH_ABS_RSSI_LPF(x, y) do {                     \
    if ((y) >= (RSSI_LPF_THRESHOLD + ATH_DEFAULT_NOISE_FLOOR))      \
        x = ATH_LPF_RSSI((x), ATH_RSSI_IN((y)), ATH_RSSI_LPF_LEN);  \
} while (0)

#define ATH_RATE_LPF_LEN           10          // Low pass filter length for averaging rates
#define ATH_RATE_DUMMY_MARKER      0
#define ATH_RATE_OUT(x)            (((x) != ATH_RATE_DUMMY_MARKER) ? (ATH_EP_RND((x), ATH_RATE_EP_MULTIPLIER)) : ATH_RATE_DUMMY_MARKER)
#define ATH_RATE_IN(x)             (ATH_EP_MUL((x), ATH_RATE_EP_MULTIPLIER))
#define ATH_LPF_RATE(x, y, len) \
    (((x) != ATH_RATE_DUMMY_MARKER) ? (((x) * ((len) - 1) + (y)) / (len)) : (y))
#define ATH_RATE_LPF(x, y) \
    ((x) = ATH_LPF_RATE((x), ATH_RATE_IN((y)), ATH_RATE_LPF_LEN))
#define ATH_IS_SINGLE_CHAIN(x)      ((x & (x - 1)) == 0)
#ifdef BUILD_AP
#define	ATH_ANI_POLLINTERVAL    100     /* 100 milliseconds between ANI poll */
#define	ATH_SHORT_CALINTERVAL   100     /* 100 milliseconds between calibrations */
#define	ATH_LONG_CALINTERVAL    30000   /* 30 seconds between calibrations */
#define	ATH_RESTART_CALINTERVAL 1200000 /* 20 minutes between calibrations */
#else
#define	ATH_ANI_POLLINTERVAL    100     /* 100 milliseconds between ANI poll */
#define	ATH_SHORT_CALINTERVAL   1000    /* 1 second between calibrations */
#define	ATH_LONG_CALINTERVAL    30000   /* Station always uses 30 seconds between calibrations */
#define	ATH_RESTART_CALINTERVAL 1200000 /* 20 minutes between calibrations */
#endif


#define ATH_TXPOWER_MAX         100 /* .5 dBm units */

#define ATH_REGCLASSIDS_MAX     10

#define	ATH_BCBUF	8		/* number of beacon buffers */

#define TAIL_DROP_COUNT 50             /* maximum number of queued frames allowed */
#define ATH_CABQ_READY_TIME      80 /* % of beacon interval */

/*
 * dynamic turbo specific macros.
 */

#define ATH_TURBO_UP_THRESH 750000 /* bytes/sec */ 
#define ATH_TURBO_DN_THRESH 1000000 /* bytes/sec */ 
#define ATH_TURBO_PERIOD_HOLD 1 /*in seconds */ 

/*
 * The only case where we see skbuff chains is due to FF aggregation in
 * the driver.
 */
#ifdef ATH_SUPERG_FF
#define ATH_FF_TXQMIN 2
#define	ATH_TXDESC	1		/* number of descriptors per buffer */
#else
#define	ATH_TXDESC	1		/* number of descriptors per buffer */
#endif

/* Compress settings */
#define ATH_COMP_THRESHOLD  256         /* no compression for frames
					   longer than this threshold  */
#define ATH_COMP_PROC_NO_COMP_NO_CCS    3
#define ATH_COMP_PROC_NO_COMP_ADD_CCS   2
#define ATH_COMP_PROC_COMP_NO_OPTIAML   1
#define ATH_COMP_PROC_COMP_OPTIMAL      0
#define ATH_DEFAULT_COMP_PROC           ATH_COMP_PROC_COMP_OPTIMAL

#define INVALID_DECOMP_INDEX		0xFFFF

#define WEP_IV_FIELD_SIZE       4       /* wep IV field size */
#define WEP_ICV_FIELD_SIZE      4       /* wep ICV field size */
#define AES_ICV_FIELD_SIZE      8       /* AES ICV field size */
#define EXT_IV_FIELD_SIZE       4       /* ext IV field size */

/* XR specific macros */

#define XR_DEFAULT_GRPPOLL_RATE_STR "0.25 1 1 3 3 6 6 20"
#define GRPPOLL_RATE_STR_LEN  64 
#define XR_SLOT_DELAY         30      // in usec
#define XR_AIFS               0
#define XR_NUM_RATES          5
#define XR_NUM_SUP_RATES      8
/* XR uplink should have same cwmin/cwmax value */
#define XR_CWMIN_CWMAX              7

#define XR_DATA_AIFS    3
#define XR_DATA_CWMIN   31 
#define XR_DATA_CWMAX   1023 

/* pick the threshold so that we meet most of the regulatory constraints */
#define XR_FRAGMENTATION_THRESHOLD            540
#define XR_TELEC_FRAGMENTATION_THRESHOLD      442

#define XR_MAX_GRP_POLL_PERIOD             1000 /* Maximum Group Poll Periodicity */

#define XR_DEFAULT_POLL_INTERVAL          100 
#define XR_MIN_POLL_INTERVAL              30 
#define XR_MAX_POLL_INTERVAL              1000 
#define XR_DEFAULT_POLL_COUNT             32 
#define XR_MIN_POLL_COUNT                 16 
#define XR_MAX_POLL_COUNT                 64 
#define XR_POLL_UPDATE_PERIOD             10 /* number of xr beacons */  
#define XR_GRPPOLL_PERIOD_FACTOR          5 /* factor used in calculating grp poll interval */
#define XR_4MS_FRAG_THRESHOLD            128 /* fragmentation threshold for 4msec frame limit  */

#define US_PER_4MS                       4000 /*  4msec define  */

/*
 * Maximum Values in ms for group poll periodicty
 */
#define GRP_POLL_PERIOD_NO_XR_STA_MAX       100
#define GRP_POLL_PERIOD_XR_STA_MAX          30

 /*
 * Percentage of the configured poll periodicity
 */
#define GRP_POLL_PERIOD_FACTOR_XR_STA     30  /* When XR Stations associated freq is 30% higher */

#ifndef A_MAX
#define A_MAX(a,b) ((a) > (b) ? (a) : (b))
#endif

/*
 * Macros to obtain the Group Poll Periodicty in various situations
 *
 * Curerntly there are the two cases
 * (a) When there are no XR STAs associated
 * (b) When there is atleast one XR STA associated
 */
#define GRP_POLL_PERIOD_NO_XR_STA(sc) (sc->sc_xrpollint)
#define GRP_POLL_PERIOD_XR_STA(sc)                                                   \
        A_MAX(GRP_POLL_PERIOD_FACTOR_XR_STA * (sc->sc_xrpollint / 100),GRP_POLL_PERIOD_XR_STA_MAX)

/*
 * When there are no XR STAs and a valid double chirp is received then the Group Polls are
 * transmitted for 10 seconds from the time of the last valid double double-chirp
 */
#define NO_XR_STA_GRPPOLL_TX_DUR    10000


/*
 * The key cache is used for h/w cipher state and also for
 * tracking station state such as the current tx antenna.
 * We also setup a mapping table between key cache slot indices
 * and station state to short-circuit node lookups on rx.
 * Different parts have different size key caches.  We handle
 * up to ATH_KEYMAX entries (could dynamically allocate state).
 */
#define	ATH_KEYMAX	128		/* max key cache size we handle */
#define	ATH_KEYBYTES	(ATH_KEYMAX/NBBY)	/* storage space in bytes */
#define	ATH_MIN_FF_RATE	12000		/* min rate fof ff aggragattion.in Kbps  */

#define ATH_CHAINMASK_SEL_TIMEOUT	6000

/* Default - Number of last RSSI values that is used for chainmask 
   selection */
#define ATH_CHAINMASK_SEL_RSSI_CNT	10

/* Means use 3x3 chainmask instead of configured chainmask */
#define ATH_CHAINMASK_SEL_3X3		7

/* Means use 2x3 chainmask instead of configured chainmask */
#define ATH_CHAINMASK_SEL_2X3		5

/* Default - Rssi threshold below which we have to switch to 3x3 */
#define ATH_CHAINMASK_SEL_UP_RSSI_THRES	 20

/* Default - Rssi threshold above which we have to switch to user 
   configured values */
#define ATH_CHAINMASK_SEL_DOWN_RSSI_THRES  35

/* Struct to store the chainmask select related info */
struct ath_chainmask_sel {
    struct ath_timer    timer;
    int                 cur_tx_mask; /* user configured or 3x3 */
    int                 cur_rx_mask; /* user configured or 3x3 */
    int                 tx_avgrssi;
    int                 switch_allowed:1, /* timer will set this */
                        cm_sel_enabled:1;
};

#define WME_NUM_TID         16
#define WME_BA_BMP_SIZE     64
#define WME_MAX_BA          WME_BA_BMP_SIZE
#define ATH_TID_MAX_BUFS    (2 * WME_MAX_BA)

#define ATH_RX_TIMEOUT      40  /* 40 milliseconds */

struct ath_atx_ac;

/*
 * per TID aggregate tx state for a destination
 */
typedef struct ath_atx_tid {
    int                     tidno;      /* TID number */
    u_int16_t               seq_start;  /* starting seq of BA window */
    u_int16_t               seq_next;   /* next seq to be used */
    u_int16_t               baw_size;   /* BA window size */
    int                     baw_head;   /* first un-acked tx buffer */
    int                     baw_tail;   /* next unused tx buffer slot */
    int                     sched;      /* TID is scheduled */
    int                     paused;     /* TID is paused */
    int                     cleanup_inprogress; /* aggr of this TID is being teared down */
    TAILQ_HEAD(ath_tid_bq,ath_buf)    buf_q;    /* pending buffers */
    TAILQ_ENTRY(ath_atx_tid) tid_qelem; /* round-robin tid entry */
    int                      filtered;  /* TID has filtered pkts */
    TAILQ_HEAD(,ath_buf)     fltr_q;    /* filtered buffers */
    TAILQ_ENTRY(ath_atx_tid) fltr_qelem;/* handle hwq filtering */
    struct ath_node         *an;        /* parent node structure */
    struct ath_atx_ac       *ac;        /* parent access category */
    struct ath_buf          *tx_buf[ATH_TID_MAX_BUFS];  /* active tx frames */

    /*
     * ADDBA state
     */
    u_int32_t               addba_exchangecomplete:1,
                            addba_amsdusupported:1;
    int32_t                 addba_exchangeinprogress;
    struct ath_timer        addba_requesttimer;
    int                     addba_exchangeattempts;
    u_int16_t               addba_exchangestatuscode;
} ath_atx_tid_t;

/*
 * per access-category aggregate tx state for a destination
 */
typedef struct ath_atx_ac {
    int                     sched;      /* dest-ac is scheduled */
    int                     qnum;       /* H/W queue number associated with this AC */
    int                     hwqcnt;     /* count of pkts on hw queue */
    TAILQ_ENTRY(ath_atx_ac) ac_qelem;   /* round-robin txq entry */
    TAILQ_HEAD(,ath_atx_tid)tid_q;      /* queue of TIDs with buffers */
    int                     filtered;   /* ac is filtered */
    TAILQ_ENTRY(ath_atx_ac) fltr_qelem; /* handle hwq filtering */
    TAILQ_HEAD(,ath_atx_tid)fltr_q;     /* queue of TIDs being filtered */
} ath_atx_ac_t;

/*
 * per dest tx state
 */
struct ath_atx {
    int                 hwqcnt;         /* count of pkts on hw queue */
    u_int16_t           maxampdu;       /* per-destination max ampdu */
    u_int16_t           mpdudensity;    /* per-destination ampdu density */
    u_int8_t            weptkipdelim;   /* per-destination delimiter count for WEP/TKIP w/aggregation */
    struct ath_atx_tid  tid[WME_NUM_TID];
    struct ath_atx_ac   ac[WME_NUM_AC];
};

#define ATH_AN_2_AC(_an, _priority) &(_an)->an_aggr.tx.ac[(_priority)]
#define ATH_AN_2_TID(_an, _tidno)   (&(_an)->an_aggr.tx.tid[(_tidno)])

struct ath_rxbuf {
    wbuf_t                  rx_wbuf;   /* buffer */
    systime_t               rx_time;    /* system time when received */
    ieee80211_rx_status_t   rx_status;  /* cached rx status */
};

/*
 * Per-TID aggregate receiver state for a node
 */
struct ath_arx_tid {
    struct ath_node     *an;        /* parent ath node */
    u_int16_t           seq_next;   /* next expected sequence */
    u_int16_t           baw_size;   /* block-ack window size */
    int                 baw_head;   /* seq_next at head */
    int                 baw_tail;   /* tail of block-ack window */
    int                 seq_reset;  /* need to reset start sequence */
    struct ath_timer    timer;      /* timer element */
    struct ath_rxbuf    *rxbuf; 	/* re-ordering buffer */
    spinlock_t          tidlock;    /* lock to protect this TID structure */

    /*
     * ADDBA response information
     */
    u_int16_t                       dialogtoken;
    u_int16_t                       statuscode;
    struct ieee80211_ba_parameterset baparamset;
    u_int16_t                       batimeout;
    u_int16_t                       userstatuscode;
    int             	    	    addba_exchangecomplete;
};

#define	ATH_RXTID_LOCK_INIT(_rxtid)     spin_lock_init(&(_rxtid)->tidlock)
#define	ATH_RXTID_LOCK_DESTROY(_rxtid)
#define	ATH_RXTID_LOCK(_rxtid)          spin_lock(&(_rxtid)->tidlock)
#define	ATH_RXTID_UNLOCK(_rxtid)        spin_unlock(&(_rxtid)->tidlock)

/*
 * Per-node receiver aggregate state
 */
struct ath_arx {
    struct ath_arx_tid  tid[WME_NUM_TID];
};

/*
 * Per-node aggregation state
 */
struct ath_node_aggr {
    struct ath_atx      tx;         /* node transmit state */
    struct ath_arx      rx;         /* node receive state */
};
#define an_tx_tid       an_aggr.tx.tid
#define an_tx_ac        an_aggr.tx.ac
#define an_rx_tid       an_aggr.rx.tid

struct ath_softc;

#ifdef ATH_SWRETRY
struct ath_swretry_info {
  u_int32_t         swr_num_pendfrms;        /* num of frames which are pending in queue */
  HAL_BOOL          swr_need_cleardest;      /* need to set the clearDestMask on next frame */
};
#endif

/* driver-specific node state */
struct ath_node {
    /* Pointer to the corresponding node object in protocol stack.
     * It should be opaque to ATH layer. */
    ieee80211_node_t    an_node;
    struct ath_softc    *an_sc; /* back pointer */

    u_int16_t	an_decomp_index; /* decompression mask index */
    u_int8_t	an_prevdatarix;	/* rate ix of last data frame */
    u_int16_t	an_minffrate;	/* mimum rate in kbps for ff to aggragate */
    struct ath_buf	*an_tx_ffbuf[WME_NUM_AC]; /* ff staging area */
#ifdef ATH_SUPPORT_UAPSD
    ath_bufhead	            an_uapsd_q;         /* U-APSD delivery queue */
    int                     an_uapsd_qdepth;    /* U-APSD delivery queue depth */
#endif
#ifdef ATH_CHAINMASK_SELECT
    struct ath_chainmask_sel an_chainmask_sel;
#endif

    struct ath_node_aggr    an_aggr;        /* A-MPDU aggregation state */
    struct atheros_node     *an_rc_node;    /* opaque ptr to rc-private struct */
    u_int8_t                an_smmode;  /* SM Power save mode */
#define ATH_NODE_CLEAN 0x1  /* indicates the node is clened up */
#define ATH_NODE_PWRSAVE 0x2  /* indicates the node is 80211 power save */
#define ATH_NODE_UAPSD 0x4  /* indicates the node is uapsd */
    u_int8_t                an_flags;
    u_int16_t               an_cap;     /* ath_node capabilities */
#ifdef ATH_SWRETRY
    int32_t                 an_swrenabled;       /* Whether retry is enabled for this node */
    ath_bufhead             an_softxmit_q;       /* Soft Tx Queue used for Sw Retries */
    int                     an_softxmit_qdepth;  /* Depth of soft tx queue */
    spinlock_t              an_softxmit_lock;    /* Soft_Xmit delivery queue lock */    
    struct ath_swretry_info an_swretry_info[HAL_NUM_TX_QUEUES];  /* SW retry info state machine */
    u_int32_t               an_total_swrtx_pendfrms;    /* Num of frms which are on SW retry */
    u_int32_t               an_total_swrtx_successfrms; /* Total num of suceeded sw retry frms */
    u_int32_t               an_total_swrtx_flushfrms;   /* Total num of flushed frms */
#endif
#if defined(ATH_SWRETRY) || defined(AP_SLOW_ANT_DIV)
    LIST_ENTRY(ath_node)    an_le;  /* link entry in node list */
#endif
#ifdef ATH_SUPPORT_IQUE
	u_int					an_minRate[WME_NUM_AC];	/* Min rate index for each AC */
#endif	
#ifdef AP_SLOW_ANT_DIV
    u_int32_t               an_antdiv_rssictl[ATH_ANT_DIV_MAX_CFG];
    u_int32_t               an_antdiv_bytes[ATH_ANT_DIV_MAX_CFG];
    u_int32_t               an_antdiv_rxcnt[ATH_ANT_DIV_MAX_CFG];
#endif
};

#define ATH_NODE(_n)                    ((struct ath_node *)(_n))

#ifdef ATH_SWRETRY
#define	ATH_NODE_SWRETRY_TXBUF_LOCK_INIT(_an)    spin_lock_init(&(_an)->an_softxmit_lock)
#define	ATH_NODE_SWRETRY_TXBUF_LOCK_DESTROY(_an)
#define	ATH_NODE_SWRETRY_TXBUF_LOCK(_an)         spin_lock(&(_an)->an_softxmit_lock)
#define	ATH_NODE_SWRETRY_TXBUF_UNLOCK(_an)       spin_unlock(&(_an)->an_softxmit_lock)
#endif

#define	ATH_ANTENNA_DIFF	2	/* Num farmes difference in
					   tx to flip default recv
					   antenna */

#ifdef ATH_TX99_DIAG
struct ath_txrx99 {
	u_int32_t		tx99mode;	      /* tx99 mode  */
	u_int32_t		prefetch;	      /* prefetch   */
	u_int32_t		txpower;	      /* tx power   */
	u_int32_t		txrate;	          /* tx rate    */
	u_int32_t		rx99mode;	      /* rx99 mode  */
	void 			*prev_hard_start;
	void 			*prev_mgt_start;
	u_int32_t 		imask;
};
#endif

struct ath_hal;
struct ath_desc;
struct atheros_softc;
struct proc_dir_entry;

/*
 * Data transmit queue state.  One of these exists for each
 * hardware transmit queue.  Packets sent to us from above
 * are assigned to queues based on their priority.  Not all
 * devices support a complete set of hardware transmit queues.
 * For those devices the array sc_ac2q will map multiple
 * priorities to fewer hardware queues (typically all to one
 * hardware queue).
 */
struct ath_txq {
    u_int			axq_qnum;	/* hardware q number */
    u_int32_t		*axq_link;	/* link ptr in last TX desc */
    TAILQ_HEAD(, ath_buf)	axq_q;		/* transmit queue */
    spinlock_t		axq_lock;	/* lock on q and link */
    unsigned long	axq_lockflags;	/* intr state when must cli */
    u_int			axq_depth;		/* queue depth */
    u_int			axq_ll_count;	/* queue depth */
    u_int8_t        axq_aggr_depth; /* aggregates queued */
    u_int8_t        axq_lim;
    u_int16_t       axq_aggr_nbuf;	/* ath_bufs consumed presently */
    u_int32_t		axq_totalqueued;/* total ever queued */
    u_int			axq_intrcnt;	/* count to determine if descriptor
						 * should generate int on this txq.
						 */
    ath_bufhead		axq_ll;
    spinlock_t		axq_ll_lock;
    unsigned long	axq_ll_lockflags;
	/*
	 * State for patching up CTS when bursting.
	 */
	struct	ath_buf		*axq_linkbuf;	/* virtual addr of last buffer*/
	struct	ath_desc	*axq_lastdsWithCTS;	/* first desc of the last descriptor 
							 * that contains CTS 
							 */
	struct	ath_desc	*axq_gatingds;	/* final desc of the gating desc 
						 * that determines whether lastdsWithCTS has 
						 * been DMA'ed or not
						 */
    /*
     * Staging queue for frames awaiting a fast-frame pairing.
     */
    TAILQ_HEAD(axq_headtype, ath_buf) axq_stageq;
    TAILQ_HEAD(,ath_atx_ac) axq_acq;

#ifdef ATH_SUPERG_COMP
    /* scratch compression buffer */
    char                    *axq_compbuf;   /* scratch comp buffer */
    dma_addr_t              axq_compbufp;  /* scratch comp buffer (phys)*/
    u_int                   axq_compbufsz; /* scratch comp buffer size */
    OS_DMA_MEM_CONTEXT(axq_dmacontext)
#endif
#ifdef ATH_SWRETRY        
    u_int                   axq_destmask:1; /*Destination mask for this queue*/    
#endif    
};

/* driver-specific vap state */
struct ath_vap {
    ieee80211_if_t                  av_if_data; /* interface(vap) instance from 802.11 protocal layer */
    HAL_OPMODE                      av_opmode;  /* VAP operational mode */
    struct ath_buf                  *av_bcbuf;	/* beacon buffer */
    ieee80211_beacon_offset_t       av_boff;    /* dynamic update state */
    ieee80211_tx_control_t          av_btxctl;   /* tx control information for beacon */
    int                             av_bslot;	/* beacon slot index */
    struct ath_txq                  av_mcastq;	/* multicast transmit queue */
    u_int8_t                        av_dfswait_run;
    struct atheros_vap              *av_atvp;   /* opaque ptr to rc-private struct */
    struct ath_vap_config           av_config;  /* vap configuration parameters from 802.11 protocol layer*/
};

/*
 * Define the scheme that we select MAC address for multiple BSS on the same radio.
 * The very first VAP will just use the MAC address from the EEPROM.
 * For the next 3 VAPs, we set the U/L bit (bit 1) in MAC address,
 * and use the next bits as the index of the VAP.
 *
 * The logic used below is as follows:
 * - Default HW mac address maps to index 0
 * - Index 1 maps to default HW mac addr + 1
 * - Index 2 maps to default HW mac addr + 2 ...
 * The macros are used to generate new BSSID bits based on index and also 
 * BSSID bits are used to generate index during GET operation.
 * No of bits used depends on ATH_BCBUF value.
 * e.g for 8 vaps, 3 bits are used (bits 2,3,4 and mask of 7).
 *
 */
#define ATH_SET_VAP_BSSID_MASK(bssid_mask)      ((bssid_mask)[0] &= ~(((ATH_BCBUF-1) << 2) | 0x02))

#define ATH_GET_VAP_ID(bssid, hwbssid, id)                              \
    do {                                                                \
       u_int8_t hw_bssid = (hwbssid[0] >> 2) & (ATH_BCBUF - 1);         \
       u_int8_t tmp_bssid = (bssid[0] >> 2) & (ATH_BCBUF - 1);          \
                                                                        \
       id = ((tmp_bssid + ATH_BCBUF) - hw_bssid) & (ATH_BCBUF - 1);     \
    } while (0)
    
       
#define ATH_SET_VAP_BSSID(bssid, hwbssid, id)                        \
    do {                                                             \
        if (id) {                                                    \
            u_int8_t hw_bssid = (hwbssid[0] >> 2) & (ATH_BCBUF - 1); \
            u_int8_t tmp_bssid;                                      \
                                                                     \
            (bssid)[0] &= ~((ATH_BCBUF - 1) << 2);                   \
            tmp_bssid = ((id + hw_bssid) & (ATH_BCBUF - 1));         \
            (bssid)[0] |= (((tmp_bssid) << 2) | 0x02);               \
        }                                                            \
    } while(0)

/* Hardware uses bit 0,1,2 of last byte(Most significant byte) of MAC address
 * to support mBssid+aggregation(Howl 1.4 Onwards). But bit 0 is used for Unicast/Broadcast.
 * Therefore, we use bits 1,2,... for next VAPs. Though bit 1 is to identify
 * Universal/Local address we are using it(ignoring it) for supporting more VAPs.
 */
 

#define ATH_SET_VAP_BSSID_MASK_ULB(bssid_mask)      ((bssid_mask)[0] &= ~((ATH_BCBUF-1)<<1))
#define ATH_GET_VAP_ID_ULB(bssid)                   ((bssid)[0] >> 1)
#define ATH_SET_VAP_BSSID_ULB(bssid, id)            \
    do {                                        \
        if (id) {                               \
            (bssid)[0] |= ((id)<<1);            \
        }                                       \
    } while(0)

#define	ATH_BEACON_AIFS_DEFAULT		0  /* Default aifs for ap beacon q */
#define	ATH_BEACON_CWMIN_DEFAULT	0  /* Default cwmin for ap beacon q */
#define	ATH_BEACON_CWMAX_DEFAULT	0  /* Default cwmax for ap beacon q */

#define ATH_TXQ_INTR_PERIOD		5  /* axq_intrcnt period for intr gen */
#define	ATH_TXQ_LOCK_INIT(_tq)		spin_lock_init(&(_tq)->axq_lock)
#define	ATH_TXQ_LOCK_DESTROY(_tq)

#define	ATH_TXQ_LL_LOCK_INIT(_tq)	spin_lock_init(&(_tq)->axq_ll_lock)
#define	ATH_TXQ_LL_LOCK_DESTROY(_tq)

#define	ATH_TXQ_LOCK(_tq)           do { spin_lock_irqsave(&(_tq)->axq_lock, (_tq)->axq_lockflags); } while(0)
#define	ATH_TXQ_UNLOCK(_tq)         do { spin_unlock_irqrestore(&(_tq)->axq_lock, (_tq)->axq_lockflags); } while(0)

#define	ATH_TXQ_LL_LOCK(_tq)           do { spin_lock_irqsave(&(_tq)->axq_ll_lock, (_tq)->axq_ll_lockflags); } while(0)
#define	ATH_TXQ_LL_UNLOCK(_tq)         do { spin_unlock_irqrestore(&(_tq)->axq_ll_lock, (_tq)->axq_ll_lockflags); } while(0)

/* move buffers from MCASTQ to CABQ */
#define ATH_TXQ_MOVE_MCASTQ(_tqs,_tqd) do {                         \
    (_tqd)->axq_depth += (_tqs)->axq_depth;                         \
    (_tqd)->axq_totalqueued += (_tqs)->axq_totalqueued;             \
    (_tqd)->axq_linkbuf = (_tqs)->axq_linkbuf;                      \
    (_tqd)->axq_link = (_tqs)->axq_link;                            \
    TAILQ_CONCAT(&(_tqd)->axq_q, &(_tqs)->axq_q, bf_list);          \
    (_tqs)->axq_depth=0;                                            \
    (_tqs)->axq_totalqueued = 0;                                    \
    (_tqs)->axq_linkbuf = 0;                                        \
    (_tqs)->axq_link = NULL;                                        \
} while (0)

/* concat a list of buffers to txq */
#define ATH_TXQ_CONCAT(_tq, _stq) do {                              \
    TAILQ_CONCAT(&(_tq)->axq_q, (_stq), bf_list);                   \
    (_tq)->axq_depth ++;                                            \
    (_tq)->axq_totalqueued ++;                                      \
    (_tq)->axq_linkbuf = TAILQ_LAST(&(_tq)->axq_q, ath_bufhead_s);  \
} while (0)

/* move a list from a txq to a buffer list (including _elm) */
#define ATH_TXQ_MOVE_HEAD_UNTIL(_tq, _stq, _elm, _field) do {       \
    TAILQ_REMOVE_HEAD_UNTIL(&(_tq)->axq_q, _stq, _elm, _field);     \
    (_tq)->axq_depth --;                                            \
} while (0)

/* move a list from a txq to a buffer list (not including _elm) */
#define ATH_TXQ_MOVE_HEAD_BEFORE(_tq, _stq, _elm, _field) do {                  \
    TAILQ_REMOVE_HEAD_BEFORE(&(_tq)->axq_q, _stq, _elm, ath_bufhead_s, _field); \
    (_tq)->axq_depth --;                                                        \
} while (0)

#define ATH_TXQ_REMOVE_STALE_HEAD(_tq, _elm, _field)                \
    TAILQ_REMOVE(&(_tq)->axq_q, (_elm), _field)

/* 
 * concat buffers from one queue to other
 */
#define ATH_TXQ_MOVE_Q(_tqs,_tqd)  ATH_TXQ_MOVE_MCASTQ(_tqs,_tqd)

/*
 * Regardless of the number of beacons we stagger, (i.e. regardless of the 
 * number of BSSIDs) if a given beacon does not go out even after waiting this 
 * number of beacon intervals, the game's up.
 */ 
#define BSTUCK_THRESH   9 * ATH_BCBUF

/* unalligned little endian access */
#define LE_READ_2(p)							\
	((u_int16_t)							\
	 ((((u_int8_t *)(p))[0]      ) | (((u_int8_t *)(p))[1] <<  8)))
#define LE_READ_4(p)							\
	((u_int32_t)							\
	 ((((u_int8_t *)(p))[0]      ) | (((u_int8_t *)(p))[1] <<  8) |	\
	  (((u_int8_t *)(p))[2] << 16) | (((u_int8_t *)(p))[3] << 24)))

/* increment with wrap-around */
#define INCR(_l, _sz)   (_l) ++; (_l) &= ((_sz) - 1)

/* add with wrap-around */
#define ADD(_l, _n, _sz)   (_l) += (_n); (_l) &= ((_sz) - 1)

/*
 * new aggegation related macros
 */
#define ATH_AGGR_DELIM_SZ       4       /* delimiter size   */
#define ATH_AGGR_MINPLEN        256     /* in bytes, minimum packet length */
#define ATH_AGGR_ENCRYPTDELIM   10      /* number of delimiters for encryption padding */

typedef enum {
    ATH_AGGR_DONE,
    ATH_AGGR_BAW_CLOSED,
    ATH_AGGR_LIMITED,
    ATH_AGGR_SHORTPKT,
    ATH_AGGR_8K_LIMITED,
} ATH_AGGR_STATUS;

/*
 * returns delimiter padding required given the packet length
 */
#define ATH_AGGR_GET_NDELIM(_len)                                   \
    (((((_len) + ATH_AGGR_DELIM_SZ) < ATH_AGGR_MINPLEN) ?           \
      (ATH_AGGR_MINPLEN - (_len) - ATH_AGGR_DELIM_SZ) : 0) >> 2)

/*
 * minimum h/w qdepth to be sustained to maximize aggregation
 */
#define ATH_AGGR_MIN_QDEPTH 2

#define BAW_WITHIN(_start, _bawsz, _seqno)      \
    ((((_seqno) - (_start)) & 4095) < (_bawsz))

#define PADBYTES(_len)   ((4 - ((_len) % 4)) % 4)
#define ATH_MAX_SW_RETRIES  10


/*
 * return block-ack bitmap index given sequence and starting sequence
 */
#define ATH_BA_INDEX(_st, _seq) (((_seq) - (_st)) & (IEEE80211_SEQ_MAX - 1))

/*
 * return whether a bit at index _n in bitmap _bm is set
 * _sz is the size of the bitmap
 */
#define ATH_BA_ISSET(_bm, _n)        (((_n) < (WME_BA_BMP_SIZE)) &&          \
                                     ((_bm)[(_n) >> 5] & (1 << ((_n) & 31))))

/*
 * desc accessor macros
 */
#define ATH_DS_BA_SEQ(_ds)          (_ds)->ds_us.tx.ts_seqnum
#define ATH_DS_BA_BITMAP(_ds)       (&(_ds)->ds_us.tx.ba_low)
#define ATH_DS_TX_BA(_ds)           ((_ds)->ds_us.tx.ts_flags & HAL_TX_BA)
#define ATH_DS_TX_STATUS(_ds)       (_ds)->ds_us.tx.ts_status
#define ATH_DS_TX_FLAGS(_ds)        (_ds)->ds_us.tx.ts_flags

#define ATH_RIFS_NONE 0
#define ATH_RIFS_SUBFRAME_FIRST 1
#define ATH_RIFS_SUBFRAME_LAST 2
#define ATH_RIFS_BAR 3

#define ATH_SET_TX_SET_NOACK_POLICY(_sc, _wh) do {              \
    if (_wh) {                                                  \
        if (((_wh)->i_fc[1] & IEEE80211_FC1_DIR_MASK) !=        \
             IEEE80211_FC1_DIR_DSTODS)                          \
        ((struct ieee80211_qosframe *)_wh)->i_qos[0]            \
                      |= (1 << IEEE80211_QOS_ACKPOLICY_S)       \
                         & IEEE80211_QOS_ACKPOLICY;             \
        else                                                    \
        ((struct ieee80211_qosframe_addr4 *)_wh)->i_qos[0]      \
                      |= (1 << IEEE80211_QOS_ACKPOLICY_S)       \
                         & IEEE80211_QOS_ACKPOLICY;             \
    }                                                           \
} while (0)

#define ATH_SET_TX_CLR_NOACK_POLICY(_sc, _wh) do {              \
    if (_wh) {                                                  \
        if (((_wh)->i_fc[1] & IEEE80211_FC1_DIR_MASK) !=        \
             IEEE80211_FC1_DIR_DSTODS)                          \
        ((struct ieee80211_qosframe *)(_wh))->i_qos[0]          \
                      &= ~IEEE80211_QOS_ACKPOLICY;              \
    else                                                        \
        ((struct ieee80211_qosframe_addr4 *)(_wh))->i_qos[0]    \
                      &= ~IEEE80211_QOS_ACKPOLICY;              \
    }                                                           \
} while (0)

#define ATH_MPDU_2_QOS_WH(_mpdu) \
        (struct ieee80211_qosframe *)wbuf_header(_mpdu)

struct aggr_rifs_param {
    int param_max_frames;
    int param_max_len;
    int param_rl;
    int param_al;
    struct ath_rc_series *param_rcs;
};

/*
 * Per-instance load-time (note: NOT run-time) configurations for Atheros Device
 */
struct ath_config {
    u_int8_t    txchainmask;
    u_int8_t    rxchainmask;
    u_int8_t    txchainmasklegacy;
    u_int8_t    rxchainmasklegacy;
    u_int8_t    chainmask_sel; /* enable automatic tx chainmask selection */
    int         ampdu_limit;
    int         ampdu_subframes;
    int         rifs_ampdu_div;
	u_int32_t 	ath_aggr_prot;
	u_int32_t 	ath_aggr_prot_duration;
	u_int32_t 	ath_aggr_prot_max;
    int         ath_countrycode;	/* country code */
    int         ath_xchanmode;		/* enable extended channels */
    u_int16_t   txpowlimit;
    u_int16_t   txpowlimit_override;
    u_int16_t   tpscale;
    u_int8_t    pcieDisableAspmOnRfWake;         /* Only use ASPM when RF Silenced */
    u_int8_t    pcieAspm;                        /* ASPM bit settings */
    u_int8_t    cabqReadytime;                   /* Cabq Readytime % */
    u_int8_t    swBeaconProcess;                 /* Process received beacons in SW (vs HW) */
};

#ifdef ATH_WOW

/* Structure and defines for WoW */
/* TBD: Should dynamically allocate memeory after query hal for hardware capability */
#define MAX_PATTERN_SIZE                  256
#define MAX_PATTERN_MASK_SIZE             32
#define MAX_NUM_PATTERN                   8
#define MAX_NUM_USER_PATTERN              6  /* Deducting the disassociate/deauthenticate packets */

#define ATH_WAKE_UP_PATTERN_MATCH          0x00000001
#define ATH_WAKE_UP_MAGIC_PACKET           0x00000002
#define ATH_WAKE_UP_LINK_CHANGE            0x00000004

/* PCI Power management (PM_CSR) D0-D3 state settings */
#define PCIE_PM_CSR                 0x44    // PM Control/Status Register

#define PCIE_PM_CSR_D0              0x00
#define PCIE_PM_CSR_D1              0x01
#define PCIE_PM_CSR_D2              0x02
#define PCIE_PM_CSR_D3              0x03
#define PCIE_PM_CSR_PME_ENABLE      0x0100
#define PCIE_PM_CSR_STATUS          0x8000


typedef struct wowPattern {
    u_int32_t valid;
    u_int8_t  patternBytes[MAX_PATTERN_SIZE];
    u_int8_t  maskBytes[MAX_PATTERN_SIZE];
	u_int32_t patternLen;
} WOW_PATTERN;

struct wow_info {
    u_int32_t wakeUpEvents; //Values passed in OID_PNP_ENABLE_WAKE_UP
    u_int32_t numOfPatterns;
    u_int32_t wowWakeupReason;
    HAL_INT   intrMaskBeforeSleep;
    WOW_PATTERN patterns[MAX_NUM_PATTERN];
    u_int16_t wowDuration;
};

#endif

#ifdef ATH_SUPPORT_DFS
struct dfs_hang_war {
    os_timer_t          hang_war_timer;
    int                 hang_war_ht40count;
    int                 hang_war_ht20count;
    int                 hang_war_timeout;
    int                 hang_war_activated;
    int                 total_stuck;
    u_int64_t           last_dfs_hang_war_tstamp;
};
#endif

#ifdef ATH_MIB_INTR_FILTER
typedef enum {
    INTR_FILTER_OFF,           /* waiting for first MIB interrupt */
    INTR_FILTER_DEGLITCHING,   /* waiting for signals of burst */
    INTR_FILTER_ON             /* discarding MIB interrupts */
} INTR_FILTER_STATE;

#define MIB_FILTER_COUNT_THRESHOLD           500   /* number of interrupts to characterize a long burst ("storm") */
#define MIB_FILTER_MAX_INTR_ELAPSED_TIME      20   /* maximum allowed time interval for between 2 consecutive interrupts within the same burst, in ms */
#define MIB_FILTER_MAX_BURST_ELAPSED_TIME    100   /* maximum allowed elapsed time since the beginning of the burst, in ms */
#define MIB_FILTER_RECOVERY_TIME              50   /* minimum time, in ms, the MIB interrupts must be kept disabled after a burst has been detected */

typedef struct ath_intr_filter {
    INTR_FILTER_STATE    state;                  /* state machine's state */
    u_int32_t            activation_count;       /* number of storms detected so far */
    systime_t            burst_start_time;       /* timestamp of the first interrupt determined to be part of a burst */
    systime_t            last_intr_time;         /* timestamp of the last interrupt */
    u_int32_t            intr_count;             /* number of interrupts in the current storm */
};
#endif

struct ath_softc {
    ieee80211_handle_t      sc_ieee;
    struct ieee80211_ops    *sc_ieee_ops;

    osdev_t                 sc_osdev;   /* Opaque handle to OS-specific device */
    spinlock_t              sc_lock;    /* softc-level lock */

    struct ath_reg_parm     sc_reg_parm;/* per-instance attach-time parameters */
    struct ath_config       sc_config;  /* per-instance load-time parameters */
    struct ath_stats        sc_stats;   /* private statistics */
#ifdef ATH_SUPPORT_DFS
    struct ath_dfs          *sc_dfs;
    struct dfs_hang_war     sc_dfs_hang;
#endif
    /* private PHY statisitics for each PHY mode */
    struct ath_phy_stats    sc_phy_stats[WIRELESS_MODE_MAX];

    int                     sc_wifi;
    int                     sc_debug;
    SetDefAntenna_callback  sc_setdefantenna;
    void                    *sc_bdev;   /* associated bus device */

    struct ath_hal          *sc_ah;     /* Atheros HAL */
    struct atheros_softc    *sc_rc;     /* tx rate control support */

    u_int32_t               sc_intrstatus; /* XXX HAL_STATUS */

    /* Properties */
    unsigned int
        sc_invalid             : 1, /* being detached */
        sc_mrretry             : 1, /* multi-rate retry support */
        sc_needmib             : 1, /* enable MIB stats intr */
        sc_hasdiversity        : 1, /* rx diversity available */
        sc_diversity           : 1, /* enable rx diversity */
        sc_olddiversity        : 1, /* diversity setting before XR enable */
        sc_haswme              : 1, /* wme priority queueing support */
        sc_hascompression      : 1, /* compression support */
        sc_hasveol             : 1, /* tx VEOL support */
        sc_hastpc              : 1, /* per-packet TPC support */
        sc_dturbo              : 1, /* dynamic turbo capable */
        sc_dturbo_switch       : 1, /* turbo switch mode*/
        sc_rate_recn_state     : 1, /* dynamic turbo state recmded by ratectrl */
        sc_ignore_ar           : 1, /* ignore AR during transision*/
        sc_beacons             : 1, /* beacons running */
        sc_hasbmask            : 1, /* bssid mask support */
        sc_hastsfadd           : 1, /* tsf adjust support */
        sc_scanning            : 1, /* scanning active */
        sc_nostabeacons        : 1, /* no beacons for station */
        sc_xrgrppoll           : 1, /* xr group polls are active */
        sc_hasclrkey           : 1, /* CLR key supported */
        sc_devstopped          : 1, /* stopped 'cuz no tx bufs */
        sc_stagbeacons         : 1, /* use staggered beacons */
        sc_rtasksched          : 1, /* radar task is scheduled */
        sc_hasrfkill           : 1, /* RfKill is enabled */
        sc_hw_phystate         : 1, /* hardware (GPIO) PHY state */
        sc_sw_phystate         : 1, /* software PHY state */
        sc_txaggr              : 1, /* enable 11n tx aggregation */
        sc_rxaggr              : 1, /* enable 11n rx aggregation */
        sc_hasautosleep        : 1, /* automatic sleep after TIM */
        sc_waitbeacon          : 1, /* waiting for first beacon after waking up */
        sc_no_tx_3_chains      : 1; /* user, hardware, regulatory or country
                                     * may disallow transmit on three chains. */

    unsigned int
        sc_update_chainmask    : 1, /* change chain mask */
        sc_rx_chainmask_detect : 1, /* enable rx chain mask detection */
        sc_rx_chainmask_start  : 1, /* start rx chain mask detection */
        sc_txrifs              : 1, /* enable 11n tx rifs */
        sc_hashtsupport        : 1, /* supports 11n */
        sc_txstbcsupport       : 1,
        sc_rxstbcsupport       : 2,
        sc_log_rcfind          : 1, /* log rcfind */
        sc_txamsdu             : 1, /* enable 11n tx amsdu */
        sc_swRetryEnabled      : 1, /* support for sw retrying mechanism */
        sc_full_reset          : 1, /* force full reset */
        sc_uapsdsupported      : 1, /* supports UAPSD */
#ifdef ATH_WOW    
        sc_hasWow              : 1, /* Is WoW feature present in HW */      
        sc_wow_sleep           : 1, /* In the middle of Wow Sleep? */
        sc_wow_bmiss_intr      : 1, /* Beacon Miss workaround during WOW sleep. */
#endif
        sc_slowAntDiv          : 1, /* enable slow antenna diversity */
        sc_removed             : 1, /* card has been physically removed */
        sc_eap_lowest_rate     : 1, /* Use lowest data rate for EAP packets */
        sc_wpsgpiointr         : 1, /* WPS Push button GPIO interrupt enabled */
        sc_vap_ind             : 1, /* independent repeater */
        sc_antDivComb          : 1, /* enable antenna diversity and combining feature for KITE */
        sc_wpsbuttonpushed     : 1; /* WPS Push Button status */

#ifdef ATH_RB
    ath_rb_mode_t           sc_rxrifs;  /* enable/disable rb detection */
    u_int16_t               sc_rxrifs_timeout; /* rb timeout value*/
    u_int8_t                sc_rxrifs_skipthresh; /* rb skip thresh value*/
#endif
    HAL_OPMODE              sc_opmode;  /* current operating mode */
    u_int8_t                sc_coverageclass;

    /* rate tables */
    const HAL_RATE_TABLE    *sc_rates[WIRELESS_MODE_MAX];
    const HAL_RATE_TABLE    *sc_currates;   /* current rate table */
    const HAL_RATE_TABLE    *sc_xr_rates;   /* XR rate table */
    const HAL_RATE_TABLE    *sc_half_rates; /* half rate table */
    const HAL_RATE_TABLE    *sc_quarter_rates; /* quarter rate table */
    WIRELESS_MODE           sc_curmode;     /* current phy mode */
    u_int16_t               sc_curtxpow;    /* current tx power limit */
    u_int16_t               sc_curaid;      /* current association id */
    HAL_CHANNEL             sc_curchan;     /* current h/w channel */
    HAL_CHANNEL             sc_extchan;     /* extension h/w channel */
    u_int8_t                sc_curbssid[IEEE80211_ADDR_LEN];
    u_int8_t                sc_myaddr[IEEE80211_ADDR_LEN];
    u_int8_t                sc_rixmap[256]; /* IEEE to h/w rate table ix */
    struct {
        u_int32_t           rateKbps;       /* transfer rate in kbs */
        u_int8_t            ieeerate;       /* IEEE rate */
        u_int8_t            flags;          /* radiotap flags */
        u_int16_t           ledon;          /* softled on time */
        u_int16_t           ledoff;         /* softled off time */
    } sc_hwmap[256];         /* h/w rate ix mappings */
#ifdef ATH_SUPPORT_IQUE
    struct {
        u_int8_t            per_threshold;
        u_int8_t            probe_interval;
    } sc_rc_params[2];
    struct {
        u_int8_t            use_rts;
        u_int8_t            aggrsize_scaling;
        u_int32_t           min_kbps;
    } sc_ac_params[WME_NUM_AC];
	struct {
		u_int8_t			hbr_enable;
		u_int8_t			hbr_per_low;
	} sc_hbr_params[WME_NUM_AC];	
#endif
    u_int8_t                sc_minrateix;   /* min h/w rate index */
    u_int8_t                sc_protrix;     /* protection rate index */
    PROT_MODE               sc_protmode;    /* protection mode */
    u_int8_t                sc_mcastantenna;/* Multicast antenna number */
    u_int8_t                sc_txantenna;   /* data tx antenna (fixed or auto) */
    u_int8_t                sc_cur_txant;   /* current tx antenna */
    u_int8_t                sc_nbcnvaps;    /* # of vaps sending beacons */
    u_int16_t               sc_nvaps;       /* # of active virtual ap's */
    struct ath_vap          *sc_vaps[ATH_BCBUF]; /* interface id to avp map */

    u_int                   sc_fftxqmin;    /* aggregation threshold */
    HAL_INT                 sc_imask;       /* interrupt mask copy */
    u_int                   sc_keymax;      /* size of key cache */
    u_int8_t                sc_keymap[ATH_KEYBYTES];/* key use bit map */
    struct ieee80211_node   *sc_keyixmap[ATH_KEYMAX];/* key ix->node map */
    u_int8_t                sc_bssidmask[IEEE80211_ADDR_LEN];

    u_int8_t                sc_rxrate;      /* current rx rate for LED */
    u_int8_t                sc_txrate;      /* current tx rate for LED */
    struct ath_led_control  sc_led_control; /* LED control structure */
    struct ath_force_ppm    sc_ppm_info;    /* PPM info */

    int                     sc_rxbufsize;   /* rx size based on mtu */
    struct ath_descdma      sc_rxdma;       /* RX descriptors */
    ath_bufhead             sc_rxbuf;       /* receive buffer */
    u_int32_t               *sc_rxlink;     /* link ptr in last RX desc */
    spinlock_t              sc_rxflushlock; /* lock of RX flush */
    int32_t                 sc_rxflush;     /* rx flush in progress */
    spinlock_t              sc_rxbuflock;
    unsigned long           sc_rxbuflockflags;
    u_int8_t                sc_defant;      /* current default antenna */
    u_int8_t                sc_rxotherant;  /* rx's on non-default antenna*/
    u_int16_t               sc_cachelsz;    /* cache line size */
    u_int64_t               sc_lastrx;      /* tsf of last rx'd frame */

    struct ath_descdma      sc_txdma;       /* TX descriptors */
    ath_bufhead             sc_txbuf;       /* transmit buffer */
    spinlock_t              sc_txbuflock;   /* txbuf lock */
    u_int                   sc_txqsetup;    /* h/w queues setup */
    u_int                   sc_txintrperiod;/* tx interrupt batching */
    struct ath_txq          sc_txq[HAL_NUM_TX_QUEUES];
    u_int16_t               sc_txqlim[HAL_NUM_TX_QUEUES];
    int                     sc_haltype2q[HAL_WME_AC_VO+1]; /* HAL WME AC -> h/w qnum */
    HAL_TXQ_INFO            sc_beacon_qi;   /* adhoc only: beacon queue parameters */
#ifdef ATH_SUPPORT_UAPSD
    struct ath_descdma      sc_uapsdqnuldma;    /* U-APSD QoS NULL TX descriptors */
    ath_bufhead	            sc_uapsdqnulbf;	    /* U-APSD QoS NULL free buffers */
    spinlock_t              sc_uapsdirqlock;    /* U-APSD IRQ lock */
    unsigned long           sc_uapsdirqlockflags;
    int                     sc_uapsdqnuldepth;  /* U-APSD delivery queue depth */
#endif

    u_int8_t                sc_grppoll_str[GRPPOLL_RATE_STR_LEN];  
    struct ath_descdma      sc_bdma;        /* beacon descriptors */
    ath_bufhead             sc_bbuf;        /* beacon buffers */
    u_int                   sc_bhalq;       /* HAL q for outgoing beacons */
    u_int                   sc_bmisscount;  /* missed beacon transmits */
    u_int                   sc_noise;       /* noise detected on channel */
    u_int                   sc_toggle_immunity; /* toggle immunity parameters */
    u_int32_t               sc_ant_tx[8];   /* recent tx frames/antenna */
    struct ath_txq          *sc_cabq;       /* tx q for cab frames */
    struct ath_txq          sc_grpplq;      /* tx q for XR group polls */
    struct ath_txq          *sc_xrtxq;      /* tx q for XR data */
    struct ath_descdma      sc_grppolldma;  /* TX descriptors for grppoll */
    ath_bufhead             sc_grppollbuf;  /* transmit buffers for grouppoll  */
    u_int16_t               sc_xrpollint;   /* xr poll interval */
    u_int16_t               sc_xrpollcount; /* xr poll count */
    struct ath_txq          *sc_uapsdq;     /* tx q for uapsd */

    enum {
        OK,                 /* no change needed */
        UPDATE,             /* update pending */
        COMMIT              /* beacon sent, commit change */
    } sc_updateslot;            /* slot time update fsm */
    int                     sc_slotupdate;  /* slot to next advance fsm */
    int                     sc_slottime;    /* slot time */
    int                     sc_bslot[ATH_BCBUF];/* beacon xmit slots */
    int                     sc_bnext;       /* next slot for beacon xmit */

    os_timer_t              sc_tx_leg;      /* legacy tx timer */
    os_timer_t              sc_cal_ch;      /* calibration timer */
    HAL_NODE_STATS          sc_halstats;    /* station-mode rssi stats */

    u_int16_t               sc_reapcount;   /* # of tx buffers reaped after net dev stopped */
    int16_t                 sc_noise_floor; /* signal noise floor in dBm */
    u_int8_t                sc_noreset;
    u_int8_t                sc_limit_legacy_frames;
#ifdef AH_WAR_52640
    u_int16_t               sc_qcumask;
    os_timer_t              sc_inact;       /* inactivity timer */
    u_int8_t                sc_tx_inact;
#endif
#ifdef AP_SLOW_ANT_DIV
    os_timer_t              sc_slow_ant_div;
#endif
    u_int8_t                sc_bsthresh;    /* Beacon Stuck Threshold, based on sc_nvaps */

#ifdef ATH_SUPERG_DYNTURBO
    os_timer_t              sc_dturbo_switch_mode;  /* AP scan timer */
    u_int32_t               sc_dturbo_tcount;       /* beacon intval count */
    u_int32_t               sc_dturbo_hold_max;     /* hold count before switching to base*/
    u_int16_t               sc_dturbo_hold_count;   /* hold count before switching to base*/
    u_int16_t               sc_dturbo_turbo_tmin;   /* min turbo count */
    u_int32_t               sc_dturbo_bytes;        /* bandwidth stats */ 
    u_int32_t               sc_dturbo_base_tmin;    /* min time in base */
    u_int32_t               sc_dturbo_turbo_tmax;   /* max time in turbo */
    u_int32_t               sc_dturbo_bw_base;      /* bandwidth threshold */
    u_int32_t               sc_dturbo_bw_turbo;     /* bandwidth threshold */
#endif

    HAL_HT_EXTPROTSPACING   sc_ht_extprotspacing;

    u_int8_t                sc_tx_chainmask;
    u_int8_t                sc_rx_chainmask;
    u_int8_t                sc_rxchaindetect_ref;
    u_int8_t                sc_rxchaindetect_thresh5GHz;
    u_int8_t                sc_rxchaindetect_thresh2GHz;
    u_int8_t                sc_rxchaindetect_delta5GHz;
    u_int8_t                sc_rxchaindetect_delta2GHz;
    u_int8_t                sc_cfg_txchainmask;     /* initial chainmask config */
    u_int8_t                sc_cfg_rxchainmask;
    u_int8_t                sc_cfg_txchainmask_leg;
    u_int8_t                sc_cfg_rxchainmask_leg;
    
    u_int32_t               sc_rtsaggrlimit; /* Chipset specific aggr limit */
    ath_pwrsave_t           sc_pwrsave;
    u_int32_t               sc_pktlog_enable;
    systime_t               sc_shortcal_timer;
    systime_t               sc_longcal_timer;
    systime_t               sc_resetcal_timer;
    systime_t               sc_ani_timer;
    HAL_BOOL                sc_Caldone;
    spinlock_t              sc_resetlock;
#ifdef ATH_WOW
    u_int8_t                sc_wowenable;   /* Is wake up on wireless enabled*/
    struct wow_info         *sc_wowInfo;
#endif

#ifdef ATH_RFKILL
    struct ath_rfkill_info  sc_rfkill;      /* RfKill settings */
#endif

#ifdef ATH_BT_COEX
    struct ath_bt_info      sc_btinfo;      /* BT coexistence settings */
#endif
#ifndef REMOVE_PKT_LOG
    struct ath_pktlog_info  *pl_info;
#endif
    hal_hw_hangs_t          sc_hang_war;
    HAL_BOOL                sc_hang_check;
#ifdef ATH_RB
    ath_rb_t                sc_rb;
    u_int8_t                sc_do_rb_war;
#endif

#if defined(ATH_SWRETRY) || defined(AP_SLOW_ANT_DIV)
    u_int32_t                   sc_num_swretries;
    ATH_LIST_HEAD(, ath_node)   sc_nt;          /* node table list */
    spinlock_t                  sc_nt_lock;     /* node table lock */
#endif

#if defined(SLOW_ANT_DIV) || defined(AP_SLOW_ANT_DIV)
    /* Slow antenna diversity */
    struct ath_antdiv       sc_antdiv;
#endif
#ifdef ATH_SUPPORT_IQUE
	u_int					sc_retry_duration;
	u_int8_t				sc_hbr_per_high;
	u_int8_t				sc_hbr_per_low;	
#endif

#ifdef ATH_MIB_INTR_FILTER
    struct ath_intr_filter  sc_intr_filter;
#endif
    struct ath_antcomb       sc_antcomb;
};

#define ATH_DEV_TO_SC(_dev)         ((struct ath_softc *)(_dev))

typedef void (*ath_callback) (struct ath_softc *);

#define	ATH_TXQ_SETUP(sc, i)        ((sc)->sc_txqsetup & (1<<i))

#define	ATH_TXBUF_LOCK_INIT(_sc)    spin_lock_init(&(_sc)->sc_txbuflock)
#define	ATH_TXBUF_LOCK_DESTROY(_sc)
#define	ATH_TXBUF_LOCK(_sc)         spin_lock(&(_sc)->sc_txbuflock)
#define	ATH_TXBUF_UNLOCK(_sc)       spin_unlock(&(_sc)->sc_txbuflock)

#define	ATH_RXBUF_LOCK_INIT(_sc)    spin_lock_init(&(_sc)->sc_rxbuflock)
#define	ATH_RXBUF_LOCK_DESTROY(_sc)

#ifndef ATH_SUPPORT_UAPSD
#define	ATH_RXBUF_LOCK(_sc)         spin_lock(&(_sc)->sc_rxbuflock)
#define	ATH_RXBUF_UNLOCK(_sc)       spin_unlock(&(_sc)->sc_rxbuflock)
#else
#define	ATH_RXBUF_LOCK(_sc)         spin_lock_irqsave(&(_sc)->sc_rxbuflock, (_sc)->sc_rxbuflockflags)
#define	ATH_RXBUF_UNLOCK(_sc)       spin_unlock_irqrestore(&(_sc)->sc_rxbuflock, (_sc)->sc_rxbuflockflags)
#endif

#ifdef ATH_SUPPORT_UAPSD
#define	ATH_UAPSD_LOCK_INIT(_sc)    spin_lock_init(&(_sc)->sc_uapsdirqlock)
#define	ATH_UAPSD_LOCK_DESTROY(_sc)
#define	ATH_UAPSD_LOCK_IRQ(_sc)     spin_lock_irqsave(&(_sc)->sc_uapsdirqlock, (_sc)->sc_uapsdirqlockflags)
#define	ATH_UAPSD_UNLOCK_IRQ(_sc)   spin_unlock_irqrestore(&(_sc)->sc_uapsdirqlock, (_sc)->sc_uapsdirqlockflags)
#endif

#define	ATH_RESET_LOCK_INIT(_sc)    spin_lock_init(&(_sc)->sc_resetlock)
#define	ATH_RESET_LOCK_DESTROY(_sc)
#define	ATH_RESET_LOCK(_sc)         spin_lock(&(_sc)->sc_resetlock)
#define	ATH_RESET_UNLOCK(_sc)       spin_unlock(&(_sc)->sc_resetlock)

#define	ATH_RXFLUSH_LOCK_INIT(_sc)       spin_lock_init(&(_sc)->sc_rxflushlock)
#define	ATH_RXFLUSH_LOCK_DESTROY(_sc)
#define	ATH_RXFLUSH_LOCK(_sc)            spin_lock(&(_sc)->sc_rxflushlock)
#define	ATH_RXFLUSH_UNLOCK(_sc)          spin_unlock(&(_sc)->sc_rxflushlock)

#ifdef notyet               /* device lock ??? */
#define	ATH_LOCK_INIT(_sc)		init_MUTEX(&(_sc)->sc_lock)
#define	ATH_LOCK_DESTROY(_sc)
#define	ATH_LOCK(_sc)			down(&(_sc)->sc_lock)
#define	ATH_UNLOCK(_sc)			up(&(_sc)->sc_lock)
#endif /* ATHR_RNWF */

#if defined(ATH_SWRETRY) || defined(AP_SLOW_ANT_DIV)
#define	ATH_NODETABLE_LOCK_INIT(_sc)       spin_lock_init(&(_sc)->sc_nt_lock)
#define	ATH_NODETABLE_LOCK_DESTROY(_sc)
#define	ATH_NODETABLE_LOCK(_sc)            spin_lock(&(_sc)->sc_nt_lock)
#define	ATH_NODETABLE_UNLOCK(_sc)          spin_unlock(&(_sc)->sc_nt_lock)
#endif


/* WPS Push Button */
enum {
    ATH_WPS_BUTTON_EXISTS       = 0x00000001,     /* Push button Exists */
    ATH_WPS_BUTTON_PUSHED       = 0x00000002,     /* Push button Pressed since last query */
    ATH_WPS_BUTTON_PUSHED_CURR  = 0x00000008,     /* Push button Pressed currently */
    ATH_WPS_BUTTON_DOWN_SUP     = 0x00000010,     /* Push button down event supported */
    ATH_WPS_BUTTON_STATE_SUP    = 0x00000040      /* Push button current state reporting supported */
};

enum {
    ATH_DEBUG_XMIT          = 0x00000001,   /* basic xmit operation */
    ATH_DEBUG_XMIT_DESC     = 0x00000002,   /* xmit descriptors */
    ATH_DEBUG_RECV          = 0x00000004,   /* basic recv operation */
    ATH_DEBUG_RECV_DESC     = 0x00000008,   /* recv descriptors */
    ATH_DEBUG_RATE          = 0x00000010,   /* rate control */
    ATH_DEBUG_RESET         = 0x00000020,   /* reset processing */
    /* 0x00000040 was ATH_DEBUG_MODE */
    ATH_DEBUG_BEACON        = 0x00000080,   /* beacon handling */
    ATH_DEBUG_WATCHDOG      = 0x00000100,   /* watchdog timeout */
    ATH_DEBUG_INTR          = 0x00001000,   /* ISR */
    ATH_DEBUG_TX_PROC       = 0x00002000,   /* tx ISR proc */
    ATH_DEBUG_RX_PROC       = 0x00004000,   /* rx ISR proc */
    ATH_DEBUG_BEACON_PROC   = 0x00008000,   /* beacon ISR proc */
    ATH_DEBUG_CALIBRATE     = 0x00010000,   /* periodic calibration */
    ATH_DEBUG_KEYCACHE      = 0x00020000,   /* key cache management */
    ATH_DEBUG_STATE         = 0x00040000,   /* 802.11 state transitions */
    ATH_DEBUG_NODE          = 0x00080000,   /* node management */
    ATH_DEBUG_LED           = 0x00100000,   /* led management */
    ATH_DEBUG_FF            = 0x00200000,   /* fast frames */
    ATH_DEBUG_TURBO         = 0x00400000,   /* turbo/dynamice turbo */
    ATH_DEBUG_UAPSD         = 0x00800000,   /* uapsd */
    ATH_DEBUG_DOTH          = 0x01000000,   /* 11.h */
    ATH_DEBUG_CWM           = 0x02000000,   /* channel width managment */
    ATH_DEBUG_PPM           = 0x04000000,   /* Force PPM management */
    ATH_DEBUG_PWR_SAVE      = 0x08000000,   /* PS Poll and PS save */
    ATH_DEBUG_SWR           = 0x10000000,   /* SwRetry mechanism */
	ATH_DEBUG_AGGR_MEM		= 0x20000000,
    ATH_DEBUG_FATAL         = 0x80000000,   /* fatal errors */
    ATH_DEBUG_ANY           = 0xffffffff
};
#define	DPRINTF(sc, _m, _fmt, ...) do {         \
	if (sc->sc_debug & (_m))                \
		printk(_fmt, __VA_ARGS__);      \
} while (0)

#define KEYPRINTF(sc, ix, hk, mac) do {                         \
        if (sc->sc_debug & ATH_DEBUG_KEYCACHE)                  \
                ath_keyprint(__func__, ix, hk, mac);            \
} while (0)

/*
** Define the "default" debug mask
*/

#ifdef DBG
#define DBG_DEFAULT (ATH_DEBUG_FATAL       | ATH_DEBUG_STATE    | ATH_DEBUG_BEACON |\
                     ATH_DEBUG_BEACON_PROC | ATH_DEBUG_KEYCACHE | ATH_DEBUG_CALIBRATE)
#else
//#define DBG_DEFAULT (ATH_DEBUG_STATE  | ATH_DEBUG_KEYCACHE)
#define DBG_DEFAULT 0
#endif

#define ATH_ADDR_LEN            6
#define ATH_ADDR_EQ(a1,a2)      (OS_MEMCMP(a1,a2,ATH_ADDR_LEN) == 0)
#define ATH_ADDR_COPY(dst,src)  OS_MEMCPY(dst,src,ATH_ADDR_LEN)

/*
 * ************************
 * Internal interfaces
 * ************************
 */
#define __11nstats(sc, _x)          sc->sc_stats.ast_11n_stats._x ++
#define __11nstatsn(sc, _x, _n)     sc->sc_stats.ast_11n_stats._x += _n

/*
 * Control interfaces
 */
void ath_setdefantenna(void *sc, u_int antenna);
void ath_setslottime(struct ath_softc *sc);
void ath_update_txpow(struct ath_softc *sc, u_int16_t tpcInDb);
void ath_update_tpscale(struct ath_softc *sc);
void ath_setTxPwrLimit(ath_dev_t dev, u_int32_t limit, u_int16_t tpcInDb);

int ath_reset_start(ath_dev_t, u_int32_t);
int ath_reset_end(ath_dev_t, u_int32_t);
int ath_reset(ath_dev_t);
int ath_cabq_update(struct ath_softc *);

/*
 * Support interfaces
 */
void ath_hw_phystate_change(struct ath_softc *sc, int newstate);
u_int64_t ath_extend_tsf(struct ath_softc *sc, u_int32_t rstamp);
void ath_pcie_pwrsave_enable_on_phystate_change(struct ath_softc *sc, int enable);
void ath_pcie_pwrsave_enable(struct ath_softc *sc, int enable);
void ath_internal_reset(struct ath_softc *sc);

/*
 * Beacon interfaces
 */
void ath_beacon_sync(ath_dev_t dev, int if_id);
void ath_beacon_config(struct ath_softc *sc, int if_id);
int ath_beaconq_setup(struct ath_hal *ah);
int ath_beacon_alloc(struct ath_softc *sc, int if_id);
void ath_bstuck_tasklet(struct ath_softc *sc);
void ath_beacon_tasklet(struct ath_softc *sc, int *needmark);
void ath_beacon_free(struct ath_softc *sc);
void ath_beacon_return(struct ath_softc *sc, struct ath_vap *avp);
void ath_bmiss_tasklet(struct ath_softc *sc);
void ath_txto_tasklet(struct ath_softc *sc);
int ath_hw_hang_check(struct ath_softc *sc);

/*
 * Descriptor/MPDU interfaces
 */
int
ath_descdma_setup(
    struct ath_softc *sc,
    struct ath_descdma *dd, ath_bufhead *head,
    const char *name, int nbuf, int ndesc);

void
ath_descdma_cleanup(
    struct ath_softc *sc,
    struct ath_descdma *dd,
    ath_bufhead *head);

void ath_desc_swap(struct ath_desc *ds);

wbuf_t ath_rxbuf_alloc(struct ath_softc *sc, u_int32_t len);

/* sprintf is deprecated in Vista release build */
void ath_sprintf(char* string, u_int32_t len, const char* fmt, ...);

/*
 * Transmit interfaces
 */

int ath_tx_init(ath_dev_t, int nbufs);
int ath_tx_cleanup(ath_dev_t);
struct ath_txq *ath_txq_setup(struct ath_softc *sc, int qtype, int subtype);
void ath_tx_cleanupq(struct ath_softc *sc, struct ath_txq *txq);
int ath_tx_setup(struct ath_softc *sc, int haltype);
int ath_tx_get_qnum(ath_dev_t, int qtype, int haltype);
int ath_txq_update(ath_dev_t, int qnum, HAL_TXQ_INFO *qi);
void ath_tx_tasklet(ath_dev_t);
void ath_tx_flush(ath_dev_t);
void ath_draintxq(struct ath_softc *sc, HAL_BOOL retry_tx);
void ath_tx_draintxq(struct ath_softc *sc, struct ath_txq *txq, HAL_BOOL retry_tx);
void ath_tx_node_init(struct ath_softc *sc, struct ath_node *an);
void ath_tx_node_cleanup(struct ath_softc *sc, struct ath_node *an);
void ath_tx_node_free(struct ath_softc *sc, struct ath_node *an);
void ath_tx_node_pause(struct ath_softc *sc, struct ath_node *an);
void ath_tx_node_resume(struct ath_softc *sc, struct ath_node *an);
#ifdef ATH_CHAINMASK_SELECT
int ath_chainmask_sel_logic(struct ath_softc *sc, struct ath_node *an);
#endif
void ath_handle_tx_intr(struct ath_softc *sc);
int ath_tx_start(ath_dev_t, wbuf_t wbuf, ieee80211_tx_control_t *txctl);
u_int32_t ath_txq_depth(ath_dev_t, int);
u_int32_t ath_txq_aggr_depth(ath_dev_t, int);
u_int32_t ath_txq_aggr_nbuf(ath_dev_t, int);
u_int32_t ath_txq_lim(ath_dev_t, int);
int ath_tx_start_dma(wbuf_t, sg_t *sg, u_int32_t n_sg, void *arg);
void ath_set_protmode(ath_dev_t, PROT_MODE mode);

void ath_tx_txqaddbuf(struct ath_softc *sc, struct ath_txq *txq, ath_bufhead *head);
void ath_tx_complete_buf(struct ath_softc *sc, struct ath_buf *bf, ath_bufhead *bf_q,
                         int txok);
void ath_buf_set_rate(struct ath_softc *sc, struct ath_buf *bf);

#ifdef ATH_SWRETRY
/*
 * Transmit interfaces for software retry
 */
void ath_set_swretrystate(ath_dev_t dev, ath_node_t node, int flag);
int ath_check_swretry_req(struct ath_softc *sc, struct ath_buf *bf);
struct ath_buf *ath_form_swretry_frm(struct ath_softc *sc, struct ath_txq *txq,
                                     ath_bufhead *bf_q, struct ath_buf *bf);
void ath_seqno_recover(struct ath_softc *sc, struct ath_buf *bf);
void ath_tx_drain_sxmitq(struct ath_softc *sc, struct ath_node *an);
HAL_STATUS ath_tx_mpdu_resend(struct ath_softc *sc, struct ath_txq *txq,
                              ath_bufhead *bf_head);
void ath_tx_flush_sxmitq(struct ath_softc *sc);
void dumpTxQueue(struct ath_softc *sc, ath_bufhead *bfhead);
void ath_tx_reset_swretry(struct ath_softc *sc);
#endif

/* aggr interfaces */
int ath_aggr_check(ath_dev_t, ath_node_t node, u_int8_t tidno);
void ath_set_ampduparams(ath_dev_t, ath_node_t, u_int16_t maxampdu, u_int32_t mpdudensity);
void ath_set_weptkip_rxdelim(ath_dev_t dev, ath_node_t node, u_int8_t rxdelim);
void ath_addba_requestsetup(ath_dev_t, ath_node_t,
                            u_int8_t tidno, struct ieee80211_ba_parameterset *baparamset,
                            u_int16_t *batimeout, struct ieee80211_ba_seqctrl *basequencectrl,
                            u_int16_t buffersize);
void ath_addba_responsesetup(ath_dev_t, ath_node_t,
                             u_int8_t tidno, u_int8_t *dialogtoken,
                             u_int16_t *statuscode,
                             struct ieee80211_ba_parameterset *baparamset,
                             u_int16_t *batimeout);
int ath_addba_requestprocess(ath_dev_t, ath_node_t,
                             u_int8_t dialogtoken, struct ieee80211_ba_parameterset *baparamset,
                             u_int16_t batimeout, struct ieee80211_ba_seqctrl basequencectrl);
void ath_addba_responseprocess(ath_dev_t, ath_node_t,
                               u_int16_t statuscode, struct ieee80211_ba_parameterset *baparamset,
                               u_int16_t batimeout);
void ath_addba_clear(ath_dev_t, ath_node_t);
void ath_delba_process(ath_dev_t, ath_node_t,
                       struct ieee80211_delba_parameterset *delbaparamset, u_int16_t reasoncode);
u_int16_t ath_addba_status(ath_dev_t dev, ath_node_t node, u_int8_t tidno);
void ath_aggr_teardown(ath_dev_t dev, ath_node_t node, u_int8_t tidno, u_int8_t initiator);
void ath_tx_aggr_teardown(struct ath_softc *sc, struct ath_node *an, u_int8_t tidno);
void ath_rx_aggr_teardown(struct ath_softc *sc, struct ath_node *an, u_int8_t tidno);
void ath_set_addbaresponse(ath_dev_t dev, ath_node_t node,
                           u_int8_t tidno, u_int16_t statuscode);
void ath_clear_addbaresponsestatus(ath_dev_t dev, ath_node_t node);

/*
 * Receive interfaces
 */
int ath_rx_init(ath_dev_t, int nbufs);
void ath_rx_cleanup(ath_dev_t);
int ath_startrecv(struct ath_softc *sc);
HAL_BOOL ath_stoprecv(struct ath_softc *sc);
void ath_flushrecv(struct ath_softc *sc);
void ath_rx_requeue(ath_dev_t, wbuf_t wbuf);
u_int32_t ath_calcrxfilter(struct ath_softc *sc);
void ath_rx_node_init(struct ath_softc *sc, struct ath_node *an);
void ath_rx_node_free(struct ath_softc *sc, struct ath_node *an);
void ath_rx_node_cleanup(struct ath_softc *sc, struct ath_node *an);
void ath_handle_rx_intr(struct ath_softc *sc);
int ath_rx_indicate(struct ath_softc *sc, wbuf_t wbuf, ieee80211_rx_status_t *status, u_int16_t keyix);
int ath_rx_input(ath_dev_t, ath_node_t node, int is_ampdu,
                 wbuf_t wbuf, ieee80211_rx_status_t *rx_status,
                 ATH_RX_TYPE *status);

/* Parameters for ath_rx_tasklet */
#define RX_PROCESS          0   /* Process rx frames in rx interrupt. */
#define RX_DROP             1   /* Drop rx frames in flush routine.*/
#define RX_FORCE_PROCESS    2   /* Flush and indicate rx frames */
int ath_rx_tasklet(ath_dev_t, int flush);

void ath_opmode_init(ath_dev_t);

/* PHY state interfaces */
int ath_get_sw_phystate(ath_dev_t);
int ath_get_hw_phystate(ath_dev_t);
void ath_set_sw_phystate(ath_dev_t, int swstate);
int ath_radio_disable(ath_dev_t);
int ath_radio_enable(ath_dev_t);

/* PnP interfaces */
void ath_notify_device_removal(ath_dev_t);
int ath_detect_card_present(ath_dev_t);

/* fast frame interfaces */
int ath_ff_check(ath_dev_t dev, ath_node_t node, int qnum,
                 int check_qdepth, u_int32_t txoplimit, u_int32_t frameLen);

/* AMSDU frame tx interfaces */
int ath_get_amsdusupported(ath_dev_t dev, ath_node_t node, int tidno);

#ifdef ATH_WOW
/* WakeonWireless interfaces */
int ath_get_wow_support(ath_dev_t dev);
int ath_set_wow_enable(ath_dev_t dev);
int ath_wow_wakeup(ath_dev_t dev);
void ath_set_wow_events(ath_dev_t dev, u_int32_t wowEvents);
int ath_get_wow_events(ath_dev_t dev);
int ath_wow_add_wakeup_pattern(ath_dev_t dev, u_int8_t *patternBytes, u_int8_t *maskBytes, u_int32_t patternLen);
int ath_wow_remove_wakeup_pattern(ath_dev_t dev, u_int8_t *patternBytes, u_int8_t *maskBytes);
int ath_get_wow_wakeup_reason(ath_dev_t dev);
int ath_wow_matchpattern_exact(ath_dev_t dev);
void ath_wow_set_duration(ath_dev_t dev, u_int16_t duration);
#endif

#ifdef ATH_SUPPORT_UAPSD
/* UAPSD Interface */
int ath_process_uapsd_trigger(ath_dev_t dev, ath_node_t node, u_int8_t maxsp,
                              u_int8_t ac, u_int8_t flush);
int ath_tx_uapsd_init(struct ath_softc *sc);
void ath_tx_uapsd_cleanup(struct ath_softc *sc);
void ath_tx_uapsd_node_cleanup(struct ath_softc *sc, struct ath_node *an);
u_int32_t ath_tx_uapsd_depth(ath_node_t node);
void ath_tx_queue_uapsd(struct ath_softc *sc, struct ath_txq *txq, ath_bufhead *bf_head, ieee80211_tx_control_t *txctl);
void ath_tx_uapsd_complete(struct ath_softc *sc, struct ath_node *an, struct ath_buf *bf, ath_bufhead *bf_q, int txok);
void ath_tx_uapsdqnulbf_reclaim(struct ath_softc *sc, struct ath_buf *bf, ath_bufhead *bf_q);
void ath_tx_uapsd_draintxq(struct ath_softc *sc);
void ath_check_uapsdtriggers(ath_dev_t dev);
#endif
#ifdef ATH_SUPPORT_DFS
int ath_check_dfs_wait(ath_dev_t dev);
#endif

/*
** Configuration Prototypes
*/

int ath_get_config(ath_dev_t dev, ath_param_ID_t ID, void *buff);
int ath_set_config(ath_dev_t dev, ath_param_ID_t ID, void *buff);

/*
* DFS prototypes
*/
#ifdef ATH_SUPPORT_DFS
int 
ath_dfs_control(ath_dev_t dev, u_int id,
                void *indata, u_int32_t insize,
                void *outdata, u_int32_t *outsize);
#endif

#ifdef ATH_CCX
int         ath_update_mib_macstats(ath_dev_t dev);
int         ath_get_mib_macstats(ath_dev_t dev, struct ath_mib_mac_stats *pStats);
u_int8_t    ath_rcRateValueToPer(ath_dev_t, struct ath_node *, int);
int         ath_get_mib_cyclecounts(ath_dev_t dev, struct ath_mib_cycle_cnts *pCnts);
void        ath_clear_mib_counters(ath_dev_t dev);
u_int32_t   ath_gettsf32(ath_dev_t dev);
u_int64_t   ath_gettsf64(ath_dev_t dev);
void        ath_setrxfilter(ath_dev_t dev);
int         ath_getserialnumber(ath_dev_t dev, u_int8_t *pSerNum);
int         ath_getchandata(ath_dev_t dev, struct ieee80211_channel *pChan, struct ath_chan_data *pData);
u_int32_t   ath_getcurrssi(ath_dev_t dev);
#endif

u_int8_t ath_get_common_power(u_int16_t freq);

/*
 * HAL definitions to comply with local coding convention.
 */
#define ath_hal_reset(_ah, _opmode, _chan, _macmode, _txchainmask, _rxchainmask, _extprotspacing, _outdoor, _pstatus) \
    ((*(_ah)->ah_reset)((_ah), (_opmode), (_chan), (_macmode), (_txchainmask), (_rxchainmask), (_extprotspacing), (_outdoor), (_pstatus)))
#define ath_hal_getratetable(_ah, _mode) \
    ((*(_ah)->ah_getRateTable)((_ah), (_mode)))
#define ath_hal_getmac(_ah, _mac) \
    ((*(_ah)->ah_getMacAddress)((_ah), (_mac)))
#define ath_hal_setmac(_ah, _mac) \
    ((*(_ah)->ah_setMacAddress)((_ah), (_mac)))
#define ath_hal_getbssidmask(_ah, _mask) \
    ((*(_ah)->ah_getBssIdMask)((_ah), (_mask)))
#define ath_hal_setbssidmask(_ah, _mask) \
    ((*(_ah)->ah_setBssIdMask)((_ah), (_mask)))
#define ath_hal_intrset(_ah, _mask) \
    ((*(_ah)->ah_setInterrupts)((_ah), (_mask)))
#define ath_hal_intrget(_ah) \
    ((*(_ah)->ah_getInterrupts)((_ah)))
#define ath_hal_intrpend(_ah) \
    ((*(_ah)->ah_isInterruptPending)((_ah)))
#define ath_hal_getisr(_ah, _pmask) \
    ((*(_ah)->ah_getPendingInterrupts)((_ah), (_pmask)))
#define ath_hal_updatetxtriglevel(_ah, _inc) \
    ((*(_ah)->ah_updateTxTrigLevel)((_ah), (_inc)))
#define ath_hal_gettxtriglevel(_ah) \
    ((*(_ah)->ah_getTxTrigLevel)((_ah)))
#define ath_hal_setpower(_ah, _mode) \
    ((*(_ah)->ah_setPowerMode)((_ah), (_mode), AH_TRUE))
#define ath_hal_getpower(_ah) \
    ((*(_ah)->ah_getPowerMode)((_ah)))
#define ath_hal_setsmpsmode(_ah, _mode) \
    ((*(_ah)->ah_setSmPsMode)((_ah), (_mode)))
#ifdef ATH_WOW                
#define ath_hal_hasWow(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_WOW, 0, NULL) == HAL_OK)
#define ath_hal_wowApplyPattern(_ah, _ppattern, _pmask, _count, _plen) \
    ((*(_ah)->ah_wowApplyPattern)((_ah), (_ppattern), (_pmask), (_count), (_plen)))
#define ath_hal_wowEnable(_ah, _patternEnable) \
    ((*(_ah)->ah_wowEnable)((_ah), (_patternEnable)))
#define ath_hal_wowWakeUp(_ah) \
    ((*(_ah)->ah_wowWakeUp)((_ah)))
#define ath_hal_wowMatchPatternExact(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_WOW_MATCH_EXACT, 0, NULL) == HAL_OK)
#define ath_hal_wowMatchPatternDword(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_WOW_MATCH_DWORD, 0, NULL) == HAL_OK)
#endif        
#define ath_hal_keycachesize(_ah) \
    ((*(_ah)->ah_getKeyCacheSize)((_ah)))
#define ath_hal_keyreset(_ah, _ix) \
    ((*(_ah)->ah_resetKeyCacheEntry)((_ah), (_ix)))
#define ath_hal_keyset(_ah, _ix, _pk, _mac) \
    ((*(_ah)->ah_setKeyCacheEntry)((_ah), (_ix), (_pk), (_mac), AH_FALSE))
#define ath_hal_divant(_ah, _ix) \
    ((*(_ah)->ah_divant)((_ah), (_ix)))
#define ath_hal_keyisvalid(_ah, _ix) \
    (((*(_ah)->ah_isKeyCacheEntryValid)((_ah), (_ix))))
#define ath_hal_keysetmac(_ah, _ix, _mac) \
    ((*(_ah)->ah_setKeyCacheEntryMac)((_ah), (_ix), (_mac)))
#define ath_hal_getrxfilter(_ah) \
    ((*(_ah)->ah_getRxFilter)((_ah)))
#define ath_hal_setrxfilter(_ah, _filter) \
    ((*(_ah)->ah_setRxFilter)((_ah), (_filter)))
#define	ath_hal_setrxabort(_ah, _set) \
	((*(_ah)->ah_setRxAbort)((_ah), ((HAL_BOOL)(_set))))
#define ath_hal_setmcastfilter(_ah, _mfilt0, _mfilt1) \
    ((*(_ah)->ah_setMulticastFilter)((_ah), (_mfilt0), (_mfilt1)))
#define ath_hal_waitforbeacon(_ah, _bf) \
    ((*(_ah)->ah_waitForBeaconDone)((_ah), (_bf)->bf_daddr))
#define ath_hal_putrxbuf(_ah, _bufaddr) \
    ((*(_ah)->ah_setRxDP)((_ah), (_bufaddr)))
#define ath_hal_gettsf32(_ah) \
    ((*(_ah)->ah_getTsf32)((_ah)))
#define ath_hal_gettsf64(_ah) \
    ((*(_ah)->ah_getTsf64)((_ah)))
#define ath_hal_resettsf(_ah) \
    ((*(_ah)->ah_resetTsf)((_ah)))
#define ath_hal_detectcardpresent(_ah) \
    ((*(_ah)->ah_detectCardPresent)((_ah)))
#define ath_hal_rxena(_ah) \
    ((*(_ah)->ah_enableReceive)((_ah)))
#define ath_hal_numtxpending(_ah, _q) \
    ((*(_ah)->ah_numTxPending)((_ah), (_q)))
#define ath_hal_puttxbuf(_ah, _q, _bufaddr) \
    ((*(_ah)->ah_setTxDP)((_ah), (_q), (_bufaddr)))
#define ath_hal_gettxbuf(_ah, _q) \
    ((*(_ah)->ah_getTxDP)((_ah), (_q)))
#define ath_hal_getrxbuf(_ah) \
    ((*(_ah)->ah_getRxDP)((_ah)))
#define ath_hal_txstart(_ah, _q) \
    ((*(_ah)->ah_startTxDma)((_ah), (_q)))
#define ath_hal_setchannel(_ah, _chan) \
    ((*(_ah)->ah_setChannel)((_ah), (_chan)))
#define ath_hal_calibrate(_ah, _chan, _rxchainmask, _longcal, _isIQdone) \
    ((*(_ah)->ah_perCalibration)((_ah), (_chan), (_rxchainmask), (_longcal), (_isIQdone)))
#define ath_hal_reset_calvalid(_ah, _chan, _isIQdone) \
    ((*(_ah)->ah_resetCalValid)((_ah), (_chan), (_isIQdone)))
#define ath_hal_setledstate(_ah, _state) \
    ((*(_ah)->ah_setLedState)((_ah), (_state)))
#define ath_hal_beaconinit(_ah, _nextb, _bperiod) \
    ((*(_ah)->ah_beaconInit)((_ah), (_nextb), (_bperiod)))
#define ath_hal_beaconreset(_ah) \
    ((*(_ah)->ah_resetStationBeaconTimers)((_ah)))
#define ath_hal_beacontimers(_ah, _bs) \
    ((*(_ah)->ah_setStationBeaconTimers)((_ah), (_bs)))
#define ath_hal_setassocid(_ah, _bss, _associd) \
    ((*(_ah)->ah_writeAssocid)((_ah), (_bss), (_associd)))
#define ath_hal_phydisable(_ah) \
    ((*(_ah)->ah_phyDisable)((_ah)))
#define ath_hal_disable(_ah) \
    ((*(_ah)->ah_disable)((_ah)))
#define ath_hal_setopmode(_ah) \
    ((*(_ah)->ah_setPCUConfig)((_ah)))
#define ath_hal_configpcipowersave(_ah, _restore) \
    ((*(_ah)->ah_configPciPowerSave)((_ah), (_restore)))
#define ath_hal_stoptxdma(_ah, _qnum) \
    ((*(_ah)->ah_stopTxDma)((_ah), (_qnum)))
#define ath_hal_stoppcurecv(_ah) \
    ((*(_ah)->ah_stopPcuReceive)((_ah)))
#define ath_hal_startpcurecv(_ah) \
    ((*(_ah)->ah_startPcuReceive)((_ah)))
#define ath_hal_stopdmarecv(_ah) \
    ((*(_ah)->ah_stopDmaReceive)((_ah)))
#define ath_hal_getdiagstate(_ah, _id, _indata, _insize, _outdata, _outsize) \
    ((*(_ah)->ah_getDiagState)((_ah), (_id), \
        (_indata), (_insize), (_outdata), (_outsize)))
#define ath_hal_gettxqueueprops(_ah, _q, _qi) \
    ((*(_ah)->ah_getTxQueueProps)((_ah), (_q), (_qi)))
#define ath_hal_settxqueueprops(_ah, _q, _qi) \
    ((*(_ah)->ah_setTxQueueProps)((_ah), (_q), (_qi)))
#define ath_hal_setuptxqueue(_ah, _type, _irq) \
    ((*(_ah)->ah_setupTxQueue)((_ah), (_type), (_irq)))
#define ath_hal_resettxqueue(_ah, _q) \
    ((*(_ah)->ah_resetTxQueue)((_ah), (_q)))
#define ath_hal_releasetxqueue(_ah, _q) \
    ((*(_ah)->ah_releaseTxQueue)((_ah), (_q)))
#define ath_hal_getrfgain(_ah) \
    ((*(_ah)->ah_getRfGain)((_ah)))
#define ath_hal_getdefantenna(_ah) \
    ((*(_ah)->ah_getDefAntenna)((_ah)))
#define ath_hal_setdefantenna(_ah, _ant) \
    ((*(_ah)->ah_setDefAntenna)((_ah), (_ant)))
#define ath_hal_setAntennaSwitch(_ah, _ant, _chan, _txchmsk, _rxchmsk, _antcfgd) \
    ((*(_ah)->ah_setAntennaSwitch)((_ah), (_ant), (_chan), (_txchmsk), (_rxchmsk), (_antcfgd)))
#define ath_hal_selectAntConfig(_ah, _cfg) \
    ((*(ah)->ah_selectAntConfig)((_ah), (_cfg)))
#define ath_hal_rxmonitor(_ah, _arg, _chan) \
    ((*(_ah)->ah_rxMonitor)((_ah), (_arg), (_chan)))
#define ath_hal_mibevent(_ah, _stats) \
    ((*(_ah)->ah_procMibEvent)((_ah), (_stats)))
#define ath_hal_setslottime(_ah, _us) \
    ((*(_ah)->ah_setSlotTime)((_ah), (_us)))
#define ath_hal_getslottime(_ah) \
    ((*(_ah)->ah_getSlotTime)((_ah)))
#define ath_hal_setacktimeout(_ah, _us) \
    ((*(_ah)->ah_setAckTimeout)((_ah), (_us)))
#define ath_hal_getacktimeout(_ah) \
    ((*(_ah)->ah_getAckTimeout)((_ah)))
#define ath_hal_setctstimeout(_ah, _us) \
    ((*(_ah)->ah_setCTSTimeout)((_ah), (_us)))
#define ath_hal_getctstimeout(_ah) \
    ((*(_ah)->ah_getCTSTimeout)((_ah)))
#define ath_hal_setdecompmask(_ah, _keyid, _b) \
    ((*(_ah)->ah_setDecompMask)((_ah), (_keyid), (_b)))
#define ath_hal_enablePhyDiag(_ah) \
    ((*(_ah)->ah_enablePhyErrDiag)((_ah)))
#define ath_hal_disablePhyDiag(_ah) \
    ((*(_ah)->ah_disablePhyErrDiag)((_ah)))
#define ath_hal_getcapability(_ah, _cap, _param, _result) \
    ((*(_ah)->ah_getCapability)((_ah), (_cap), (_param), (_result)))
#define ath_hal_setcapability(_ah, _cap, _param, _v, _status) \
    ((*(_ah)->ah_setCapability)((_ah), (_cap), (_param), (_v), (_status)))
#define ath_hal_ciphersupported(_ah, _cipher) \
    (ath_hal_getcapability(_ah, HAL_CAP_CIPHER, _cipher, NULL) == HAL_OK)
#define ath_hal_fastframesupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_FASTFRAME, 0, NULL) == HAL_OK)
#define ath_hal_burstsupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_BURST, 0, NULL) == HAL_OK)
#define ath_hal_xrsupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_XR, 0, NULL) == HAL_OK)
#define ath_hal_compressionsupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_COMPRESSION, 0, NULL) == HAL_OK)
#define ath_hal_htsupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_HT, 0, NULL) == HAL_OK)
#define ath_hal_rxstbcsupport(_ah, _rxstbc) \
    (ath_hal_getcapability(_ah, HAL_CAP_RX_STBC, 0, _rxstbc) == HAL_OK)
#define ath_hal_txstbcsupport(_ah, _txstbc) \
    (ath_hal_getcapability(_ah, HAL_CAP_TX_STBC, 0, _txstbc) == HAL_OK)
#define ath_hal_weptkipaggrsupport(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_WEP_TKIP_AGGR, 0, NULL) == HAL_OK)
#define ath_hal_gettxdelimweptkipaggr(_ah, _pv) \
    (ath_hal_getcapability(_ah, HAL_CAP_WEP_TKIP_AGGR_TX_DELIM, 0, _pv) == HAL_OK)
#define ath_hal_getrxdelimweptkipaggr(_ah, _pv) \
    (ath_hal_getcapability(_ah, HAL_CAP_WEP_TKIP_AGGR_RX_DELIM, 0, _pv) == HAL_OK)
#define ath_hal_singleframeaggrsupport(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_SINGLE_AGGR_SAFE, 0, NULL) == HAL_OK)
#define ath_hal_gttsupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_GTT, 0, NULL) == HAL_OK)
#define ath_hal_turboagsupported(_ah, _ath_countrycode) \
    (ath_hal_getwirelessmodes(_ah, _ath_countrycode) & (HAL_MODE_108G|HAL_MODE_TURBO))
#define ath_hal_halfrate_chansupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_CHAN_HALFRATE, 0, NULL) == HAL_OK)
#define ath_hal_quarterrate_chansupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_CHAN_QUARTERRATE, 0, NULL) == HAL_OK)
#define ath_hal_getregdomain(_ah, _prd) \
    ath_hal_getcapability(_ah, HAL_CAP_REG_DMN, 0, (_prd))
#define ath_hal_setregdomain(_ah, _regdmn, _statusp) \
    ((*(_ah)->ah_setRegulatoryDomain)((_ah), (_regdmn), (_statusp)))
#define ath_hal_getcountrycode(_ah, _pcc) \
    (*(_pcc) = (_ah)->ah_countryCode)
#define ath_hal_tkipsplit(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TKIP_SPLIT, 0, NULL) == HAL_OK)
#define ath_hal_wmetkipmic(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_WME_TKIPMIC, 0, NULL) == HAL_OK)
#define ath_hal_hwphycounters(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_PHYCOUNTERS, 0, NULL) == HAL_OK)
#define ath_hal_hasdiversity(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_DIVERSITY, 0, NULL) == HAL_OK)
#define ath_hal_getdiversity(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_DIVERSITY, 1, NULL) == HAL_OK)
#define ath_hal_setdiversity(_ah, _v) \
    ath_hal_setcapability(_ah, HAL_CAP_DIVERSITY, 1, _v, NULL)
#define ath_hal_getnumtxqueues(_ah, _pv) \
    (ath_hal_getcapability(_ah, HAL_CAP_NUM_TXQUEUES, 0, _pv) == HAL_OK)
#define ath_hal_hasveol(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_VEOL, 0, NULL) == HAL_OK)
#define ath_hal_hastxpowlimit(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TXPOW, 0, NULL) == HAL_OK)
#define ath_hal_settxpowlimit(_ah, _pow, _tpcInDb) \
    ((*(_ah)->ah_setTxPowerLimit)((_ah), (_pow), (_tpcInDb)))
#define ath_hal_gettxpowlimit(_ah, _ppow) \
    ath_hal_getcapability(_ah, HAL_CAP_TXPOW, 1, _ppow)
#define ath_hal_getmaxtxpow(_ah, _ppow) \
    ath_hal_getcapability(_ah, HAL_CAP_TXPOW, 2, _ppow)
#define ath_hal_gettpscale(_ah, _scale) \
    ath_hal_getcapability(_ah, HAL_CAP_TXPOW, 3, _scale)
#define ath_hal_settpscale(_ah, _v) \
    ath_hal_setcapability(_ah, HAL_CAP_TXPOW, 3, _v, NULL)
#define ath_hal_hastpc(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TPC, 0, NULL) == HAL_OK)
#define ath_hal_gettpc(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TPC, 1, NULL) == HAL_OK)
#define ath_hal_settpc(_ah, _v) \
    ath_hal_setcapability(_ah, HAL_CAP_TPC, 1, _v, NULL)
#define ath_hal_hasbursting(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_BURST, 0, NULL) == HAL_OK)
#define ath_hal_hascompression(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_COMPRESSION, 0, NULL) == HAL_OK)
#define ath_hal_hasfastframes(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_FASTFRAME, 0, NULL) == HAL_OK)
#define ath_hal_hasbssidmask(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_BSSIDMASK, 0, NULL) == HAL_OK)
#define ath_hal_setmcastkeysearch(_ah, _v)                                 \
    ath_hal_setcapability(_ah, HAL_CAP_MCAST_KEYSRCH, 1, _v, NULL)
#define ath_hal_hasmcastkeysearch(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_MCAST_KEYSRCH, 0, NULL) == HAL_OK)
#define ath_hal_getmcastkeysearch(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_MCAST_KEYSRCH, 1, NULL) == HAL_OK)
#define ath_hal_hastkipmic(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TKIP_MIC, 0, NULL) == HAL_OK)
#define ath_hal_gettkipmic(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TKIP_MIC, 1, NULL) == HAL_OK)
#define ath_hal_settkipmic(_ah, _v) \
    ath_hal_setcapability(_ah, HAL_CAP_TKIP_MIC, 1, _v, NULL)
#define ath_hal_hastsfadjust(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TSF_ADJUST, 0, NULL) == HAL_OK)
#define ath_hal_gettsfadjust(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_TSF_ADJUST, 1, NULL) == HAL_OK)
#define ath_hal_settsfadjust(_ah, _v) \
    ath_hal_setcapability(_ah, HAL_CAP_TSF_ADJUST, 1, _v, NULL)
#define ath_hal_hasenhancedpmsupport(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_ENHANCED_PM_SUPPORT, 0, NULL) == HAL_OK)
#define ath_hal_haswpsbutton(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_WPS_PUSH_BUTTON, 0, NULL) == HAL_OK)
    
#define ath_hal_AntDivCombSupport(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_ANT_DIV_COMB, 0, NULL) == HAL_OK)    

#define ath_hal_setuprxdesc(_ah, _ds, _size, _intreq) \
    ((*(_ah)->ah_setupRxDesc)((_ah), (_ds), (_size), (_intreq)))
#define ath_hal_rxprocdesc(_ah, _ds, _dspa, _dsnext, _tsf) \
    ((*(_ah)->ah_procRxDesc)((_ah), (_ds), (_dspa), (_dsnext), (_tsf)))
#define ath_hal_updateCTSForBursting(_ah, _ds, _prevds, _prevdsWithCTS, _gatingds,    \
                                     _txOpLimit, _ctsDuration)                        \
    ((*(_ah)->ah_updateCTSForBursting)((_ah), (_ds), (_prevds), (_prevdsWithCTS), \
                                       (_gatingds), (_txOpLimit), (_ctsDuration)))
#define ath_hal_setuptxdesc(_ah, _ds, _plen, _hlen, _atype, _txpow, \
                            _txr0, _txtr0, _keyix, _ant, _flags, \
                            _rtsrate, _rtsdura, \
                            _compicvlen, _compivlen, _comp) \
    ((*(_ah)->ah_setupTxDesc)((_ah), (_ds), (_plen), (_hlen), (_atype), \
        (_txpow), (_txr0), (_txtr0), (_keyix), (_ant), \
        (_flags), (_rtsrate), (_rtsdura), \
        (_compicvlen), (_compivlen), (_comp)))
#define ath_hal_setupxtxdesc(_ah, _ds, \
                             _txr1, _txtr1, _txr2, _txtr2, _txr3, _txtr3) \
    ((*(_ah)->ah_setupXTxDesc)((_ah), (_ds), \
        (_txr1), (_txtr1), (_txr2), (_txtr2), (_txr3), (_txtr3)))
#define ath_hal_filltxdesc(_ah, _ds, _l, _first, _last, _ds0) \
    ((*(_ah)->ah_fillTxDesc)((_ah), (_ds), (_l), (_first), (_last), (_ds0)))
#define ath_hal_cleartxdesc(_ah, _ds) \
    ((*(_ah)->ah_clearTxDescStatus)((_ah), (_ds)))
#ifdef ATH_SWRETRY        
#define ath_hal_cleardestmask(_ah, _ds) \
    ((*(_ah)->ah_clearDestMask)((_ah), (_ds)))        
#endif        
#define ath_hal_txprocdesc(_ah, _ds) \
    ((*(_ah)->ah_procTxDesc)((_ah), (_ds)))
#define ath_hal_gettxintrtxqs(_ah, _txqs) \
    ((*(_ah)->ah_getTxIntrQueue)((_ah), (_txqs)))
#define ath_hal_txreqintrdesc(_ah, _ds) \
    ((*(_ah)->ah_reqTxIntrDesc)((_ah), (_ds)))
#define ath_hal_txcalcairtime(_ah, _ds) \
    ((*(_ah)->ah_calcTxAirtime)((_ah), (_ds)))

#define ath_hal_gpioCfgInput(_ah, _gpio) \
    ((*(_ah)->ah_gpioCfgInput)((_ah), (_gpio)))
#define ath_hal_gpioCfgOutput(_ah, _gpio, _signalType) \
    ((*(_ah)->ah_gpioCfgOutput)((_ah), (_gpio), (_signalType)))
#define ath_hal_gpioset(_ah, _gpio, _b) \
    ((*(_ah)->ah_gpioSet)((_ah), (_gpio), (_b)))
#define ath_hal_gpioget(_ah, _gpio) \
        ((*(_ah)->ah_gpioGet)((_ah), (_gpio)))
#define	ath_hal_setcoverageclass(_ah, _coverageclass, _now) \
	((*(_ah)->ah_setCoverageClass)((_ah), (_coverageclass), (_now)))
#define	ath_hal_setQuiet(_ah, _period, _duration, _nextStart,_enabled) \
	((*(_ah)->ah_setQuiet)((_ah), (_period),(_duration), (_nextStart), (_enabled)))

#define ath_hal_setpowerledstate(_ah, _enable) \
    ((*(_ah)->ah_setpowerledstate)((_ah), (_enable)))
#define ath_hal_setnetworkledstate(_ah, _enable) \
    ((*(_ah)->ah_setnetworkledstate)((_ah), (_enable)))

#define ath_hal_mark_phy_inactive(_ah) \
    ((*(_ah)->ah_markPhyInactive)((_ah)))

/* DFS defines */
#define ath_hal_radar_wait(_ah, _chan) \
    ((*(_ah)->ah_radarWait)((_ah), (_chan)))
#define ath_hal_checkdfs(_ah, _chan) \
        ((*(_ah)->ah_arCheckDfs)((_ah), (_chan)))
#define ath_hal_dfsfound(_ah, _chan, _time) \
        ((*(_ah)->ah_arDfsFound)((_ah), (_chan), (_time)))
#define ath_hal_enabledfs(_ah, _param) \
        ((*(_ah)->ah_arEnableDfs)((_ah), (_param)))
#define ath_hal_getdfsthresh(_ah, _param) \
        ((*(_ah)->ah_arGetDfsThresh)((_ah), (_param)))
#define ath_hal_getdfsradars(_ah, _domain, _numrdr, _bin5pulses, _numb5rdrs, _pe) \
        ((*(_ah)->ah_arGetDfsRadars)((_ah), (_domain), (_numrdr), (_bin5pulses), (_numb5rdrs), (_pe)))
#define ath_hal_get_extension_channel(_ah) \
        ((*(_ah)->ah_getExtensionChannel)((_ah)))
#define ath_hal_is_fast_clock_enabled(_ah) \
        ((*(_ah)->ah_isFastClockEnabled)((_ah)))

extern int16_t ath_hal_getChanNoise(struct ath_hal *ah, HAL_CHANNEL *chan);

#define ath_hal_gpioGet(_ah, _gpio) \
    ((*(_ah)->ah_gpioGet)((_ah), (_gpio)))
#define ath_hal_gpioSetIntr(_ah, _gpio, _ilevel) \
    ((*(_ah)->ah_gpioSetIntr)((_ah), (_gpio), (_ilevel)))

#define ath_hal_hasrfkill(_ah)  \
    (ath_hal_getcapability(_ah, HAL_CAP_RFSILENT, 0, NULL) == HAL_OK)
#define ath_hal_isrfkillenabled(_ah)  \
    (ath_hal_getcapability(_ah, HAL_CAP_RFSILENT, 1, NULL) == HAL_OK)
#define ath_hal_getrfkillinfo(_ah, _ri)  \
    ath_hal_getcapability(_ah, HAL_CAP_RFSILENT, 2, (void *)(_ri))
#define ath_hal_enable_rfkill(_ah, _v) \
    ath_hal_setcapability(_ah, HAL_CAP_RFSILENT, 1, _v, NULL)
#define ath_hal_hasrfkillInt(_ah)  \
    (ath_hal_getcapability(_ah, HAL_CAP_RFSILENT, 3, NULL) == HAL_OK)
#define ath_hal_hasPciePwrsave(_ah)  \
    (ath_hal_getcapability(_ah, HAL_CAP_PCIE_PS, 0, NULL) == HAL_OK)
#define ath_hal_isPciePwrsaveEnabled(_ah)  \
    (ath_hal_getcapability(_ah, HAL_CAP_PCIE_PS, 1, NULL) == HAL_OK)
#define ath_hal_hasMbssidAggrSupport(_ah, _pv)  \
    (ath_hal_getcapability(_ah, HAL_CAP_MBSSID_AGGR_SUPPORT, 0, _pv))

#define ath_hal_getdescinfo(_ah, _descinfo) \
    ((*(_ah)->ah_getDescInfo)((_ah), (_descinfo)))

#define ath_hal_hasfastcc(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_FAST_CC, 0, NULL) == HAL_OK)
#define ath_hal_gettxchainmask(_ah, _pv)  \
    (ath_hal_getcapability(_ah, HAL_CAP_TX_CHAINMASK, 0, _pv) == HAL_OK)
#define ath_hal_getrxchainmask(_ah, _pv)  \
    (ath_hal_getcapability(_ah, HAL_CAP_RX_CHAINMASK, 0, _pv) == HAL_OK)
#define ath_hal_hascst(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_CST, 0, NULL) == HAL_OK)
#define ath_hal_hasrifsrx(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_RIFS_RX, 0, NULL) == HAL_OK)
#define ath_hal_hasrifstx(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_RIFS_TX, 0, NULL) == HAL_OK)
#define ath_hal_getrtsaggrlimit(_ah, _pv) \
    ath_hal_getcapability(_ah, HAL_CAP_RTS_AGGR_LIMIT, 0, _pv)
#define ath_hal_4addraggrsupported(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_4ADDR_AGGR, 0, NULL) == HAL_OK)
#define ath_hal_bb_rifs_rx_enabled(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_RIFS_RX_ENABLED, 0, NULL) == HAL_OK)
#define ath_hal_get_hang_types(_ah, _types) \
    (*(_ah)->ah_getHangTypes)((_ah), (_types))

#define ath_hal_hasautosleep(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_AUTO_SLEEP, 0, NULL) == HAL_OK)

#define ath_hal_has4kbsplittrans(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_4KB_SPLIT_TRANS, 0, NULL) == HAL_OK)

/* 11n */
#define ath_hal_set11n_txdesc(_ah, _ds, _pktlen, _type, _txpower,           \
    _keyix, _keytype, _flags)                                               \
    ((*(_ah)->ah_set11nTxDesc)(_ah, _ds, _pktlen, _type, _txpower, _keyix,  \
    _keytype, _flags))
#define ath_hal_set11n_ratescenario(_ah, _ds, _lastds, _durupdate, _rtsctsrate, _rtsctsduration,    \
        _series, _nseries, _flags)                                                                  \
    ((*(_ah)->ah_set11nRateScenario)(_ah, _ds, _lastds, _durupdate, _rtsctsrate, _rtsctsduration,\
        _series, _nseries, _flags))
#define ath_hal_set11n_aggr_first(_ah, _ds, _aggrlen) \
    ((*(_ah)->ah_set11nAggrFirst)(_ah, _ds, _aggrlen))
#define ath_hal_set11n_aggr_middle(_ah, _ds, _numdelims) \
    ((*(_ah)->ah_set11nAggrMiddle)(_ah, _ds, _numdelims))
#define ath_hal_set11n_aggr_last(_ah, _ds) \
    ((*(_ah)->ah_set11nAggrLast)(_ah, _ds))
#define ath_hal_clr11n_aggr(_ah, _ds) \
    ((*(_ah)->ah_clr11nAggr)(_ah, _ds))
#define ath_hal_set11n_rifs_burst_middle(_ah, _ds) \
    ((*(_ah)->ah_set11nRifsBurstMiddle)(_ah, _ds))
#define ath_hal_set11n_rifs_burst_last(_ah, _ds) \
    ((*(_ah)->ah_set11nRifsBurstLast)(_ah, _ds))
#define ath_hal_clr11n_rifs_burst(_ah, _ds) \
    ((*(_ah)->ah_clr11nRifsBurst)(_ah, _ds))
#define ath_hal_set11n_aggr_rifs_burst(_ah, _ds) \
    ((*(_ah)->ah_set11nAggrRifsBurst)(_ah, _ds))
#define ath_hal_set11n_burstduration(_ah, _ds, _burstduration) \
    ((*(_ah)->ah_set11nBurstDuration)(_ah, _ds, _burstduration))
#define ath_hal_set11n_virtualmorefrag(_ah, _ds, _vmf)  \
    ((*(_ah)->ah_set11nVirtualMoreFrag)(_ah, _ds, _vmf))
#define ath_hal_get11nextbusy(_ah) \
    ((*(_ah)->ah_get11nExtBusy)((_ah)))
#define ath_hal_set11nmac2040(_ah, _mode) \
    ((*(_ah)->ah_set11nMac2040)((_ah), (_mode)))
#define ath_hal_get11nRxClear(_ah) \
    ((*(_ah)->ah_get11nRxClear)((_ah)))
#define ath_hal_set11nRxClear(_ah, _rxclear) \
    ((*(_ah)->ah_set11nRxClear)((_ah), (_rxclear)))
#define ath_hal_get11n_hwplatform(_ah) \
    ((*(_ah)->ah_get11nHwPlatform)((_ah)))

#define ath_hal_ppmGetRssiDump(_ah) \
    ((*(_ah)->ah_ppmGetRssiDump)((_ah)))
#define ath_hal_ppmArmTrigger(_ah) \
    ((*(_ah)->ah_ppmArmTrigger)((_ah)))
#define ath_hal_ppmGetTrigger(_ah) \
    ((*(_ah)->ah_ppmGetTrigger)((_ah)))
#define ath_hal_ppmForce(_ah) \
    ((*(_ah)->ah_ppmForce)((_ah)))
#define ath_hal_ppmUnForce(_ah) \
    ((*(_ah)->ah_ppmUnForce)((_ah)))
#define ath_hal_ppmGetForceState(_ah) \
    ((*(_ah)->ah_ppmGetForceState)((_ah)))

#define ath_hal_getspurInfo(_ah, _en, _ln, _hi) \
    ((*(_ah)->ah_getSpurInfo)((_ah), (_en), (_ln), (_hi)))
#define ath_hal_setspurInfo(_ah, _en, _ln, _hi) \
    ((*(_ah)->ah_setSpurInfo)((_ah), (_en), (_ln), (_hi)))

#define ath_hal_getMibCycleCountsPct(_ah, _pRxc, _pRxf, _pTxf) \
    ((*(_ah)->ah_getMibCycleCountsPct)((_ah), (_pRxc), (_pRxf), (_pTxf)))
#define ath_hal_getMibCycleCounts(_ah, _pContext) \
    ((*(_ah)->ah_getMibCycleCounts)((_ah), (_pContext)))
#define ath_hal_dmaRegDump(_ah) \
    ((*(_ah)->ah_dmaRegDump)((_ah)))
#define ath_hal_updateMibMacStats(_ah) \
    ((*(_ah)->ah_updateMibMacStats)((_ah)))
#define ath_hal_getMibMacStats(_ah, _pStats) \
    ((*(_ah)->ah_getMibMacStats)((_ah), (_pStats)))
#define ath_hal_clearMibCounters(_ah) \
        ((*(_ah)->ah_clearMibCounters)((_ah)))
#define ath_hal_getCurRSSI(_ah) \
            ((*(_ah)->ah_getCurRSSI)((_ah)))

#define ath_hal_is_bb_hung(_ah) \
    ((*(_ah)->ah_detectBbHang)((_ah)))
#define ath_hal_is_mac_hung(_ah) \
    ((*(_ah)->ah_detectMacHang)((_ah)))
#define ath_hal_set_rifs(_ah, _enable) \
    ((*(_ah)->ah_set11nRxRifs)((_ah), (_enable)))
#define ath_hal_hasdynamicsmps(_ah) \
    (ath_hal_getcapability(_ah, HAL_CAP_DYNAMIC_SMPS, 0, NULL) == HAL_OK)
#define ath_hal_set_immunity(_ah, _enable) \
    ((*(_ah)->ah_immunity)((_ah), (_enable)))

#ifdef ATH_BT_COEX
#define ath_hal_hasbtcoex(_ah)                  \
    (ath_hal_getcapability((_ah), HAL_CAP_BT_COEX, 0, NULL) == HAL_OK)
#define ath_hal_bt_getinfo(_ah, _btinfo)        \
    ((*(_ah)->ah_getBTCoexInfo)((_ah), (_btinfo)))
#define ath_hal_bt_config(_ah, _btconf)         \
    ((*(_ah)->ah_btCoexConfig)((_ah), (_btconf)))
#define ath_hal_btcoex_setqcuthresh(_ah, _qnum) \
    ((*(_ah)->ah_btCoexSetQcuThresh)((_ah), (_qnum)))
#define ath_hal_bt_setweights(_ah, _bw, _wlw)   \
    ((*(_ah)->ah_btCoexSetWeights)((_ah), (_bw), (_wlw)))
#define ath_hal_btcoex_setbmissthresh(_ah, _tr) \
    ((*(_ah)->ah_btCoexSetBmissThresh)((_ah), (_tr)))
#define ath_hal_bt_disable_coex(_ah)             \
    ((*(_ah)->ah_btCoexDisable)((_ah)))
#define ath_hal_bt_enable_coex(_ah)              \
    ((*(_ah)->ah_btCoexEnable)((_ah)))
#endif /* ATH_BT_COEX */

#define ath_hal_getmfpsupport(_ah, _ri)  \
    ath_hal_getcapability(_ah, HAL_CAP_MFP, 0, (void *)(_ri))

#define ath_hal_getMacVersion(_ah) \
	((*(_ah)->ah_getMacVersion)((_ah)))
	
#define ath_hal_getAntDivCombConf(_ah, _conf) \
    ((*(_ah)->ah_getAntDviCombConf)((_ah), (_conf)))

#define ath_hal_setAntDivCombConf(_ah, _conf) \
    ((*(_ah)->ah_setAntDviCombConf)((_ah), (_conf)))
        	
#endif /* ATH_INTERNAL_H */
