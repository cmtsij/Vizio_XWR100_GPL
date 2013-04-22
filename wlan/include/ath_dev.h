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

#ifndef ATH_DEV_H
#define ATH_DEV_H

/*
 * Public Interface of ATH layer
 */

#include <osdep.h>
#include <ah.h>
#include <wbuf.h>
#include <if_athioctl.h>

#include <ieee80211.h>

/**
 * @defgroup ath_dev ath_dev - Atheros Device Module
 * @{
 * ath_dev implements the Atheros' specific low-level functions of the wireless driver.
 */

/**
 * @brief Clients of ATH layer call ath_attach to obtain a reference to an ath_dev structure.
 * Hardware-related operation that follow must call back into ATH through interface,
 * supplying the reference as the first parameter.
 */
typedef void *ath_dev_t;

/**
 * @brief Opaque handle of an associated node (including BSS itself).
 */
typedef void *ath_node_t;

/**
 * @defgroup ieee80211_if - 802.11 Protocal Interface required by Atheros Device Module
 * @{
 * @ingroup ath_dev
 */

/**
 * @brief Opaque handle of 802.11 protocal layer.
 * ATH module must call back into protocal layer through callbacks passed in attach time,
 * supplying the reference as the first parameter.
 */
typedef void *ieee80211_handle_t;

/**
 * @brief Opaque handle of network instance in 802.11 protocal layer.
 */
typedef void *ieee80211_if_t;

/**
 * @brief Opaque handle of per-destination information in 802.11 protocal layer.
 */
typedef void *ieee80211_node_t;

/**
 * @brief Wireless Mode Definition
 */
typedef enum {
    WIRELESS_MODE_11a = 0,
    WIRELESS_MODE_11b,
    WIRELESS_MODE_11g,
    WIRELESS_MODE_108a,
    WIRELESS_MODE_108g,
    WIRELESS_MODE_11NA_HT20,
    WIRELESS_MODE_11NG_HT20,
    WIRELESS_MODE_11NA_HT40PLUS,
    WIRELESS_MODE_11NA_HT40MINUS,
    WIRELESS_MODE_11NG_HT40PLUS,
    WIRELESS_MODE_11NG_HT40MINUS,
    WIRELESS_MODE_XR,
    WIRELESS_MODE_MAX
} WIRELESS_MODE;

/*
 * @breif Wireless Mode Mask
 */
typedef enum {
    MODE_SELECT_11A       = 0x00001,
    MODE_SELECT_TURBO     = 0x00002,
    MODE_SELECT_11B       = 0x00004,
    MODE_SELECT_11G       = 0x00008,
    MODE_SELECT_108G      = 0x00020,
    MODE_SELECT_108A      = 0x00040,
    MODE_SELECT_XR        = 0x00100,
    MODE_SELECT_49_27     = 0x00200,
    MODE_SELECT_49_54     = 0x00400,
    MODE_SELECT_11NG_HT20 = 0x00800,
    MODE_SELECT_11NA_HT20 = 0x01000,
    MODE_SELECT_11NG_HT40PLUS  = 0x02000,       /* 11N-G HT40+ channels */
    MODE_SELECT_11NG_HT40MINUS = 0x04000,       /* 11N-G HT40- channels */
    MODE_SELECT_11NA_HT40PLUS  = 0x08000,       /* 11N-A HT40+ channels */
    MODE_SELECT_11NA_HT40MINUS = 0x10000,       /* 11N-A HT40- channels */

    MODE_SELECT_2GHZ      = (MODE_SELECT_11B | MODE_SELECT_11G | MODE_SELECT_108G | MODE_SELECT_11NG_HT20),

    MODE_SELECT_5GHZ      = (MODE_SELECT_11A | MODE_SELECT_TURBO | MODE_SELECT_108A | MODE_SELECT_11NA_HT20),

    MODE_SELECT_ALL       = (MODE_SELECT_5GHZ | MODE_SELECT_2GHZ | MODE_SELECT_49_27 | MODE_SELECT_49_54 | MODE_SELECT_11NG_HT40PLUS | MODE_SELECT_11NG_HT40MINUS | MODE_SELECT_11NA_HT40PLUS | MODE_SELECT_11NA_HT40MINUS),

} WIRELESS_MODES_SELECT;

/*
 * Spatial Multiplexing Modes.
 */
typedef enum {
	ATH_SM_ENABLE,
	ATH_SM_PWRSAV_STATIC,
	ATH_SM_PWRSAV_DYNAMIC,
} ATH_SM_PWRSAV;

/*
 * Rate
 */
typedef enum {
    NORMAL_RATE = 0,
    HALF_RATE,
    QUARTER_RATE
} RATE_TYPE;

/**
 * @brief Protection Moide
 */
typedef enum {
    PROT_M_NONE = 0,
    PROT_M_RTSCTS,
    PROT_M_CTSONLY
} PROT_MODE;

#ifdef ATH_BT_COEX
/**
 * @brief Bluetooth stream type
 */
typedef enum {
    BT_STREAM_UNDEF = 0,
    BT_STREAM_SCO,
    BT_STREAM_A2DP,
    BT_STREAM_HID,
    BT_STREAM_MAX
} BT_STREAM_TYPE;
#endif

/**
 * @brief beacon configuration
 */
typedef struct {
    u_int16_t       beacon_interval;
    u_int16_t       listen_interval;
    u_int16_t       dtim_period;
    u_int16_t       bmiss_timeout;
    u_int8_t        dtim_count;
    u_int8_t        tim_offset;
    union {
        u_int64_t   last_tsf;
        u_int8_t    last_tstamp[8];
    } u;    /* last received beacon/probe response timestamp of this BSS. */
} ieee80211_beacon_config_t;

/**
 * @brief offsets in a beacon frame for
 * quick acess of beacon content by low-level driver
 */
typedef struct {
    u_int8_t        *bo_tim;    /* start of atim/dtim */
} ieee80211_beacon_offset_t;

/**
 * @brief per-frame tx control block
 */
typedef struct {
    ath_node_t      an;         /* destination to sent to */
    int             if_id;      /* only valid for cab traffic */
    int             qnum;       /* h/w queue number */

    u_int           ismcast:1;  /* if it's a multicast frame */
    u_int           istxfrag:1; /* if it's a tx fragment */
    u_int           ismgmt:1;   /* if it's a management frame */
    u_int           isdata:1;   /* if it's a data frame */
    u_int           isqosdata:1; /* if it's a qos data frame */
    u_int           ps:1;       /* if one or more stations are in PS mode */
    u_int           shortPreamble:1; /* use short preamble */
    u_int           ht:1;       /* if it can be transmitted using HT */
    u_int           use_minrate:1; /* if this frame should transmitted using specified
                                    * mininum rate */
    u_int           isbar:1;    /* if it is a block ack request */
    u_int           ispspoll:1; /* if it is a PS-Poll frame */
    u_int           calcairtime:1; /* requests airtime be calculated when set for tx frame */
    u_int           iseap:1; /* Is this an EAP packet? */
#ifdef ATH_SUPPORT_UAPSD
    u_int           isuapsd:1;  /* if this frame needs uapsd handling */
#endif

    int iseapol;
    HAL_PKT_TYPE    atype;      /* Atheros packet type */
    u_int           flags;      /* HAL flags */
    u_int32_t       keyix;      /* key index */
    HAL_KEY_TYPE    keytype;    /* key type */
    u_int16_t       txpower;    /* transmit power */
    u_int16_t       seqno;      /* sequence number */
    u_int16_t       tidno;      /* tid number */

    u_int16_t       frmlen;     /* frame length */
#ifdef USE_LEGACY_HAL
    u_int16_t       hdrlen;     /* header length of this frame */
    int             compression; /* compression scheme */
    u_int8_t        ivlen;      /* iv length for compression */
    u_int8_t        icvlen;     /* icv length for compression */
    u_int8_t        antenna;    /* antenna control */
#endif

    int             min_rate;   /* minimum rate */
    int             mcast_rate; /* multicast rate */
    u_int16_t       nextfraglen; /* next fragment length */

    /* below is set only by ath_dev */
    ath_dev_t       dev;        /* device handle */
    u_int8_t        priv[64];   /* private rate control info */

    OS_DMA_MEM_CONTEXT(dmacontext) /* OS specific DMA context */
} ieee80211_tx_control_t;

/*
 * @brief per frame tx status block
 */
typedef struct {
    int             retries;    /* number of retries to successufully transmit this frame */
    int             flags;      /* status of transmit */
#define ATH_TX_ERROR        0x01
#define ATH_TX_XRETRY       0x02
} ieee80211_tx_status_t;

#define ATH_MAX_ANTENNA 3

/*
 * @brief headline block removal 
 */
/*
 * HBR_TRIGGER_PER_HIGH is the per threshold to
 * trigger the headline block removal state machine
 * into PROBING state; HBR_TRIGGER_PER_LOW is the 
 * per threshold to trigger the state machine to
 * ACTIVE mode from PROBING. Since log(5/25)/log(7/8)=12,
 * which means 12 continuous QoSNull probing frames
 * been sent successfully.
 */
#define HBR_TRIGGER_PER_HIGH	25
#define HBR_TRIGGER_PER_LOW		5


/*
 * @brief per frame rx status block
 */
typedef struct {
    u_int64_t       tsf;        /* mac tsf */
    int8_t          rssi;       /* RSSI (noise floor ajusted) */
    int8_t          rssictl[ATH_MAX_ANTENNA];       /* RSSI (noise floor ajusted) */
    int8_t          rssiextn[ATH_MAX_ANTENNA];       /* RSSI (noise floor ajusted) */
    int8_t          abs_rssi;   /* absolute RSSI */
    u_int8_t        rateieee;   /* data rate received (IEEE rate code) */
    u_int8_t        ratecode;   /* phy rate code */
    u_int32_t       rateKbps;   /* data rate received (Kbps) */
    int             flags;      /* status of associated wbuf */
#define ATH_RX_FCS_ERROR        0x01
#define ATH_RX_MIC_ERROR        0x02
#define ATH_RX_DECRYPT_ERROR    0x04
#define ATH_RX_RSSI_VALID       0x08
#define ATH_RX_CHAIN_RSSI_VALID 0x10 /* if any of ctl,extn chain rssis are valid */
#define ATH_RX_RSSI_EXTN_VALID  0x20 /* if extn chain rssis are valid */
#define ATH_RX_40MHZ            0x40 /* set if 40Mhz, clear if 20Mhz */
#define ATH_RX_SHORT_GI         0x80 /* set if short GI, clear if full GI */
#define ATH_RX_SM_ENABLE        0x100
} ieee80211_rx_status_t;

typedef struct {
    int             rssi;       /* RSSI (noise floor ajusted) */
    int             rssictl[ATH_MAX_ANTENNA];       /* RSSI (noise floor ajusted) */
    int             rssiextn[ATH_MAX_ANTENNA];       /* RSSI (noise floor ajusted) */
    int             rateieee;   /* data rate xmitted (IEEE rate code) */
    u_int32_t       rateKbps;   /* data rate xmitted (Kbps) */
    int             ratecode;   /* phy rate code */
    int             flags;      /* validity flags */
#define ATH_TX_CHAIN_RSSI_VALID 0x01 /* if any of ctl,extn chain rssis are valid */
#define ATH_TX_RSSI_EXTN_VALID  0x02 /* if extn chain rssis are valid */
    u_int32_t       airtime;    /* time on air per final tx rate */
} ieee80211_tx_stat_t;

enum ieee80211_clist_cmd {
    CLIST_UPDATE,
    CLIST_DFS_UPDATE,
	CLIST_NEW_COUNTRY
};

#ifdef ATH_SUPPORT_DFS
/* DFS channel notification status */
#if 0
enum ieee80211_dfs_chan_status {
    IEEE80211_DFS_RADAR         = 0,    /* radar found on channel n */
    IEEE80211_DFS_ROAMED_TO     = 1,    /* roamed to channel n */
    IEEE80211_DFS_CAC_START     = 2,    /* cac started on channel n */
    IEEE80211_DFS_CAC_END       = 3,    /* cac complete on channel n */
};
struct ieee80211_dfs_chan_status_event {
    uint16_t    iev_freq;    /* freq radar detected on */
    uint8_t        iev_ieee;    /* chan radar detected on */
    uint8_t        iev_status;    /* dfs channel status */
};
#endif

#endif

/* VAP configuration (from protocol layer) */
struct ath_vap_config {
    u_int32_t		av_fixed_rateset;
    u_int32_t		av_fixed_retryset;
};

#ifdef ATH_CCX
struct ath_mib_cycle_cnts {
    u_int32_t   txFrameCount;
    u_int32_t   rxFrameCount;
    u_int32_t   rxClearCount;
    u_int32_t   cycleCount;
    u_int8_t    isRxActive;
    u_int8_t    isTxActive;
};

struct ath_mib_mac_stats {
    u_int32_t   ackrcv_bad;
    u_int32_t   rts_bad;
    u_int32_t   rts_good;
    u_int32_t   fcs_bad;
    u_int32_t   beacons;
};

struct ath_chan_data {
    u_int8_t    clockRate;
    u_int32_t   noiseFloor;
    u_int8_t    ccaThreshold;
};
#endif
struct ieee80211_ops {
    int         (*get_netif_settings)(ieee80211_handle_t);
#define ATH_NETIF_RUNNING       0x01
#define ATH_NETIF_PROMISCUOUS   0x02
#define ATH_NETIF_ALLMULTI      0x04
#define ATH_NETIF_NO_BEACON     0x08
    void        (*netif_mcast_merge)(ieee80211_handle_t, u_int32_t mfilt[2]);
    
    void        (*setup_channel_list)(ieee80211_handle_t, enum ieee80211_clist_cmd cmd,
                                      const HAL_CHANNEL *chans, int nchan,
                                      const u_int8_t *regclassids, u_int nregclass,
                                      int countrycode);
    void        (*update_txpow)(ieee80211_handle_t, u_int16_t txpowlimit, u_int16_t txpowlevel);
    void        (*get_beacon_config)(ieee80211_handle_t, int if_id, ieee80211_beacon_config_t *conf);
    wbuf_t      (*get_beacon)(ieee80211_handle_t, int if_id, ieee80211_beacon_offset_t *bo, ieee80211_tx_control_t *txctl);
    int         (*update_beacon)(ieee80211_handle_t, int if_id, ieee80211_beacon_offset_t *bo, wbuf_t wbuf, int mcast);
    void        (*notify_beacon_miss)(ieee80211_handle_t);
    void        (*proc_tim)(ieee80211_handle_t);
    int         (*send_bar)(ieee80211_node_t, u_int8_t tidno, u_int16_t seqno);
    void        (*notify_txq_status)(ieee80211_handle_t, u_int16_t queue_depth);
    void        (*tx_complete)(wbuf_t wbuf, ieee80211_tx_status_t *);
    void        (*tx_status)(ieee80211_node_t, ieee80211_tx_stat_t *);
    int         (*rx_indicate)(ieee80211_handle_t, wbuf_t wbuf, ieee80211_rx_status_t *status, u_int16_t keyix);
    int         (*rx_subframe)(ieee80211_node_t, wbuf_t wbuf, ieee80211_rx_status_t *status);
    HAL_HT_MACMODE (*cwm_macmode)(ieee80211_handle_t);
    /* DFS related ops */
    void        (*ath_net80211_dfs_test_return)(ieee80211_handle_t ieee, u_int8_t ieeeChan);
    void        (*ath_net80211_mark_dfs)(ieee80211_handle_t ieee, struct ieee80211_channel *ichan);       
    void        (*ath_net80211_set_vap_state)(ieee80211_handle_t ieee,u_int8_t if_id, u_int8_t state);
    void        (*ath_net80211_change_channel)(ieee80211_handle_t ieee, struct ieee80211_channel *chan);
    void        (*ath_net80211_switch_mode_static20)(ieee80211_handle_t ieee);
    void        (*ath_net80211_switch_mode_dynamic2040)(ieee80211_handle_t ieee);
	void		(*ath_net80211_set_doth)(ieee80211_handle_t ieee,int dothSetting);
    /* Rate control related ops */
    void        (*setup_rate)(ieee80211_handle_t, WIRELESS_MODE mode, RATE_TYPE type, const HAL_RATE_TABLE *rt);
    void        (*update_txrate)(ieee80211_node_t, int txrate);
    void        (*rate_newstate)(ieee80211_handle_t ieee, ieee80211_if_t if_data);
    void        (*rate_node_update)(ieee80211_handle_t ieee, ieee80211_node_t, int isnew);

    void        (*drain_amsdu)(ieee80211_handle_t);
#ifdef ATH_SUPPORT_UAPSD
    void        (*check_uapsdtrigger)(ieee80211_handle_t ieee, wbuf_t wbuf, u_int16_t keyix);
    void        (*uapsd_eospindicate)(ieee80211_node_t node, wbuf_t wbuf, int txok);
    wbuf_t      (*uapsd_allocqosnullframe)(void);
    wbuf_t      (*uapsd_getqosnullframe)(ieee80211_node_t node, wbuf_t wbuf, int ac);
#endif
#ifdef ATH_SUPPORT_IQUE
	void		(*hbr_settrigger)(ieee80211_node_t node, int state);
	int			(*hbr_getstate)(ieee80211_node_t);
#endif
        u_int8_t*        (*get_macaddr)(ieee80211_node_t node);
#ifdef NODE_FREE_DEBUG
        void             (*node_print)(ieee80211_node_t node);
        void             (*add_trace)(ieee80211_node_t node, char * funcp,
                                      char *descp, u_int64_t value);
#endif
};

/**
 * @}
 */

/**
 * @brief Capabilities of Atheros Devices
 */
typedef enum {
    ATH_CAP_FF = 0,
    ATH_CAP_BURST,
    ATH_CAP_COMPRESSION,
    ATH_CAP_TURBO,
    ATH_CAP_XR,
    ATH_CAP_TXPOWER,
    ATH_CAP_DIVERSITY,
    ATH_CAP_BSSIDMASK,
    ATH_CAP_TKIP_SPLITMIC,
    ATH_CAP_MCAST_KEYSEARCH,
    ATH_CAP_TKIP_WMEMIC,
    ATH_CAP_WMM,
    ATH_CAP_HT,
    ATH_CAP_RX_STBC,
    ATH_CAP_TX_STBC,
    ATH_CAP_4ADDR_AGGR,
    ATH_CAP_ENHANCED_PM_SUPPORT,
    ATH_CAP_MBSSID_AGGR_SUPPORT,
#ifdef ATH_SWRETRY 
    ATH_CAP_SWRETRY_SUPPORT,
#endif
#ifdef ATH_SUPPORT_UAPSD
    ATH_CAP_UAPSD,
#endif
    ATH_CAP_DYNAMIC_SMPS,
    ATH_CAP_WPS_BUTTON,
    ATH_CAP_WEP_TKIP_AGGR,
    ATH_CAP_DS,
    ATH_CAP_SINGLE_AGGR_SAFE,
} ATH_CAPABILITY_TYPE;

typedef enum {
    ATH_RX_NON_CONSUMED = 0,
    ATH_RX_CONSUMED
} ATH_RX_TYPE;

/*
** Enumeration of parameter IDs
** This is how the external users refer to specific parameters, which is
** why it's defined in the external interface
*/

typedef enum {
    ATH_PARAM_TXCHAINMASK=1,
    ATH_PARAM_RXCHAINMASK,
    ATH_PARAM_TXCHAINMASKLEGACY,
    ATH_PARAM_RXCHAINMASKLEGACY,
    ATH_PARAM_CHAINMASK_SEL,
    ATH_PARAM_AMPDU,
    ATH_PARAM_AMPDU_LIMIT,
    ATH_PARAM_AMPDU_SUBFRAMES,
    ATH_PARAM_AGGR_PROT,
    ATH_PARAM_AGGR_PROT_DUR,
    ATH_PARAM_AGGR_PROT_MAX,
    ATH_PARAM_TXPOWER_LIMIT,
    ATH_PARAM_TXPOWER_OVERRIDE,
    ATH_PARAM_PCIE_DISABLE_ASPM_WK,
    ATH_PARAM_PCID_ASPM,
    ATH_PARAM_BEACON_NORESET,
    ATH_PARAM_CAB_CONFIG,
    ATH_PARAM_ATH_DEBUG,
    ATH_PARAM_ATH_TPSCALE,
    ATH_PARAM_ACKTIMEOUT,
#ifdef ATH_RB
    ATH_PARAM_RX_RB,
    ATH_PARAM_RX_RB_DETECT,
    ATH_PARAM_RX_RB_TIMEOUT,
    ATH_PARAM_RX_RB_SKIPTHRESH,
#endif
    ATH_PARAM_AMSDU_ENABLE,
#ifdef ATH_SUPPORT_IQUE
	ATH_PARAM_RETRY_DURATION,
	ATH_PARAM_HBR_HIGHPER,
	ATH_PARAM_HBR_LOWPER,
#endif
    ATH_PARAM_RX_STBC,
    ATH_PARAM_TX_STBC,
    ATH_PARAM_LIMIT_LEGACY_FRM,
    ATH_PARAM_TOGGLE_IMMUNITY,
    ATH_PARAM_WEP_TKIP_AGGR_TX_DELIM,
    ATH_PARAM_WEP_TKIP_AGGR_RX_DELIM,
    ATH_PARAM_GPIO_LED_CUSTOM,
    ATH_PARAM_SWAP_DEFAULT_LED,
    ATH_PARAM_USE_EAP_LOWEST_RATE,
#ifdef AP_SLOW_ANT_DIV
    ATH_PARAM_SLOWANTDIV_RSSITHRHIGH,
    ATH_PARAM_SLOWANTDIV_RSSITHRLOW,
    ATH_PARAM_SLOWANTDIV_BYTESTHRRX,
    ATH_PARAM_SLOWANTDIV_DWELL_TIME,
    ATH_PARAM_SLOWANTDIV_SETTLE_TIME,
    ATH_PARAM_SLOWANTDIV_ENABLE,
#endif
}ath_param_ID_t;

#ifdef ATH_RB
typedef enum {
    ATH_RB_MODE_OFF,
    ATH_RB_MODE_DETECT,
    ATH_RB_MODE_FORCE
} ath_rb_mode_t;
#endif

#define IEEE80211_ADDR_LEN    6

/* Reset flag */
#define RESET_RETRY_TXQ     0x00000001

typedef enum {
    ATH_MFP_QOSDATA = 0,
    ATH_MFP_PASSTHRU,
    ATH_MFP_HW_CRYPTO
} ATH_MFP_OPT_t;

struct ath_ops {
    /* To query hardware capabilities */
    int         (*has_capability)(ath_dev_t, ATH_CAPABILITY_TYPE type);
    int         (*has_cipher)(ath_dev_t, HAL_CIPHER cipher);
    
    /* To initialize/de-initialize */
    int         (*open)(ath_dev_t, HAL_CHANNEL *initial_chan);
    int         (*stop)(ath_dev_t);

    /* To attach/detach virtual interface */
#define ATH_IF_ID_ANY   0xff
    int         (*add_interface)(ath_dev_t, int if_id, ieee80211_if_t if_data, HAL_OPMODE opmode, HAL_OPMODE iv_opmode, int nostabeacon);
    int         (*remove_interface)(ath_dev_t, int if_id);
    int         (*config_interface)(ath_dev_t, int if_id, struct ath_vap_config *if_config);

    /* Notification of event/state of virtual interface */
    int         (*down)(ath_dev_t, int if_id, u_int flags);
    int         (*listen)(ath_dev_t, int if_id);
    int         (*join)(ath_dev_t, int if_id, const u_int8_t bssid[IEEE80211_ADDR_LEN], u_int flags);
    int         (*up)(ath_dev_t, int if_id, const u_int8_t bssid[IEEE80211_ADDR_LEN], u_int8_t aid, u_int flags);
#define ATH_IF_HW_OFF           0x0001  /* hardware state needs to turn off */
#define ATH_IF_HW_ON            0x0002  /* hardware state needs to turn on */
#define ATH_IF_HT               0x0004  /* STA only: the associated AP is HT capable */
#define ATH_IF_PRIVACY          0x0008  /* AP/IBSS only: current BSS has privacy on */
#define ATH_IF_BEACON_ENABLE    0x0010  /* AP/IBSS only: enable beacon */
#define ATH_IF_BEACON_SYNC      0x0020  /* IBSS only: need to sync beacon */
#define ATH_IF_DTURBO           0x0040  /* STA: the associated AP is dynamic turbo capable
                                         * AP: current BSS is in turbo mode */
#define ATH_IF_VAP_IND          0x0080  /* Independent VAPs */

    /* To attach/detach per-destination info (i.e., node) */
    ath_node_t  (*alloc_node)(ath_dev_t, int if_id, ieee80211_node_t);
    void        (*free_node)(ath_dev_t, ath_node_t);
    void        (*cleanup_node)(ath_dev_t, ath_node_t);
    void        (*update_node_pwrsave)(ath_dev_t, ath_node_t,int);

    /* notify a newly associated node */
    void        (*new_assoc)(ath_dev_t, ath_node_t, int isnew, int isuapsd);

    /* interrupt processing */
    void        (*enable_interrupt)(ath_dev_t);
    void        (*disable_interrupt)(ath_dev_t);
#define ATH_ISR_NOSCHED         0x0000  /* Do not schedule bottom half/DPC				*/
#define ATH_ISR_SCHED           0x0001  /* Schedule the bottom half for execution		*/
#define ATH_ISR_NOTMINE         0x0002  /* This was not my interrupt - for shared IRQ's	*/
    int         (*isr)(ath_dev_t);
    void        (*handle_intr)(ath_dev_t);

    /* reset */
    int         (*reset_start)(ath_dev_t, u_int32_t);
    int         (*reset)(ath_dev_t);
    int         (*reset_end)(ath_dev_t, u_int32_t);
    int         (*switch_opmode)(ath_dev_t, HAL_OPMODE opmode);

    /* set channel */
    void        (*set_channel)(ath_dev_t, HAL_CHANNEL *);

    /* scan notifications */
    void        (*scan_start)(ath_dev_t);
    void        (*scan_end)(ath_dev_t);
    void        (*led_scan_start)(ath_dev_t);
    void        (*led_scan_end)(ath_dev_t);

    /* force ppm notications */
    void        (*force_ppm_notify)(ath_dev_t, int event, u_int8_t *bssid);

    /* beacon synchronization */
    void        (*sync_beacon)(ath_dev_t, int if_id);
    void        (*update_beacon_info)(ath_dev_t, int avgrssi);

    /* tx callbacks */
    int         (*tx_init)(ath_dev_t, int nbufs);
    int         (*tx_cleanup)(ath_dev_t);
    int         (*tx_get_qnum)(ath_dev_t, int qtype, int haltype);
    int         (*txq_update)(ath_dev_t, int qnum, HAL_TXQ_INFO *qi);
    int         (*tx)(ath_dev_t, wbuf_t wbuf, ieee80211_tx_control_t *txctl);
    void        (*tx_proc)(ath_dev_t);
    void        (*tx_flush)(ath_dev_t);
    u_int32_t   (*txq_depth)(ath_dev_t, int);
    u_int32_t   (*txq_aggr_depth)(ath_dev_t, int);
    u_int32_t   (*txq_aggr_nbuf)(ath_dev_t, int);
    u_int32_t   (*txq_lim)(ath_dev_t, int);

    /* rx callbacks */
    int         (*rx_init)(ath_dev_t, int nbufs);
    void        (*rx_cleanup)(ath_dev_t);
    int         (*rx_proc)(ath_dev_t, int flushing);
    void        (*rx_requeue)(ath_dev_t, wbuf_t wbuf);
    int         (*rx_proc_frame)(ath_dev_t, ath_node_t, int is_ampdu,
                                 wbuf_t wbuf, ieee80211_rx_status_t *rx_status,
                                 ATH_RX_TYPE *status);

    /* aggregation callbacks */
    int         (*check_aggr)(ath_dev_t, ath_node_t, u_int8_t tidno);
    void        (*set_ampdu_params)(ath_dev_t, ath_node_t, u_int16_t maxampdu, u_int32_t mpdudensity);
    void        (*set_weptkip_rxdelim)(ath_dev_t, ath_node_t, u_int8_t rxdelim);
    void        (*addba_request_setup)(ath_dev_t, ath_node_t, u_int8_t tidno,
                                       struct ieee80211_ba_parameterset *baparamset,
                                       u_int16_t *batimeout,
                                       struct ieee80211_ba_seqctrl *basequencectrl,
                                       u_int16_t buffersize);
    void        (*addba_response_setup)(ath_dev_t, ath_node_t an,
                                        u_int8_t tidno, u_int8_t *dialogtoken,
                                        u_int16_t *statuscode,
                                        struct ieee80211_ba_parameterset *baparamset,
                                        u_int16_t *batimeout);
    int         (*addba_request_process)(ath_dev_t, ath_node_t an,
                                         u_int8_t dialogtoken,
                                         struct ieee80211_ba_parameterset *baparamset,
                                         u_int16_t batimeout,
                                         struct ieee80211_ba_seqctrl basequencectrl);
    void        (*addba_response_process)(ath_dev_t, ath_node_t,
                                          u_int16_t statuscode,
                                          struct ieee80211_ba_parameterset *baparamset,
                                          u_int16_t batimeout);
    void        (*addba_clear)(ath_dev_t, ath_node_t);
    void        (*delba_process)(ath_dev_t, ath_node_t an,
                                 struct ieee80211_delba_parameterset *delbaparamset,
                                 u_int16_t reasoncode);
    u_int16_t   (*addba_status)(ath_dev_t, ath_node_t, u_int8_t tidno);
    void        (*aggr_teardown)(ath_dev_t, ath_node_t, u_int8_t tidno, u_int8_t initiator);
    void        (*set_addbaresponse)(ath_dev_t, ath_node_t, u_int8_t tidno, u_int16_t statuscode);
    void        (*clear_addbaresponsestatus)(ath_dev_t, ath_node_t);
    
    /* Misc runtime configuration */
    void        (*set_slottime)(ath_dev_t, int slottime);
    void        (*set_protmode)(ath_dev_t, PROT_MODE);
    void        (*set_txpowlimit)(ath_dev_t, u_int16_t txpowlimit);
    
    /* get/set MAC address, multicast list, etc. */
    void        (*get_macaddr)(ath_dev_t, u_int8_t macaddr[IEEE80211_ADDR_LEN]);
    void        (*set_macaddr)(ath_dev_t, const u_int8_t macaddr[IEEE80211_ADDR_LEN]);
    void        (*set_mcastlist)(ath_dev_t);
    void        (*mc_upload)(ath_dev_t);

    /* key cache callbacks */
    u_int16_t   (*key_alloc_2pair)(ath_dev_t);
    u_int16_t   (*key_alloc_pair)(ath_dev_t);
    u_int16_t   (*key_alloc_single)(ath_dev_t);
    void        (*key_delete)(ath_dev_t, u_int16_t keyix, int freeslot);
    int         (*key_set)(ath_dev_t, u_int16_t keyix, HAL_KEYVAL *hk,
                           const u_int8_t mac[IEEE80211_ADDR_LEN]);
    u_int       (*keycache_size)(ath_dev_t);
    
    /* PHY state */
    int         (*get_sw_phystate)(ath_dev_t);
    int         (*get_hw_phystate)(ath_dev_t);
    void        (*set_sw_phystate)(ath_dev_t, int onoff);
    int         (*radio_enable)(ath_dev_t);
    int         (*radio_disable)(ath_dev_t);
    
    /* LED control */
    void        (*led_suspend)(ath_dev_t);
    
    /* power management */
    void        (*awake)(ath_dev_t);
    void        (*netsleep)(ath_dev_t);
    void        (*fullsleep)(ath_dev_t);

    /* regdomain callbacks */
    int         (*set_country)(ath_dev_t, char *iso_name, u_int16_t);
    void         (*set_divant)(ath_dev_t, int divant_value);
    void        (*get_current_country)(ath_dev_t, HAL_COUNTRY_ENTRY *ctry);
    int         (*set_regdomain)(ath_dev_t, int regdomain);
    int         (*get_regdomain)(ath_dev_t);
    int         (*set_quiet)(ath_dev_t, u_int16_t period, u_int16_t duration,
                             u_int16_t nextStart, u_int16_t enabled);
    u_int16_t   (*find_countrycode)(ath_dev_t, char* isoName);

    /* antenna select */
    int         (*set_antenna_select)(ath_dev_t, u_int32_t antenna_select);
    u_int32_t   (*get_current_tx_antenna)(ath_dev_t);
    u_int32_t   (*get_default_antenna)(ath_dev_t);

    /* PnP */
    void        (*notify_device_removal)(ath_dev_t);
    int         (*detect_card_present)(ath_dev_t);
    
    /* convert frequency to channel number */
    u_int       (*mhz2ieee)(ath_dev_t, u_int freq, u_int flags);

    /* statistics */
    struct ath_phy_stats    *(*get_phy_stats)(ath_dev_t, WIRELESS_MODE);
    struct ath_stats        *(*get_ath_stats)(ath_dev_t);
    struct ath_11n_stats    *(*get_11n_stats)(ath_dev_t);
    void                    (*clear_stats)(ath_dev_t);
    
    /* channel width management */
    void        (*set_macmode)(ath_dev_t, HAL_HT_MACMODE);
    void        (*set_extprotspacing)(ath_dev_t, HAL_HT_EXTPROTSPACING);
    int         (*get_extbusyper)(ath_dev_t);

#ifdef ATH_SUPERG_FF
    /* fast frame */
    int         (*check_ff)(ath_dev_t, ath_node_t, int, int, u_int32_t,u_int32_t);
#endif
#ifdef ATH_SUPPORT_DFS
/* DFS ioctl support */
    int 
                (*ath_dfs_control)(ath_dev_t dev, u_int id,
                void *indata, u_int32_t insize,
                void *outdata, u_int32_t *outsize);
#endif                 
#ifdef ATH_WOW        
    /* Wake on Wireless */
    int         (*ath_get_wow_support)(ath_dev_t);
    int         (*ath_set_wow_enable)(ath_dev_t);
    int         (*ath_wow_wakeup)(ath_dev_t);
    void        (*ath_set_wow_events)(ath_dev_t, u_int32_t);
    int         (*ath_get_wow_events)(ath_dev_t);
    int         (*ath_wow_add_wakeup_pattern)(ath_dev_t, u_int8_t *, u_int8_t *, u_int32_t);
    int         (*ath_wow_remove_wakeup_pattern)(ath_dev_t, u_int8_t *, u_int8_t *);
    int         (*ath_get_wow_wakeup_reason)(ath_dev_t);
    int         (*ath_wow_matchpattern_exact)(ath_dev_t);
    void        (*ath_wow_set_duration)(ath_dev_t, u_int16_t);
#endif    

	/* Configuration Interface */
	int			(*ath_get_config_param)(ath_dev_t dev, ath_param_ID_t ID, void *buff);
	int			(*ath_set_config_param)(ath_dev_t dev, ath_param_ID_t ID, void *buff);

	/* Noise floor func */
	short       (*get_noisefloor)(ath_dev_t dev, unsigned short	freq,  unsigned int chan_flags);

    void	(*ath_sm_pwrsave_update)(ath_dev_t, ath_node_t,
		    ATH_SM_PWRSAV mode, int);

    /* Rate control */
    int     (*ath_rate_newassoc)(ath_dev_t dev, ath_node_t , int isnew, unsigned int capflag,
                                  struct ieee80211_rateset *negotiated_rates,
                                  struct ieee80211_rateset *negotiated_htrates);
    void    (*ath_rate_newstate)(ath_dev_t dev, int if_id, int up);

#ifdef DBG
    /* Debug feature: register access */
    u_int32_t (*ath_register_read)(ath_dev_t, u_int32_t);
    void      (*ath_register_write)(ath_dev_t, u_int32_t, u_int32_t);
#endif

    /* TX Power Limit */
    void        (*ath_set_txPwrLimit)(ath_dev_t dev, u_int32_t limit, u_int16_t tpcInDb);
    u_int8_t    (*get_common_power)(u_int16_t freq);
#ifdef ATH_CCX
    /* MIB Control */
    int         (*ath_update_mibMacstats)(ath_dev_t dev);
    int         (*ath_get_mibMacstats)(ath_dev_t dev, struct ath_mib_mac_stats *pStats);
    u_int8_t    (*rcRateValueToPer)(ath_dev_t, struct ath_node *, int);
    int         (*ath_get_mibCycleCounts)(ath_dev_t dev, struct ath_mib_cycle_cnts *pCnts);
    void        (*ath_clear_mibCounters)(ath_dev_t dev);

    /* TSF 32/64 */
    u_int32_t   (*ath_get_tsf32)(ath_dev_t dev);
    u_int64_t   (*ath_get_tsf64)(ath_dev_t dev);

    /* Rx Filter */
    void        (*ath_set_rxfilter)(ath_dev_t dev);

    /* Get Serial Number */
    int         (*ath_get_sernum)(ath_dev_t dev, u_int8_t *pSn);

    /* Get Channel Data */
    int         (*ath_get_chandata)(ath_dev_t dev, struct ieee80211_channel *pChan, struct ath_chan_data *pData);

    /* Get Current RSSI */
    u_int32_t   (*ath_get_curRSSI)(ath_dev_t dev);
#endif
    /* A-MSDU frame */
    int     (*get_amsdusupported)(ath_dev_t, ath_node_t, int);

#ifdef ATH_SWRETRY
    void        (*set_swretrystate)(ath_dev_t dev, ath_node_t node, int flag);
#endif
#ifdef ATH_SUPPORT_UAPSD
    /* UAPSD */
    int         (*process_uapsd_trigger)(ath_dev_t dev, ath_node_t node, u_int8_t maxsp,
                                         u_int8_t ac, u_int8_t flush);
    u_int32_t   (*uapsd_depth)(ath_node_t node);
#endif
#ifndef REMOVE_PKT_LOG
    int     (*pktlog_start)(ath_dev_t, int log_state);
    int     (*pktlog_read_hdr)(ath_dev_t, void *buf, u_int32_t buf_len,
                               u_int32_t *required_len,
                               u_int32_t *actual_len);
    int     (*pktlog_read_buf)(ath_dev_t, void *buf, u_int32_t buf_len,
                               u_int32_t *required_len,
                               u_int32_t *actual_len);
#endif
#ifdef ATH_SUPPORT_IQUE
    /* Set and Get AC and Rate Control parameters */
    void        (*ath_set_acparams)(ath_dev_t, u_int8_t ac, u_int8_t use_rts,
                                    u_int8_t aggrsize_scaling, u_int32_t min_kbps);

    void        (*ath_set_rtparams)(ath_dev_t, u_int8_t rt_index, u_int8_t perThresh,
                                    u_int8_t probeInterval);
    void        (*ath_get_iqueconfig)(ath_dev_t);
	void		(*ath_set_hbrparams)(ath_dev_t, u_int8_t ac, u_int8_t enable, u_int8_t per);
#endif

    void        (*ath_printreg)(ath_dev_t dev, u_int32_t printctrl);
    /* Get MFP support level */
    u_int32_t   (*ath_get_mfpsupport)(ath_dev_t dev);
#ifdef ATH_SUPPORT_DFS
    int     	(*check_dfs_wait)(ath_dev_t dev);
#endif
};

/* Load-time configuration for ATH device */
struct ath_reg_parm {
    u_int16_t            busConfig;                       /* PCI target retry and TRDY timeout */
    u_int8_t             networkAddress[6];               /* Number of map registers for DMA mapping */
    u_int16_t            calibrationTime;                 /* how often to do calibrations, in seconds (0=off) */
    u_int8_t             gpioPinFuncs[NUM_GPIO_FUNCS];    /* GPIO pin for each associated function */
    u_int8_t             gpioActHiFuncs[NUM_GPIO_FUNCS];  /* Set gpioPinFunc0 active high */
    u_int8_t             WiFiLEDEnable;                   /* Toggle WIFI recommended LED mode controlled by GPIO */
    u_int8_t             softLEDEnable;                   /* Enable AR5213 software LED control to work in WIFI LED mode */
    u_int8_t             swapDefaultLED;                  /* Enable default LED swap */
    u_int16_t            linkLedFunc;                     /* Led registry entry for setting the Link Led operation */
    u_int16_t            activityLedFunc;                 /* Led registry entry for setting the Activity Led operation */
    u_int16_t            connectionLedFunc;               /* Led registry entry for setting the Connection Led operation */
    u_int8_t             gpioLedCustom;                   /* Defines customer-specific blinking requirements (OWL) */
    u_int8_t             linkLEDDefaultOn;                /* State of link LED while not connected */
    u_int8_t             DisableLED01;                    /* LED_0 or LED_1 in PCICFG register is used for other purposes */
    u_int16_t            diversityControl;                /* Enable/disable antenna diversity */
    u_int32_t            hwTxRetries;                     /* num of times hw retries the frame */
    u_int16_t            tpScale;                         /* Scaling factor to be applied to max transmit power */
    u_int8_t             extendedChanMode;                /* Extended Channel Mode flag */
    u_int16_t            overRideTxPower;                 /* Override of transmit power */
    u_int8_t             enableFCC3;                      /* TRUE only if card has been FCC3-certified */
    u_int32_t            DmaStopWaitTime;                 /* Overrides default dma wait time */
    u_int8_t             pciDetectEnable;                 /* Chipenabled. */
    u_int8_t             singleWriteKC;                   /* write key cache one word at time */
    u_int32_t            smbiosEnable;                    /* Bit 1 - enable device ID check, Bit 2 - enable smbios check */
    u_int8_t             userCoverageClass;               /* User defined coverage class */
    u_int16_t            pciCacheLineSize;                /* User cache line size setting */
    u_int16_t            pciCmdRegister;                  /* User command register setting */
    u_int32_t            regdmn;                          /* Regulatory domain code for private test */
    char                 countryName[4];                  /* country name */
    u_int32_t            wModeSelect;                     /* Software limiting of device capability for SKU reduction */
    u_int32_t            NetBand;                         /* User's choice of a, b, turbo, g, etc. from registry */
    u_int8_t             pcieDisableAspmOnRfWake;         /* Only use ASPM when RF Silenced */
    u_int8_t             pcieAspm;                        /* ASPM bit settings */
    u_int8_t             txAggrEnable;                    /* Enable Tx aggregation */
    u_int8_t             rxAggrEnable;                    /* Enable Rx aggregation */
    u_int8_t             txAmsduEnable;                   /* Enable Tx AMSDU */
    int                  aggrLimit;                       /* A-MPDU length limit */
    int                  aggrSubframes;                   /* A-MPDU subframe limit */
    u_int8_t             aggrProtEnable;                  /* Enable aggregation protection */
    u_int32_t            aggrProtDuration;                /* Aggregation protection duration */
    u_int32_t            aggrProtMax;                     /* Aggregation protection threshold */
    u_int8_t             txRifsEnable;                    /* Enable Tx RIFS */
    int                  rifsAggrDiv;                     /* RIFS aggregation divisor */
#ifdef ATH_RB
    int                  rxRifsEnable;                    /* Enable Rx RIFS */
#define ATH_RB_DEF_TIMEOUT      7000
#define ATH_RB_DEF_SKIP_THRESH  5
    u_int16_t            rxRifsTimeout;                   /* Rx RIFS timeout */
    u_int8_t             rxRifsSkipThresh;                /* Rx RIFS skip thresh */
#endif
    u_int8_t             txChainMask;                     /* Tx ChainMask */
    u_int8_t             rxChainMask;                     /* Rx ChainMask */
    u_int8_t             txChainMaskLegacy;               /* Tx ChainMask in legacy mode */
    u_int8_t             rxChainMaskLegacy;               /* Rx ChainMask in legacy mode */
    u_int8_t             rxChainDetect;                   /* Rx chain detection for Lenovo */
    u_int8_t             rxChainDetectRef;                /* Rx chain detection reference RSSI */
    u_int8_t             rxChainDetectThreshA;            /* Rx chain detection RSSI threshold in 5GHz */
    u_int8_t             rxChainDetectThreshG;            /* Rx chain detection RSSI threshold in 2GHz */
    u_int8_t             rxChainDetectDeltaA;             /* Rx chain detection RSSI delta in 5GHz */
    u_int8_t             rxChainDetectDeltaG;             /* Rx chain detection RSSI delta in 2GHz */
#ifdef ATH_WOW    
    u_int8_t             wowEnable;
#endif    
    u_int32_t            shMemAllocRetry;                 /* Shared memory allocation no of retries */
    u_int8_t             forcePpmEnable;                  /* Force PPM */
    u_int16_t            forcePpmUpdateTimeout;           /* Force PPM window update interval in ms. */
    u_int8_t             enable2GHzHt40Cap;               /* Enable HT40 in 2GHz channels */
    u_int8_t             swBeaconProcess;                 /* Process received beacons in SW (vs HW) */
#ifdef ATH_RFKILL
    u_int8_t             disableRFKill;                   /* Disable RFKill */
#endif
    u_int8_t             rfKillDelayDetect;               /* Enable WAR for slow rise for RfKill on power resume */
#ifdef ATH_SWRETRY
    u_int8_t             numSwRetries;                    /* Num of sw retries to be done */
#endif
    u_int8_t             slowAntDivEnable;                /* Enable slow antenna diversity */
    int8_t               slowAntDivThreshold;             /* Rssi threshold for slow antenna diversity trigger */
    u_int32_t            slowAntDivMinDwellTime;          /* Minimum dwell time on the best antenna configuration */
    u_int32_t            slowAntDivSettleTime;            /* Time spent on probing antenna configurations */
    u_int8_t             stbcEnable;                      /* Enable STBC */
#ifdef ATH_BT_COEX
    u_int8_t             btCoexEnable;                    /* Enable btCoex */
#endif
    u_int8_t             cwmEnable;                       /* Enable Channel Width Management (CWM) */
    u_int8_t             wpsButtonGpio;                   /* GPIO associated with Push Button */
};

/**
 * @brief Create and attach an ath_dev object
 */
int ath_dev_attach(u_int16_t devid, void *base_addr,
                   ieee80211_handle_t ieee, struct ieee80211_ops *ieee_ops,
                   osdev_t osdev,
                   ath_dev_t *dev, struct ath_ops **ops,
                   struct ath_reg_parm *ath_conf_parm,
                   struct hal_reg_parm *hal_conf_parm);

/**
 * @brief Free an ath_dev object
 */
void ath_dev_free(ath_dev_t);

/**
 * @}
 */

#endif /* ATH_DEV_H */
