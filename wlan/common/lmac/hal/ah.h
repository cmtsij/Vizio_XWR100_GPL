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

#ifndef _ATH_AH_H_
#define _ATH_AH_H_
/*
 * Atheros Hardware Access Layer
 *
 * Clients of the HAL call ath_hal_attach to obtain a reference to an ath_hal
 * structure for use with the device.  Hardware-related operations that
 * follow must call back into the HAL through interface, supplying the
 * reference as the first parameter.
 */
#include "ah_osdep.h"
/*
 * __ahdecl is analogous to _cdecl; it defines the calling
 * convention used within the HAL.  For most systems this
 * can just default to be empty and the compiler will (should)
 * use _cdecl.  For systems where _cdecl is not compatible this
 * must be defined.  See linux/ah_osdep.h for an example.
 */
#ifndef __ahdecl
#define __ahdecl
#endif

/*
 * Status codes that may be returned by the HAL.  Note that
 * interfaces that return a status code set it only when an
 * error occurs--i.e. you cannot check it for success.
 */
typedef enum {
    HAL_OK          = 0,    /* No error */
    HAL_ENXIO       = 1,    /* No hardware present */
    HAL_ENOMEM      = 2,    /* Memory allocation failed */
    HAL_EIO         = 3,    /* Hardware didn't respond as expected */
    HAL_EEMAGIC     = 4,    /* EEPROM magic number invalid */
    HAL_EEVERSION   = 5,    /* EEPROM version invalid */
    HAL_EELOCKED    = 6,    /* EEPROM unreadable */
    HAL_EEBADSUM    = 7,    /* EEPROM checksum invalid */
    HAL_EEREAD      = 8,    /* EEPROM read problem */
    HAL_EEBADMAC    = 9,    /* EEPROM mac address invalid */
    HAL_EESIZE      = 10,   /* EEPROM size not supported */
    HAL_EEWRITE     = 11,   /* Attempt to change write-locked EEPROM */
    HAL_EINVAL      = 12,   /* Invalid parameter to function */
    HAL_ENOTSUPP    = 13,   /* Hardware revision not supported */
    HAL_ESELFTEST   = 14,   /* Hardware self-test failed */
    HAL_EINPROGRESS = 15,   /* Operation incomplete */
} HAL_STATUS;

typedef enum {
    AH_FALSE = 0,       /* NB: lots of code assumes false is zero */
    AH_TRUE  = 1,
} HAL_BOOL;

typedef enum {
    HAL_CAP_REG_DMN           = 0,    /* current regulatory domain */
    HAL_CAP_CIPHER            = 1,    /* hardware supports cipher */
    HAL_CAP_TKIP_MIC          = 2,    /* handle TKIP MIC in hardware */
    HAL_CAP_TKIP_SPLIT        = 3,    /* hardware TKIP uses split keys */
    HAL_CAP_PHYCOUNTERS       = 4,    /* hardware PHY error counters */
    HAL_CAP_DIVERSITY         = 5,    /* hardware supports fast diversity */
    HAL_CAP_KEYCACHE_SIZE     = 6,    /* number of entries in key cache */
    HAL_CAP_NUM_TXQUEUES      = 7,    /* number of hardware xmit queues */
    HAL_CAP_VEOL              = 9,    /* hardware supports virtual EOL */
    HAL_CAP_PSPOLL            = 10,   /* hardware has working PS-Poll support */
    HAL_CAP_DIAG              = 11,   /* hardware diagnostic support */
    HAL_CAP_COMPRESSION       = 12,   /* hardware supports compression */
    HAL_CAP_BURST             = 13,   /* hardware supports packet bursting */
    HAL_CAP_FASTFRAME         = 14,   /* hardware supoprts fast frames */
    HAL_CAP_TXPOW             = 15,   /* global tx power limit  */
    HAL_CAP_TPC               = 16,   /* per-packet tx power control  */
    HAL_CAP_PHYDIAG           = 17,   /* hardware phy error diagnostic */
    HAL_CAP_BSSIDMASK         = 18,   /* hardware supports bssid mask */
    HAL_CAP_MCAST_KEYSRCH     = 19,   /* hardware has multicast key search */
    HAL_CAP_TSF_ADJUST        = 20,   /* hardware has beacon tsf adjust */
    HAL_CAP_XR                = 21,   /* hardware has XR support  */
    HAL_CAP_WME_TKIPMIC       = 22,   /* hardware can support TKIP MIC when WMM is turned on */
    HAL_CAP_CHAN_HALFRATE     = 23,   /* hardware can support half rate channels */
    HAL_CAP_CHAN_QUARTERRATE  = 24,   /* hardware can support quarter rate channels */
    HAL_CAP_RFSILENT          = 25,   /* hardware has rfsilent support  */
    HAL_CAP_TPC_ACK           = 26,   /* ack txpower with per-packet tpc */
    HAL_CAP_TPC_CTS           = 27,   /* cts txpower with per-packet tpc */
    HAL_CAP_11D               = 28,   /* 11d beacon support for changing cc */
    HAL_CAP_PCIE_PS           = 29,   /* pci express power save */
    HAL_CAP_HT                = 30,   /* hardware can support HT */
    HAL_CAP_GTT               = 31,   /* hardware supports global transmit timeout */
    HAL_CAP_FAST_CC           = 32,   /* hardware supports global transmit timeout */
    HAL_CAP_TX_CHAINMASK      = 33,   /* number of tx chains */
    HAL_CAP_RX_CHAINMASK      = 34,   /* number of rx chains */
    HAL_CAP_TX_TRIG_LEVEL_MAX = 35,   /* maximum Tx trigger level */
    HAL_CAP_NUM_GPIO_PINS     = 36,   /* number of GPIO pins */
    HAL_CAP_WOW               = 37,   /* WOW support */       
    HAL_CAP_CST               = 38,   /* hardware supports carrier sense timeout interrupt */
    HAL_CAP_RIFS_RX           = 39,   /* hardware supports RIFS receive */
    HAL_CAP_RIFS_TX           = 40,   /* hardware supports RIFS transmit */
    HAL_CAP_FORCE_PPM         = 41,   /* Force PPM */
    HAL_CAP_RTS_AGGR_LIMIT    = 42,   /* aggregation limit with RTS */
    HAL_CAP_4ADDR_AGGR        = 43,   /* hardware is capable of 4addr aggregation */
    HAL_CAP_DFS_DMN           = 44,   /* DFS domain */
    HAL_CAP_EXT_CHAN_DFS      = 45,   /* DFS support for extension channel */
    HAL_CAP_COMBINED_RADAR_RSSI = 46, /* Is combined RSSI for radar accurate */
    HAL_CAP_ENHANCED_PM_SUPPORT = 47, /* hardware supports enhanced power management */
    HAL_CAP_AUTO_SLEEP          = 48, /* hardware can go to network sleep automatically after waking up to receive TIM */
    HAL_CAP_MBSSID_AGGR_SUPPORT = 49, /* Support for mBSSID Aggregation */
    HAL_CAP_4KB_SPLIT_TRANS   = 50,   /* hardware is capable of splitting PCIe transanctions on 4KB boundaries */
    HAL_CAP_REG_FLAG          = 51,   /* Regulatory domain flags */
    HAL_CAP_BB_RIFS_HANG      = 52,   /* BB may hang due to RIFS */
    HAL_CAP_RIFS_RX_ENABLED   = 53,   /* RIFS RX currently enabled */
    HAL_CAP_BB_DFS_HANG       = 54,   /* BB may hang due to DFS */
    HAL_CAP_WOW_MATCH_EXACT   = 55,   /* WoW exact match pattern support */
    HAL_CAP_ANT_CFG_2GHZ      = 56,   /* Number of antenna configurations */
    HAL_CAP_ANT_CFG_5GHZ      = 57,   /* Number of antenna configurations */
    HAL_CAP_RX_STBC           = 58,
    HAL_CAP_TX_STBC           = 59,
    HAL_CAP_BT_COEX           = 60,   /* hardware is capable of bluetooth coexistence */
    HAL_CAP_DYNAMIC_SMPS      = 61,   /* Dynamic MIMO Power Save hardware support */
    HAL_CAP_WOW_MATCH_DWORD   = 62,   /* Wow pattern match first dword */
    HAL_CAP_WPS_PUSH_BUTTON   = 63,   /* hardware has WPS push button */
    HAL_CAP_WEP_TKIP_AGGR     = 64,   /* hw is capable of aggregation with WEP/TKIP */
    HAL_CAP_WEP_TKIP_AGGR_TX_DELIM = 65,   /* number of delimiters required by hw when transmitting aggregates with WEP/TKIP */
    HAL_CAP_WEP_TKIP_AGGR_RX_DELIM = 66,   /* number of delimiters required by hw when receiving aggregates with WEP/TKIP */
    HAL_CAP_DS                = 67,   /* hardware support double stream: HB93 1x2 only support single stream */
    HAL_CAP_BB_RX_CLEAR_STUCK_HANG  = 68, /* BB may hang due to Rx Clear Stuck Low */
    HAL_CAP_MAC_HANG          = 69, /* MAC may hang */
    HAL_CAP_MFP               = 70,   /* Management Frame Proctection in hardware */
    HAL_CAP_DEVICE_TYPE       = 71,   /* Type of device */
    HAL_INTR_MITIGATION_SUPPORTED = 72, /* interrupt mitigation */
    HAL_CAP_MAX_TKIP_WEP_HT_RATE = 73, /* max HT TKIP/WEP rate HW supports */
    HAL_CAP_NUM_MR_RETRIES    = 74,   /* limit on multirate retries */
    HAL_CAP_GEN_TIMER         = 75,   /* Generic(TSF) timer */
    HAL_CAP_OL_PWRCTRL        = 76,   /* open-loop power control */
    HAL_CAP_HIGH_PWR_ANT      = 77,
    HAL_CAP_MISC_FLAGS        = 78,
    HAL_CAP_SINGLE_AGGR_SAFE  = 79,   /* hw is capable of supporting aggregates with single sub-frame */
    HAL_CAP_ANT_DIV_COMB      = 80,   /* Enable antenna diversity/combining */
} HAL_CAPABILITY_TYPE;

#define HAL_CAP_MISC_FLAGS_HEAVY_CLIP   0x1
/* 
 * "States" for setting the LED.  These correspond to
 * the possible 802.11 operational states and there may
 * be a many-to-one mapping between these states and the
 * actual hardware states for the LED's (i.e. the hardware
 * may have fewer states).
 *
 * State HAL_LED_SCAN corresponds to external scan, and it's used inside
 * the LED module only.
 */
typedef enum {
    HAL_LED_RESET   = 0,
    HAL_LED_INIT    = 1,
    HAL_LED_READY   = 2,
    HAL_LED_SCAN    = 3,
    HAL_LED_AUTH    = 4,
    HAL_LED_ASSOC   = 5,
    HAL_LED_RUN     = 6
} HAL_LED_STATE;

/*
 * Transmit queue types/numbers.  These are used to tag
 * each transmit queue in the hardware and to identify a set
 * of transmit queues for operations such as start/stop dma.
 */
typedef enum {
    HAL_TX_QUEUE_INACTIVE   = 0,        /* queue is inactive/unused */
    HAL_TX_QUEUE_DATA       = 1,        /* data xmit q's */
    HAL_TX_QUEUE_BEACON     = 2,        /* beacon xmit q */
    HAL_TX_QUEUE_CAB        = 3,        /* "crap after beacon" xmit q */
    HAL_TX_QUEUE_UAPSD      = 4,        /* u-apsd power save xmit q */
    HAL_TX_QUEUE_PSPOLL     = 5,        /* power-save poll xmit q */
} HAL_TX_QUEUE;

#define    HAL_NUM_TX_QUEUES    10        /* max possible # of queues */

/*
 * Transmit queue subtype.  These map directly to
 * WME Access Categories (except for UPSD).  Refer
 * to Table 5 of the WME spec.
 */
typedef enum {
    HAL_WME_AC_BK   = 0,            /* background access category */
    HAL_WME_AC_BE   = 1,            /* best effort access category*/
    HAL_WME_AC_VI   = 2,            /* video access category */
    HAL_WME_AC_VO   = 3,            /* voice access category */
    HAL_WME_UPSD    = 4,            /* uplink power save */
    HAL_XR_DATA     = 5,            /* uplink power save */
} HAL_TX_QUEUE_SUBTYPE;

/*
 * Transmit queue flags that control various
 * operational parameters.
 */
typedef enum {
    TXQ_FLAG_TXOKINT_ENABLE            = 0x0001, /* enable TXOK interrupt */
    TXQ_FLAG_TXERRINT_ENABLE           = 0x0001, /* enable TXERR interrupt */
    TXQ_FLAG_TXDESCINT_ENABLE          = 0x0002, /* enable TXDESC interrupt */
    TXQ_FLAG_TXEOLINT_ENABLE           = 0x0004, /* enable TXEOL interrupt */
    TXQ_FLAG_TXURNINT_ENABLE           = 0x0008, /* enable TXURN interrupt */
    TXQ_FLAG_BACKOFF_DISABLE           = 0x0010, /* disable Post Backoff  */
    TXQ_FLAG_COMPRESSION_ENABLE        = 0x0020, /* compression enabled */
    TXQ_FLAG_RDYTIME_EXP_POLICY_ENABLE = 0x0040, /* enable ready time
                                                    expiry policy */
    TXQ_FLAG_FRAG_BURST_BACKOFF_ENABLE = 0x0080, /* enable backoff while
                                                    sending fragment burst*/
} HAL_TX_QUEUE_FLAGS;

typedef struct {
    u_int32_t               tqi_ver;        /* hal TXQ version */
    HAL_TX_QUEUE_SUBTYPE    tqi_subtype;    /* subtype if applicable */
    HAL_TX_QUEUE_FLAGS      tqi_qflags;     /* flags (see above) */
    u_int32_t               tqi_priority;   /* (not used) */
    u_int32_t               tqi_aifs;       /* aifs */
    u_int32_t               tqi_cwmin;      /* cwMin */
    u_int32_t               tqi_cwmax;      /* cwMax */
    u_int16_t               tqi_shretry;    /* rts retry limit */
    u_int16_t               tqi_lgretry;    /* long retry limit (not used)*/
    u_int32_t               tqi_cbrPeriod;
    u_int32_t               tqi_cbrOverflowLimit;
    u_int32_t               tqi_burstTime;
    u_int32_t               tqi_readyTime; /* specified in msecs */
    u_int32_t               tqi_compBuf;    /* compression buffer phys addr */
} HAL_TXQ_INFO;

#if ATH_WOW
/*
 * Pattern Types.
 */
#define AH_WOW_USER_PATTERN_EN      0x1
#define AH_WOW_MAGIC_PATTERN_EN     0x2
#define AH_WOW_LINK_CHANGE          0x4
#define AH_WOW_BEACON_MISS          0x8
#define AH_WOW_MAX_EVENTS           4

#endif

#define HAL_TQI_NONVAL 0xffff

/* token to use for aifs, cwmin, cwmax */
#define    HAL_TXQ_USEDEFAULT    ((u_int32_t) -1)

/* compression definitions */
#define HAL_COMP_BUF_MAX_SIZE           9216            /* 9K */
#define HAL_COMP_BUF_ALIGN_SIZE         512
#define HAL_DECOMP_MASK_SIZE            128

/* ReadyTime % lo and hi bounds */
#define HAL_READY_TIME_LO_BOUND         50
#define HAL_READY_TIME_HI_BOUND         96  /* considering the swba and dma 
                                               response times for cabQ */

/*
 * Transmit packet types.  This belongs in ah_desc.h, but
 * is here so we can give a proper type to various parameters
 * (and not require everyone include the file).
 *
 * NB: These values are intentionally assigned for
 *     direct use when setting up h/w descriptors.
 */
typedef enum {
    HAL_PKT_TYPE_NORMAL     = 0,
    HAL_PKT_TYPE_ATIM       = 1,
    HAL_PKT_TYPE_PSPOLL     = 2,
    HAL_PKT_TYPE_BEACON     = 3,
    HAL_PKT_TYPE_PROBE_RESP = 4,
    HAL_PKT_TYPE_CHIRP      = 5,
    HAL_PKT_TYPE_GRP_POLL   = 6,
} HAL_PKT_TYPE;

/* Rx Filter Frame Types */
typedef enum {
    HAL_RX_FILTER_UCAST     = 0x00000001,   /* Allow unicast frames */
    HAL_RX_FILTER_MCAST     = 0x00000002,   /* Allow multicast frames */
    HAL_RX_FILTER_BCAST     = 0x00000004,   /* Allow broadcast frames */
    HAL_RX_FILTER_CONTROL   = 0x00000008,   /* Allow control frames */
    HAL_RX_FILTER_BEACON    = 0x00000010,   /* Allow beacon frames */
    HAL_RX_FILTER_PROM      = 0x00000020,   /* Promiscuous mode */
    HAL_RX_FILTER_XRPOLL    = 0x00000040,   /* Allow XR poll frame */
    HAL_RX_FILTER_PROBEREQ  = 0x00000080,   /* Allow probe request frames */
    HAL_RX_FILTER_PHYERR    = 0x00000100,   /* Allow phy errors */
    HAL_RX_FILTER_MYBEACON  = 0x00000200,   /* Filter beacons other than mine */
    HAL_RX_FILTER_PHYRADAR  = 0x00002000,   /* Allow phy radar errors*/
    HAL_RX_FILTER_PSPOLL    = 0x00004000,   /* Allow PSPOLL frames */
        /* 
        ** PHY "Pseudo bits" should be in the upper 16 bits since the lower
        ** 16 bits actually correspond to register 0x803c bits
        */
} HAL_RX_FILTER;

typedef enum {
    HAL_PM_AWAKE            = 0,
    HAL_PM_FULL_SLEEP       = 1,
    HAL_PM_NETWORK_SLEEP    = 2,
    HAL_PM_UNDEFINED        = 3
} HAL_POWER_MODE;

typedef enum {
    HAL_SMPS_DEFAULT = 0,
    HAL_SMPS_SW_CTRL_LOW_PWR,     /* Software control, low power setting */
    HAL_SMPS_SW_CTRL_HIGH_PWR,    /* Software control, high power setting */
    HAL_SMPS_HW_CTRL              /* Hardware Control */
} HAL_SMPS_MODE;

/*
 * NOTE WELL:
 * These are mapped to take advantage of the common locations for many of
 * the bits on all of the currently supported MAC chips. This is to make
 * the ISR as efficient as possible, while still abstracting HW differences.
 * When new hardware breaks this commonality this enumerated type, as well
 * as the HAL functions using it, must be modified. All values are directly
 * mapped unless commented otherwise.
 */
typedef enum {
    HAL_INT_RX        = 0x00000001,   /* Non-common mapping */
    HAL_INT_RXDESC    = 0x00000002,
    HAL_INT_RXERR     = 0x00000004,
    HAL_INT_RXNOFRM   = 0x00000008,
    HAL_INT_RXEOL     = 0x00000010,
    HAL_INT_RXORN     = 0x00000020,
    HAL_INT_TX        = 0x00000040,   /* Non-common mapping */
    HAL_INT_TXDESC    = 0x00000080,
    HAL_INT_TIM_TIMER = 0x00000100,
    HAL_INT_TXURN     = 0x00000800,
    HAL_INT_MIB       = 0x00001000,
    HAL_INT_RXPHY     = 0x00004000,
    HAL_INT_RXKCM     = 0x00008000,
    HAL_INT_SWBA      = 0x00010000,
    HAL_INT_BMISS     = 0x00040000,
    HAL_INT_BNR       = 0x00100000,   /* Non-common mapping */
    HAL_INT_TIM       = 0x00200000,   /* Non-common mapping */
    HAL_INT_DTIM      = 0x00400000,   /* Non-common mapping */
    HAL_INT_DTIMSYNC  = 0x00800000,   /* Non-common mapping */
    HAL_INT_GPIO      = 0x01000000,
    HAL_INT_CABEND    = 0x02000000,   /* Non-common mapping */
    HAL_INT_TSFOOR    = 0x04000000,   /* Non-common mapping */
    HAL_INT_GENTIMER  = 0x08000000,   /* Non-common mapping */
    HAL_INT_CST       = 0x10000000,   /* Non-common mapping */
    HAL_INT_GTT       = 0x20000000,   /* Non-common mapping */
    HAL_INT_FATAL     = 0x40000000,   /* Non-common mapping */
    HAL_INT_GLOBAL    = 0x80000000,   /* Set/clear IER */
    HAL_INT_BMISC     = HAL_INT_TIM
            | HAL_INT_DTIM
            | HAL_INT_DTIMSYNC
            | HAL_INT_CABEND,

    /* Interrupt bits that map directly to ISR/IMR bits */
    HAL_INT_COMMON    = HAL_INT_RXNOFRM
            | HAL_INT_RXDESC
            | HAL_INT_RXERR
            | HAL_INT_RXEOL
            | HAL_INT_RXORN
            | HAL_INT_TXURN
            | HAL_INT_TXDESC
            | HAL_INT_MIB
            | HAL_INT_RXPHY
            | HAL_INT_RXKCM
            | HAL_INT_SWBA
            | HAL_INT_BMISS
            | HAL_INT_GPIO,
    HAL_INT_NOCARD   = 0xffffffff    /* To signal the card was removed */
} HAL_INT;

/* For interrupt mitigation registers */
typedef enum {
    HAL_INT_RX_FIRSTPKT=0,
    HAL_INT_RX_LASTPKT,
    HAL_INT_TX_FIRSTPKT,
    HAL_INT_TX_LASTPKT,
    HAL_INT_THRESHOLD
} HAL_INT_MITIGATION;

typedef enum {
    HAL_RFGAIN_INACTIVE         = 0,
    HAL_RFGAIN_READ_REQUESTED   = 1,
    HAL_RFGAIN_NEED_CHANGE      = 2
} HAL_RFGAIN;

/*
 * Channels are specified by frequency.
 */
typedef struct {
    u_int16_t   channel;        /* setting in Mhz */
    u_int32_t   channelFlags;   /* see below */
    u_int8_t    privFlags;
    int8_t      maxRegTxPower;  /* max regulatory tx power in dBm */
    int8_t      maxTxPower;     /* max true tx power in 0.5 dBm */
    int8_t      minTxPower;     /* min true tx power in 0.5 dBm */
} HAL_CHANNEL;

/* channelFlags */
#define CHANNEL_CW_INT    0x00002 /* CW interference detected on channel */
#define CHANNEL_TURBO     0x00010 /* Turbo Channel */
#define CHANNEL_CCK       0x00020 /* CCK channel */
#define CHANNEL_OFDM      0x00040 /* OFDM channel */
#define CHANNEL_2GHZ      0x00080 /* 2 GHz spectrum channel. */
#define CHANNEL_5GHZ      0x00100 /* 5 GHz spectrum channel */
#define CHANNEL_PASSIVE   0x00200 /* Only passive scan allowed in the channel */
#define CHANNEL_DYN       0x00400 /* dynamic CCK-OFDM channel */
#define CHANNEL_XR        0x00800 /* XR channel */
#define CHANNEL_STURBO    0x02000 /* Static turbo, no 11a-only usage */
#define CHANNEL_HALF      0x04000 /* Half rate channel */
#define CHANNEL_QUARTER   0x08000 /* Quarter rate channel */
#define CHANNEL_HT20      0x10000 /* HT20 channel */
#define CHANNEL_HT40PLUS  0x20000 /* HT40 channel with extention channel above */
#define CHANNEL_HT40MINUS 0x40000 /* HT40 channel with extention channel below */

/* privFlags */
#define CHANNEL_INTERFERENCE    0x01 /* Software use: channel interference 
                                        used for as AR as well as RADAR 
                                        interference detection */
#define CHANNEL_DFS             0x02 /* DFS required on channel */
#define CHANNEL_4MS_LIMIT       0x04 /* 4msec packet limit on this channel */
#define CHANNEL_DFS_CLEAR       0x08 /* if channel has been checked for DFS */
#define CHANNEL_DISALLOW_ADHOC  0x10 /* ad hoc not allowed on this channel */
#define CHANNEL_PER_11D_ADHOC   0x20 /* ad hoc support is per 11d */
#define CHANNEL_EDGE_CH         0x40 /* Edge Channel */

#define CHANNEL_A           (CHANNEL_5GHZ|CHANNEL_OFDM)
#define CHANNEL_B           (CHANNEL_2GHZ|CHANNEL_CCK)
#define CHANNEL_PUREG       (CHANNEL_2GHZ|CHANNEL_OFDM)
#ifdef notdef
#define CHANNEL_G           (CHANNEL_2GHZ|CHANNEL_DYN)
#else
#define CHANNEL_G           (CHANNEL_2GHZ|CHANNEL_OFDM)
#endif
#define CHANNEL_T           (CHANNEL_5GHZ|CHANNEL_OFDM|CHANNEL_TURBO)
#define CHANNEL_ST          (CHANNEL_T|CHANNEL_STURBO)
#define CHANNEL_108G        (CHANNEL_2GHZ|CHANNEL_OFDM|CHANNEL_TURBO)
#define CHANNEL_108A        CHANNEL_T
#define CHANNEL_X           (CHANNEL_5GHZ|CHANNEL_OFDM|CHANNEL_XR)
#define CHANNEL_G_HT20      (CHANNEL_2GHZ|CHANNEL_HT20)
#define CHANNEL_A_HT20      (CHANNEL_5GHZ|CHANNEL_HT20)
#define CHANNEL_G_HT40PLUS  (CHANNEL_2GHZ|CHANNEL_HT40PLUS)
#define CHANNEL_G_HT40MINUS (CHANNEL_2GHZ|CHANNEL_HT40MINUS)
#define CHANNEL_A_HT40PLUS  (CHANNEL_5GHZ|CHANNEL_HT40PLUS)
#define CHANNEL_A_HT40MINUS (CHANNEL_5GHZ|CHANNEL_HT40MINUS)
#define CHANNEL_ALL \
        (CHANNEL_OFDM|CHANNEL_CCK| CHANNEL_2GHZ | CHANNEL_5GHZ | CHANNEL_TURBO | CHANNEL_HT20 | CHANNEL_HT40PLUS | CHANNEL_HT40MINUS)
#define CHANNEL_ALL_NOTURBO (CHANNEL_ALL &~ CHANNEL_TURBO)


#define HAL_ANTENNA_MIN_MODE  0
#define HAL_ANTENNA_FIXED_A   1
#define HAL_ANTENNA_FIXED_B   2
#define HAL_ANTENNA_MAX_MODE  3

typedef struct {
    u_int32_t   ackrcv_bad;
    u_int32_t   rts_bad;
    u_int32_t   rts_good;
    u_int32_t   fcs_bad;
    u_int32_t   beacons;
} HAL_MIB_STATS;

typedef u_int16_t HAL_CTRY_CODE;        /* country code */
typedef u_int16_t HAL_REG_DOMAIN;       /* regulatory domain code */

enum {
    CTRY_DEBUG  = 0x1ff,        /* debug country code */
    CTRY_DEFAULT    = 0         /* default country code */
};

typedef enum {
        REG_EXT_FCC_MIDBAND            = 0,
        REG_EXT_JAPAN_MIDBAND          = 1,
        REG_EXT_FCC_DFS_HT40           = 2,
        REG_EXT_JAPAN_NONDFS_HT40      = 3,
        REG_EXT_JAPAN_DFS_HT40         = 4
} REG_EXT_BITMAP;       

/*
 * Regulatory related information
 */
typedef struct _HAL_COUNTRY_ENTRY{
    u_int16_t   countryCode;  /* HAL private */
    u_int16_t   regDmnEnum;   /* HAL private */
    u_int16_t   regDmn5G;
    u_int16_t   regDmn2G;
    u_int8_t    isMultidomain;
    u_int8_t    iso[3];       /* ISO CC + (I)ndoor/(O)utdoor or both ( ) */
} HAL_COUNTRY_ENTRY;

enum {
    HAL_MODE_11A              = 0x00001,      /* 11a channels */
    HAL_MODE_TURBO            = 0x00002,      /* 11a turbo-only channels */
    HAL_MODE_11B              = 0x00004,      /* 11b channels */
    HAL_MODE_PUREG            = 0x00008,      /* 11g channels (OFDM only) */
#ifdef notdef                 
    HAL_MODE_11G              = 0x00010,      /* 11g channels (OFDM/CCK) */
#else                         
    HAL_MODE_11G              = 0x00008,      /* XXX historical */
#endif                        
    HAL_MODE_108G             = 0x00020,      /* 11a+Turbo channels */
    HAL_MODE_108A             = 0x00040,      /* 11g+Turbo channels */
    HAL_MODE_XR               = 0x00100,      /* XR channels */
    HAL_MODE_11A_HALF_RATE    = 0x00200,      /* 11A half rate channels */
    HAL_MODE_11A_QUARTER_RATE = 0x00400,      /* 11A quarter rate channels */
    HAL_MODE_11NG_HT20        = 0x00800,      /* 11N-G HT20 channels */
    HAL_MODE_11NA_HT20        = 0x01000,      /* 11N-A HT20 channels */
    HAL_MODE_11NG_HT40PLUS    = 0x02000,      /* 11N-G HT40 + channels */
    HAL_MODE_11NG_HT40MINUS   = 0x04000,      /* 11N-G HT40 - channels */
    HAL_MODE_11NA_HT40PLUS    = 0x08000,      /* 11N-A HT40 + channels */
    HAL_MODE_11NA_HT40MINUS   = 0x10000,      /* 11N-A HT40 - channels */
    HAL_MODE_ALL              = 0xffffffff
};

#define HAL_MODE_11N_MASK \
  ( HAL_MODE_11NG_HT20 | HAL_MODE_11NA_HT20 | HAL_MODE_11NG_HT40PLUS | \
    HAL_MODE_11NG_HT40MINUS | HAL_MODE_11NA_HT40PLUS | HAL_MODE_11NA_HT40MINUS )

#define UL_BIT_MASK 0x80000000

typedef struct {
    int     rateCount;      /* NB: for proper padding */
    u_int8_t    rateCodeToIndex[256];    /* back mapping */
    struct {
        u_int8_t    valid;      /* valid for rate control use */
        u_int8_t    phy;        /* CCK/OFDM/XR */
        u_int32_t   rateKbps;   /* transfer rate in kbs */
        u_int8_t    rateCode;   /* rate for h/w descriptors */
        u_int8_t    shortPreamble;  /* mask for enabling short
                         * preamble in CCK rate code */
        u_int8_t    dot11Rate;  /* value for supported rates
                         * info element of MLME */
        u_int8_t    controlRate;    /* index of next lower basic
                         * rate; used for dur. calcs */
        u_int16_t   lpAckDuration;  /* long preamble ACK duration */
        u_int16_t   spAckDuration;  /* short preamble ACK duration*/
    } info[32];
} HAL_RATE_TABLE;

typedef struct {
    u_int       rs_count;       /* number of valid entries */
    u_int8_t    rs_rates[32];       /* rates */
} HAL_RATE_SET;

typedef enum {
    HAL_ANT_VARIABLE = 0,           /* variable by programming */
    HAL_ANT_FIXED_A  = 1,           /* fixed to 11a frequencies */
    HAL_ANT_FIXED_B  = 2,           /* fixed to 11b frequencies */
} HAL_ANT_SETTING;

typedef enum {
    HAL_M_STA   = 1,            /* infrastructure station */
    HAL_M_IBSS  = 0,            /* IBSS (adhoc) station */
    HAL_M_HOSTAP    = 6,            /* Software Access Point */
    HAL_M_MONITOR   = 8         /* Monitor mode */
} HAL_OPMODE;

typedef struct {
    u_int8_t    kv_type;        /* one of HAL_CIPHER */
    u_int8_t    kv_pad;
    u_int16_t   kv_len;         /* length in bits */
    u_int8_t    kv_val[16];     /* enough for 128-bit keys */
    u_int8_t    kv_mic[8];      /* TKIP Rx MIC key */
    u_int8_t    kv_txmic[8];    /* TKIP Tx MIC key */
} HAL_KEYVAL;

typedef enum {
    HAL_KEY_TYPE_CLEAR,
    HAL_KEY_TYPE_WEP,
    HAL_KEY_TYPE_AES,
    HAL_KEY_TYPE_TKIP,
} HAL_KEY_TYPE;

typedef enum {
    HAL_CIPHER_WEP      = 0,
    HAL_CIPHER_AES_OCB  = 1,
    HAL_CIPHER_AES_CCM  = 2,
    HAL_CIPHER_CKIP     = 3,
    HAL_CIPHER_TKIP     = 4,
    HAL_CIPHER_CLR      = 5,        /* no encryption */

    HAL_CIPHER_MIC      = 127       /* TKIP-MIC, not a cipher */
} HAL_CIPHER;

enum {
    HAL_SLOT_TIME_6  = 6,
    HAL_SLOT_TIME_9  = 9,
    HAL_SLOT_TIME_20 = 20,
};

/* 11n */
typedef enum {
        HAL_HT_MACMODE_20       = 0,            /* 20 MHz operation */
        HAL_HT_MACMODE_2040     = 1,            /* 20/40 MHz operation */
} HAL_HT_MACMODE;

typedef enum {
    HAL_HT_EXTPROTSPACING_20    = 0,            /* 20 MHZ spacing */
    HAL_HT_EXTPROTSPACING_25    = 1,            /* 25 MHZ spacing */
} HAL_HT_EXTPROTSPACING;

typedef struct {
    HAL_HT_MACMODE              ht_macmode;             /* MAC - 20/40 mode */
    HAL_HT_EXTPROTSPACING       ht_extprotspacing;      /* ext channel protection spacing */
} HAL_HT_CWM;

typedef enum {
    HAL_RX_CLEAR_CTL_LOW        = 0x1,  /* force control channel to appear busy */
    HAL_RX_CLEAR_EXT_LOW        = 0x2,  /* force extension channel to appear busy */
} HAL_HT_RXCLEAR;

typedef enum {
    HAL_FREQ_BAND_5GHZ          = 0,
    HAL_FREQ_BAND_2GHZ          = 1,
} HAL_FREQ_BAND;

#define HAL_RATESERIES_RTS_CTS  0x0001  /* use rts/cts w/this series */
#define HAL_RATESERIES_2040     0x0002  /* use ext channel for series */
#define HAL_RATESERIES_HALFGI   0x0004  /* use half-gi for series */
#define HAL_RATESERIES_STBC     0x0008  /* use STBC for series */

typedef struct {
    u_int   Tries;
    u_int   Rate;
    u_int   PktDuration;
    u_int   ChSel;
    u_int   RateFlags;
} HAL_11N_RATE_SERIES;

enum {
    HAL_TRUE_CHIP = 1
};

/*
 * Per-station beacon timer state.  Note that the specified
 * beacon interval (given in TU's) can also include flags
 * to force a TSF reset and to enable the beacon xmit logic.
 * If bs_cfpmaxduration is non-zero the hardware is setup to
 * coexist with a PCF-capable AP.
 */
typedef struct {
    u_int32_t   bs_nexttbtt;        /* next beacon in TU */
    u_int32_t   bs_nextdtim;        /* next DTIM in TU */
    u_int32_t   bs_intval;          /* beacon interval+flags */
#define HAL_BEACON_PERIOD       0x0000ffff  /* beacon interval period */
#define HAL_BEACON_ENA          0x00800000  /* beacon xmit enable */
#define HAL_BEACON_RESET_TSF    0x01000000  /* clear TSF */
#define HAL_TSFOOR_THRESHOLD    0x00004240 /* TSF OOR threshold (16k us) */ 
    u_int32_t   bs_dtimperiod;
    u_int16_t   bs_cfpperiod;       /* CFP period in TU */
    u_int16_t   bs_cfpmaxduration;  /* max CFP duration in TU */
    u_int32_t   bs_cfpnext;         /* next CFP in TU */
    u_int16_t   bs_timoffset;       /* byte offset to TIM bitmap */
    u_int16_t   bs_bmissthreshold;  /* beacon miss threshold */
    u_int32_t   bs_sleepduration;   /* max sleep duration */
    u_int32_t   bs_tsfoor_threshold;/* TSF out of range threshold */
} HAL_BEACON_STATE;

/*
 * Per-node statistics maintained by the driver for use in
 * optimizing signal quality and other operational aspects.
 */
typedef struct {
    u_int32_t   ns_avgbrssi;    /* average beacon rssi */
    u_int32_t   ns_avgrssi;     /* average rssi of all received frames */
    u_int32_t   ns_avgtxrssi;   /* average tx rssi */
    u_int32_t   ns_avgtxrate;   /* average IEEE tx rate (in 500kbps units) */
#ifdef ATH_SWRETRY    
    u_int32_t    ns_swretryfailcount; /*number of sw retries which got failed*/
    u_int32_t    ns_swretryframecount; /*total number of frames that are sw retried*/
#endif        
} HAL_NODE_STATS;

typedef struct {
    u_int8_t txctl_offset;
    u_int8_t txctl_numwords;
    u_int8_t txstatus_offset;
    u_int8_t txstatus_numwords;

    u_int8_t rxctl_offset;
    u_int8_t rxctl_numwords;
    u_int8_t rxstatus_offset;
    u_int8_t rxstatus_numwords;

    u_int8_t macRevision;
} HAL_DESC_INFO;

#define HAL_RSSI_EP_MULTIPLIER  (1<<7)  /* pow2 to optimize out * and / */
#define HAL_RATE_EP_MULTIPLIER  (1<<7)  /* pow2 to optimize out * and / */

/*
 * GPIO Output mux can select from a number of different signals as input.
 * The current implementation uses 5 of these input signals:
 *     - An output value specified by the caller;
 *     - The Attention LED signal provided by the PCIE chip;
 *     - The Power     LED signal provided by the PCIE chip;
 *     - The Network LED pin controlled by the chip's MAC;
 *     - The Power   LED pin controlled by the chip's MAC.
 */
typedef enum {
    HAL_GPIO_OUTPUT_MUX_AS_OUTPUT,
    HAL_GPIO_OUTPUT_MUX_AS_PCIE_ATTENTION_LED,
    HAL_GPIO_OUTPUT_MUX_AS_PCIE_POWER_LED,
    HAL_GPIO_OUTPUT_MUX_AS_MAC_NETWORK_LED,
    HAL_GPIO_OUTPUT_MUX_AS_MAC_POWER_LED,
    HAL_GPIO_OUTPUT_MUX_AS_WLAN_ACTIVE,
    HAL_GPIO_OUTPUT_MUX_AS_TX_FRAME,
    HAL_GPIO_OUTPUT_MUX_NUM_ENTRIES    // always keep this type last; it must map to the number of entries in this enumeration
} HAL_GPIO_OUTPUT_MUX_TYPE;

typedef enum {
    HAL_GPIO_INTR_LOW = 0,
    HAL_GPIO_INTR_HIGH,
    HAL_GPIO_INTR_DISABLE
} HAL_GPIO_INTR_TYPE;

/*
** Definition of HAL operating parameters
**
** This ENUM provides a mechanism for external objects to update HAL operating
** parameters through a common ioctl-like interface.  The parameter IDs are
** used to identify the specific parameter in question to the get/set interface.
*/

typedef enum {
    HAL_CONFIG_DMA_BEACON_RESPONSE_TIME = 0,
    HAL_CONFIG_SW_BEACON_RESPONSE_TIME,
    HAL_CONFIG_ADDITIONAL_SWBA_BACKOFF,
    HAL_CONFIG_6MB_ACK,                
    HAL_CONFIG_CWMIGNOREEXTCCA,        
    HAL_CONFIG_FORCEBIAS,              
    HAL_CONFIG_FORCEBIASAUTO,
    HAL_CONFIG_PCIEPOWERSAVEENABLE,    
    HAL_CONFIG_PCIEL1SKPENABLE,        
    HAL_CONFIG_PCIECLOCKREQ,           
    HAL_CONFIG_PCIEWAEN,               
    HAL_CONFIG_PCIEPOWERRESET,         
    HAL_CONFIG_PCIERESTORE,         
    HAL_CONFIG_HTENABLE,               
    HAL_CONFIG_DISABLETURBOG,          
    HAL_CONFIG_OFDMTRIGLOW,            
    HAL_CONFIG_OFDMTRIGHIGH,           
    HAL_CONFIG_CCKTRIGHIGH,            
    HAL_CONFIG_CCKTRIGLOW,             
    HAL_CONFIG_ENABLEANI,              
    HAL_CONFIG_NOISEIMMUNITYLVL,       
    HAL_CONFIG_OFDMWEAKSIGDET,         
    HAL_CONFIG_CCKWEAKSIGTHR,          
    HAL_CONFIG_SPURIMMUNITYLVL,        
    HAL_CONFIG_FIRSTEPLVL,             
    HAL_CONFIG_RSSITHRHIGH,            
    HAL_CONFIG_RSSITHRLOW,             
    HAL_CONFIG_DIVERSITYCONTROL,       
    HAL_CONFIG_ANTENNASWITCHSWAP,
    HAL_CONFIG_DEBUG,
    HAL_CONFIG_RXMIT_LASTPKT,
    HAL_CONFIG_RXMIT_FIRSTPKT,
    HAL_CONFIG_TXMIT_LASTPKT,
    HAL_CONFIG_TXMIT_FIRSTPKT,

    HAL_CONFIG_OPS_PARAM_MAX
} HAL_CONFIG_OPS_PARAMS_TYPE;

/*
 * Diagnostic interface.  This is an open-ended interface that
 * is opaque to applications.  Diagnostic programs use this to
 * retrieve internal data structures, etc.  There is no guarantee
 * that calling conventions for calls other than HAL_DIAG_REVS
 * are stable between HAL releases; a diagnostic application must
 * use the HAL revision information to deal with ABI/API differences.
 */
enum {
    HAL_DIAG_REVS       = 0,    /* MAC/PHY/Radio revs */
    HAL_DIAG_EEPROM     = 1,    /* EEPROM contents */
    HAL_DIAG_EEPROM_EXP_11A = 2,    /* EEPROM 5112 power exp for 11a */
    HAL_DIAG_EEPROM_EXP_11B = 3,    /* EEPROM 5112 power exp for 11b */
    HAL_DIAG_EEPROM_EXP_11G = 4,    /* EEPROM 5112 power exp for 11g */
    HAL_DIAG_ANI_CURRENT    = 5,    /* ANI current channel state */
    HAL_DIAG_ANI_OFDM   = 6,    /* ANI OFDM timing error stats */
    HAL_DIAG_ANI_CCK    = 7,    /* ANI CCK timing error stats */
    HAL_DIAG_ANI_STATS  = 8,    /* ANI statistics */
    HAL_DIAG_RFGAIN     = 9,    /* RfGain GAIN_VALUES */
    HAL_DIAG_RFGAIN_CURSTEP = 10,   /* RfGain GAIN_OPTIMIZATION_STEP */
    HAL_DIAG_PCDAC      = 11,   /* PCDAC table */
    HAL_DIAG_TXRATES    = 12,   /* Transmit rate table */
    HAL_DIAG_REGS       = 13,   /* Registers */
    HAL_DIAG_ANI_CMD    = 14,   /* ANI issue command */
    HAL_DIAG_SETKEY     = 15,   /* Set keycache backdoor */
    HAL_DIAG_RESETKEY   = 16,   /* Reset keycache backdoor */
    HAL_DIAG_EEREAD     = 17,   /* Read EEPROM word */
    HAL_DIAG_EEWRITE    = 18,   /* Write EEPROM word */
    HAL_DIAG_TXCONT     = 19,   /* TX99 settings */
    HAL_DIAG_SET_RADAR  = 20,   /* Set Radar thresholds */
    HAL_DIAG_GET_RADAR  = 21,   /* Get Radar thresholds */
    HAL_DIAG_USENOL     = 22,   /* Turn on/off the use of the radar NOL */
    HAL_DIAG_GET_USENOL = 23,   /* Get status of the use of the radar NOL */
    HAL_DIAG_REGREAD        = 24,   /* Reg reads */
    HAL_DIAG_REGWRITE       = 25,   /* Reg writes */
    HAL_DIAG_GET_REGBASE    = 26,   /* Get register base */
    HAL_DIAG_PRINT_REG      = 27,
    HAL_DIAG_RDWRITE        = 28,   /* Write regulatory domain */
    HAL_DIAG_RDREAD         = 29    /* Get regulatory domain */
};

/* For register print */
#define HAL_DIAG_PRINT_REG_COUNTER  0x00000001 /* print tf, rf, rc, cc counters */
#define HAL_DIAG_PRINT_REG_ALL      0x80000000 /* print all registers */


#ifdef ATH_CCX
typedef struct {
    u_int8_t    clockRate;
    u_int32_t   noiseFloor;
    u_int8_t    ccaThreshold;
} HAL_CHANNEL_DATA;

typedef struct halCounters {
    u_int32_t   txFrameCount;
    u_int32_t   rxFrameCount;
    u_int32_t   rxClearCount;
    u_int32_t   cycleCount;
    u_int8_t    isRxActive;     // AH_TRUE (1) or AH_FALSE (0)
    u_int8_t    isTxActive;     // AH_TRUE (1) or AH_FALSE (0)
} HAL_COUNTERS;
#endif

/* DFS defines */

struct  dfs_pulse {
    u_int32_t    rp_numpulses;    /* Num of pulses in radar burst */
    u_int32_t    rp_pulsedur;    /* Duration of each pulse in usecs */
    u_int32_t    rp_pulsefreq;    /* Frequency of pulses in burst */
    u_int32_t    rp_max_pulsefreq;    /* Frequency of pulses in burst */
        u_int32_t       rp_patterntype;  /*fixed or variable pattern type*/
    u_int32_t    rp_pulsevar;    /* Time variation of pulse duration for
                       matched filter (single-sided) in usecs */
    u_int32_t    rp_threshold;    /* Thershold for MF output to indicate 
                       radar match */
    u_int32_t    rp_mindur;    /* Min pulse duration to be considered for
                       this pulse type */
    u_int32_t    rp_maxdur;    /* Max pusle duration to be considered for
                       this pulse type */
    u_int32_t    rp_rssithresh;    /* Minimum rssi to be considered a radar pulse */
    u_int32_t    rp_meanoffset;    /* Offset for timing adjustment */
    int32_t        rp_rssimargin;  /* rssi threshold margin. In Turbo Mode HW reports rssi 3dBm */
                                        /* lower than in non TURBO mode. This will be used to offset that diff.*/
    u_int32_t    rp_pulseid;    /* Unique ID for identifying filter */

};
struct  dfs_staggered_pulse {
    u_int32_t    rp_numpulses;    /* Num of pulses in radar burst */
    u_int32_t    rp_pulsedur;    /* Duration of each pulse in usecs */
    u_int32_t    rp_min_pulsefreq;    /* Frequency of pulses in burst */
    u_int32_t    rp_max_pulsefreq;    /* Frequency of pulses in burst */
        u_int32_t       rp_patterntype;  /*fixed or variable pattern type*/
    u_int32_t    rp_pulsevar;    /* Time variation of pulse duration for
                       matched filter (single-sided) in usecs */
    u_int32_t    rp_threshold;    /* Thershold for MF output to indicate 
                       radar match */
    u_int32_t    rp_mindur;    /* Min pulse duration to be considered for
                       this pulse type */
    u_int32_t    rp_maxdur;    /* Max pusle duration to be considered for
                       this pulse type */
    u_int32_t    rp_rssithresh;    /* Minimum rssi to be considered a radar pulse */
    u_int32_t    rp_meanoffset;    /* Offset for timing adjustment */
    int32_t        rp_rssimargin;  /* rssi threshold margin. In Turbo Mode HW reports rssi 3dBm */
                                        /* lower than in non TURBO mode. This will be used to offset that diff.*/
    u_int32_t    rp_pulseid;    /* Unique ID for identifying filter */

};
struct dfs_bin5pulse {
        u_int32_t       b5_threshold;          /* Number of bin5 pulses to indicate detection */
        u_int32_t       b5_mindur;             /* Min duration for a bin5 pulse */
        u_int32_t       b5_maxdur;             /* Max duration for a bin5 pulse */
        u_int32_t       b5_timewindow;         /* Window over which to count bin5 pulses */
        u_int32_t       b5_rssithresh;         /* Min rssi to be considered a pulse */
        u_int32_t       b5_rssimargin;         /* rssi threshold margin. In Turbo Mode HW reports rssi 3dB */
};

typedef struct {
        int32_t         pe_firpwr;      /* FIR pwr out threshold */
        int32_t         pe_rrssi;       /* Radar rssi thresh */
        int32_t         pe_height;      /* Pulse height thresh */
        int32_t         pe_prssi;       /* Pulse rssi thresh */
        int32_t         pe_inband;      /* Inband thresh */
        /* The following params are only for AR5413 and later */
        u_int32_t       pe_relpwr;      /* Relative power threshold in 0.5dB steps */
        u_int32_t       pe_relstep;     /* Pulse Relative step threshold in 0.5dB steps */
        u_int32_t       pe_maxlen;      /* Max length of radar sign in 0.8us units */
        HAL_BOOL        pe_usefir128;   /* Use the average in-band power measured over 128 cycles */         HAL_BOOL        pe_blockradar;  /* Enable to block radar check if pkt detect is done via OFDM
                                           weak signal detect or pkt is detected immediately after tx                                            to rx transition */
        HAL_BOOL        pe_enmaxrssi;   /* Enable to use the max rssi instead of the last rssi during                                            fine gain changes for radar detection */
} HAL_PHYERR_PARAM;

#define HAL_PHYERR_PARAM_NOVAL  65535
#define HAL_PHYERR_PARAM_ENABLE 0x8000  /* Enable/Disable if applicable */

/* DFS defines end */

#ifdef ATH_BT_COEX
/*
 * BT Co-existence definitions
 */
typedef enum {
    HAL_BT_MODULE_CSR_BC4       = 0,    /* CSR BlueCore v4 */
    HAL_BT_MODULE_VALKYRIE_V1   = 1,    /* Valkyrie v1 */
    HAL_MAX_BT_MODULES
} HAL_BT_MODULE;

typedef struct {
    HAL_BT_MODULE   bt_module;
    u_int8_t        bt_gpioBTActive;
    u_int8_t        bt_gpioBTPriority;
    u_int8_t        bt_gpioWLANActive;
    u_int8_t        bt_activePolarity;
} HAL_BT_COEX_INFO;

typedef enum {
    HAL_BT_COEX_MODE_LEGACY     = 0,    /* legacy rx_clear mode */
    HAL_BT_COEX_MODE_UNSLOTTED  = 1,    /* untimed/unslotted mode */
    HAL_BT_COEX_MODE_SLOTTED    = 2,    /* slotted mode */
    HAL_BT_COEX_MODE_DISALBED   = 3,    /* coexistence disabled */
} HAL_BT_COEX_MODE;

typedef struct {
    u_int8_t            bt_timeExtend;      /* extend rx_clear after tx/rx to protect
                                             * the burst (in usec). */
    HAL_BOOL            bt_txstateExtend;   /* extend rx_clear as long as txsm is
                                             * transmitting or waiting for ack. */
    HAL_BOOL            bt_txframeExtend;   /* extend rx_clear so that when tx_frame
                                             * is asserted, rx_clear will drop. */
    HAL_BT_COEX_MODE    bt_mode;            /* coexistence mode */
    HAL_BOOL            bt_quietCollision;  /* treat BT high priority traffic as
                                             * a quiet collision */
    HAL_BOOL            bt_rxclearPolarity; /* invert rx_clear as WLAN_ACTIVE */
    u_int8_t            bt_priorityTime;    /* slotted mode only. indicate the time in usec
                                             * from the rising edge of BT_ACTIVE to the time
                                             * BT_PRIORITY can be sampled to indicate priority. */
    u_int8_t            bt_firstSlotTime;   /* slotted mode only. indicate the time in usec
                                             * from the rising edge of BT_ACTIVE to the time
                                             * BT_PRIORITY can be sampled to indicate tx/rx and
                                             * BT_FREQ is sampled. */
    HAL_BOOL            bt_holdRxclear;     /* slotted mode only. rx_clear and bt_ant decision
                                             * will be held the entire time that BT_ACTIVE is asserted,
                                             * otherwise the decision is made before every slot boundry. */
} HAL_BT_COEX_CONFIG;
#endif

typedef struct mac_dbg_regs {
    u_int32_t dma_dbg_0;
    u_int32_t dma_dbg_1;
    u_int32_t dma_dbg_2;
    u_int32_t dma_dbg_3;
    u_int32_t dma_dbg_4;
    u_int32_t dma_dbg_5;
    u_int32_t dma_dbg_6;
    u_int32_t dma_dbg_7;
} mac_dbg_regs_t;

typedef enum hal_mac_hangs {
    dcu_chain_state = 0x1,
    dcu_complete_state = 0x2,
    qcu_state = 0x4,
    qcu_fsp_ok = 0x8,
    qcu_fsp_state = 0x10,
    qcu_stitch_state = 0x20,
    qcu_fetch_state = 0x40,
    qcu_complete_state = 0x80
} hal_mac_hangs_t;

typedef struct hal_mac_hang_check {
    u_int8_t dcu_chain_state;
    u_int8_t dcu_complete_state;
    u_int8_t qcu_state;
    u_int8_t qcu_fsp_ok;
    u_int8_t qcu_fsp_state;
    u_int8_t qcu_stitch_state;
    u_int8_t qcu_fetch_state;
    u_int8_t qcu_complete_state;
} hal_mac_hang_check_t;

typedef struct hal_bb_hang_check {
    u_int16_t hang_reg_offset;
    u_int32_t hang_val;
    u_int32_t hang_mask;
    u_int32_t hang_offset;
} hal_hw_hang_check_t; 

typedef enum hal_hw_hangs {
    HAL_DFS_BB_HANG_WAR          = 0x1,
    HAL_RIFS_BB_HANG_WAR         = 0x2,
    HAL_RX_STUCK_LOW_BB_HANG_WAR = 0x4,
    HAL_MAC_HANG_WAR             = 0x8,
    HAL_MAC_HANG_DETECTED        = 0x40000000,
    HAL_BB_HANG_DETECTED         = 0x80000000
} hal_hw_hangs_t;

#ifndef REMOVE_PKT_LOG
/* Hal packetlog defines */
typedef struct hal_log_data_ani {
    u_int8_t phyStatsDisable;
    u_int8_t noiseImmunLvl;
    u_int8_t spurImmunLvl;
    u_int8_t ofdmWeakDet;
    u_int8_t cckWeakThr;
    u_int16_t firLvl;
    u_int16_t listenTime;
    u_int32_t cycleCount;
    u_int32_t ofdmPhyErrCount;
    u_int32_t cckPhyErrCount;
    int8_t rssi;
    int32_t misc[8];            /* Can be used for HT specific or other misc info */
    /* TBD: Add other required log information */
} HAL_LOG_DATA_ANI;
/* Hal packetlog defines end*/
#endif

/*
 * MFP decryption options for initializing the MAC.
 */

typedef enum {
    HAL_MFP_QOSDATA = 0,        /* Decrypt MFP frames like QoS data frames. All chips before Merlin. */
    HAL_MFP_PASSTHRU,       /* Don't decrypt MFP frames at all. Passthrough */            
    HAL_MFP_HW_CRYPTO       /* hardware decryption enabled. Merlin can do it. */
} HAL_MFP_OPT_t;

typedef enum {
    HAL_ANT_DIV_COMB_LNA1_MINUS_LNA2 = 0,
    HAL_ANT_DIV_COMB_LNA2            = 1,
    HAL_ANT_DIV_COMB_LNA1            = 2,
    HAL_ANT_DIV_COMB_LNA1_PLUS_LNA2  = 3,
} HAL_ANT_DIV_COMB_LNA_CONF;

typedef struct {
    u_int8_t main_lna_conf;
    u_int8_t alt_lna_conf;
    u_int8_t fast_div_bias;
} HAL_ANT_COMB_CONFIG;

/*
** Forward Reference
*/      

struct ath_desc;

/*
 * Hardware Access Layer (HAL) API.
 *
 * Clients of the HAL call ath_hal_attach to obtain a reference to an
 * ath_hal structure for use with the device.  Hardware-related operations
 * that follow must call back into the HAL through interface, supplying
 * the reference as the first parameter.  Note that before using the
 * reference returned by ath_hal_attach the caller should verify the
 * ABI version number.
 */
struct ath_hal {
    u_int32_t   ah_magic;   /* consistency check magic number */
    u_int32_t   ah_abi;     /* HAL ABI version */
#define HAL_ABI_VERSION 0x05071100  /* YYMMDDnn */
    u_int16_t   ah_devid;   /* PCI device ID */
    u_int16_t   ah_subvendorid; /* PCI subvendor ID */
    HAL_ADAPTER_HANDLE ah_osdev; /* back pointer to OS adapter handle */
    HAL_SOFTC   ah_sc;      /* back pointer to driver/os state */
    HAL_BUS_TAG ah_st;      /* params for register r+w */
    HAL_BUS_HANDLE  ah_sh;
    HAL_BUS_TYPE    ah_bustype;
    HAL_CTRY_CODE   ah_countryCode;

    u_int32_t   ah_macVersion;  /* MAC version id */
    u_int16_t   ah_macRev;  /* MAC revision */
    u_int16_t   ah_phyRev;  /* PHY revision */
    /* NB: when only one radio is present the rev is in 5Ghz */
    u_int16_t   ah_analog5GhzRev;/* 5GHz radio revision */
    u_int16_t   ah_analog2GhzRev;/* 2GHz radio revision */
    u_int8_t    ah_decompMask[HAL_DECOMP_MASK_SIZE]; /* decomp mask array */
    const HAL_RATE_TABLE *__ahdecl(*ah_getRateTable)(struct ath_hal *,
                u_int mode);
    void      __ahdecl(*ah_detach)(struct ath_hal*);

    /* Reset functions */
    HAL_BOOL  __ahdecl(*ah_reset)(struct ath_hal *, HAL_OPMODE,
                HAL_CHANNEL *, HAL_HT_MACMODE, u_int8_t,
                                u_int8_t, HAL_HT_EXTPROTSPACING,
                                HAL_BOOL bChannelChange, HAL_STATUS *status);

    HAL_BOOL  __ahdecl(*ah_phyDisable)(struct ath_hal *);
    HAL_BOOL  __ahdecl(*ah_disable)(struct ath_hal *);
        void  __ahdecl(*ah_configPciPowerSave)(struct ath_hal *, int);
    void      __ahdecl(*ah_setPCUConfig)(struct ath_hal *);
    HAL_BOOL  __ahdecl(*ah_perCalibration)(struct ath_hal*, HAL_CHANNEL *, 
                u_int8_t, HAL_BOOL, HAL_BOOL *);
    void      __ahdecl(*ah_resetCalValid)(struct ath_hal*, HAL_CHANNEL *,
                       HAL_BOOL *);
    HAL_BOOL  __ahdecl(*ah_setTxPowerLimit)(struct ath_hal *, u_int32_t, u_int16_t);
    
    HAL_BOOL  __ahdecl(*ah_radarWait)(struct ath_hal *, HAL_CHANNEL *);

    /* New DFS declarations*/
    void       __ahdecl(*ah_arCheckDfs)(struct ath_hal *, HAL_CHANNEL *chan);
    void       __ahdecl(*ah_arDfsFound)(struct ath_hal *, HAL_CHANNEL *chan, u_int64_t nolTime);
    void       __ahdecl(*ah_arEnableDfs)(struct ath_hal *, HAL_PHYERR_PARAM *pe);
    void       __ahdecl(*ah_arGetDfsThresh)(struct ath_hal *, HAL_PHYERR_PARAM *pe);
    struct dfs_pulse *__ahdecl(*ah_arGetDfsRadars)(struct ath_hal *ah, u_int32_t dfsdomain, 
                                           int *numradars, struct dfs_bin5pulse **bin5pulses,
                                           int *numb5radars, HAL_PHYERR_PARAM *pe);
    HAL_CHANNEL* __ahdecl(*ah_getExtensionChannel)(struct ath_hal*ah);
    HAL_BOOL __ahdecl(*ah_isFastClockEnabled)(struct ath_hal*ah);
    /* Xr Functions */
    void      __ahdecl(*ah_xrEnable)(struct ath_hal *);
    void      __ahdecl(*ah_xrDisable)(struct ath_hal *);

    /* Transmit functions */
    HAL_BOOL  __ahdecl(*ah_updateTxTrigLevel)(struct ath_hal*,
                HAL_BOOL incTrigLevel);
    u_int16_t __ahdecl(*ah_getTxTrigLevel)(struct ath_hal *);
    int   __ahdecl(*ah_setupTxQueue)(struct ath_hal *, HAL_TX_QUEUE,
                const HAL_TXQ_INFO *qInfo);
    HAL_BOOL  __ahdecl(*ah_setTxQueueProps)(struct ath_hal *, int q,
                const HAL_TXQ_INFO *qInfo);
    HAL_BOOL  __ahdecl(*ah_getTxQueueProps)(struct ath_hal *, int q,
                HAL_TXQ_INFO *qInfo);
    HAL_BOOL  __ahdecl(*ah_releaseTxQueue)(struct ath_hal *ah, u_int q);
    HAL_BOOL  __ahdecl(*ah_resetTxQueue)(struct ath_hal *ah, u_int q);
    u_int32_t __ahdecl(*ah_getTxDP)(struct ath_hal*, u_int);
    HAL_BOOL  __ahdecl(*ah_setTxDP)(struct ath_hal*, u_int, u_int32_t txdp);
    u_int32_t __ahdecl(*ah_numTxPending)(struct ath_hal *, u_int q);
    HAL_BOOL  __ahdecl(*ah_startTxDma)(struct ath_hal*, u_int);
    HAL_BOOL  __ahdecl(*ah_stopTxDma)(struct ath_hal*, u_int);
    HAL_BOOL  __ahdecl(*ah_updateCTSForBursting)(struct ath_hal *,
                struct ath_desc *, struct ath_desc *,
                struct ath_desc *, struct ath_desc *,
                u_int32_t, u_int32_t);
    HAL_BOOL  __ahdecl(*ah_setupTxDesc)(struct ath_hal *, struct ath_desc *,
                u_int pktLen, u_int hdrLen,
                HAL_PKT_TYPE type, u_int txPower,
                u_int txRate0, u_int txTries0,
                u_int keyIx, u_int antMode, u_int flags,
                u_int rtsctsRate, u_int rtsctsDuration,
                u_int compicvLen, u_int compivLen,
                u_int comp);
    HAL_BOOL  __ahdecl(*ah_setupXTxDesc)(struct ath_hal *, struct ath_desc*,
                u_int txRate1, u_int txTries1,
                u_int txRate2, u_int txTries2,
                u_int txRate3, u_int txTries3);
    HAL_BOOL  __ahdecl(*ah_fillTxDesc)(struct ath_hal *, struct ath_desc *,
                u_int segLen, HAL_BOOL firstSeg,
                HAL_BOOL lastSeg, const struct ath_desc *);
    void       __ahdecl(*ah_clearTxDescStatus)(struct ath_hal *, struct ath_desc *);
#ifdef ATH_SWRETRY        
    void       __ahdecl(*ah_clearDestMask)(struct ath_hal *, struct ath_desc *);
#endif        
    HAL_STATUS __ahdecl(*ah_procTxDesc)(struct ath_hal *, struct ath_desc*);
    void       __ahdecl(*ah_getTxIntrQueue)(struct ath_hal *, u_int32_t *);
    void       __ahdecl(*ah_reqTxIntrDesc)(struct ath_hal *, struct ath_desc*);
    u_int32_t  __ahdecl(*ah_calcTxAirtime)(struct ath_hal *, struct ath_desc*);

    /* Receive Functions */
    u_int32_t __ahdecl(*ah_getRxDP)(struct ath_hal*);
    void      __ahdecl(*ah_setRxDP)(struct ath_hal*, u_int32_t rxdp);
    void      __ahdecl(*ah_enableReceive)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_stopDmaReceive)(struct ath_hal*);
    void      __ahdecl(*ah_startPcuReceive)(struct ath_hal*);
    void      __ahdecl(*ah_stopPcuReceive)(struct ath_hal*);
    void      __ahdecl(*ah_setMulticastFilter)(struct ath_hal*,
                u_int32_t filter0, u_int32_t filter1);
    HAL_BOOL  __ahdecl(*ah_setMulticastFilterIndex)(struct ath_hal*,
                u_int32_t index);
    HAL_BOOL  __ahdecl(*ah_clrMulticastFilterIndex)(struct ath_hal*,
                u_int32_t index);
    u_int32_t __ahdecl(*ah_getRxFilter)(struct ath_hal*);
    void      __ahdecl(*ah_setRxFilter)(struct ath_hal*, u_int32_t);
    HAL_BOOL  __ahdecl(*ah_setRxAbort)(struct ath_hal*, HAL_BOOL);
    HAL_BOOL  __ahdecl(*ah_setupRxDesc)(struct ath_hal *, struct ath_desc *,
                u_int32_t size, u_int flags);
    HAL_STATUS __ahdecl(*ah_procRxDesc)(struct ath_hal *, struct ath_desc *,
                u_int32_t phyAddr, struct ath_desc *next,
                u_int64_t tsf);
    void      __ahdecl(*ah_rxMonitor)(struct ath_hal *,
                const HAL_NODE_STATS *, HAL_CHANNEL *);
    void      __ahdecl(*ah_procMibEvent)(struct ath_hal *,
                const HAL_NODE_STATS *);

    /* Misc Functions */
    HAL_STATUS __ahdecl(*ah_getCapability)(struct ath_hal *,
                HAL_CAPABILITY_TYPE, u_int32_t capability,
                u_int32_t *result);
    HAL_BOOL   __ahdecl(*ah_setCapability)(struct ath_hal *,
                HAL_CAPABILITY_TYPE, u_int32_t capability,
                u_int32_t setting, HAL_STATUS *);
    HAL_BOOL   __ahdecl(*ah_getDiagState)(struct ath_hal *, int request,
                const void *args, u_int32_t argsize,
                void **result, u_int32_t *resultsize);
    void      __ahdecl(*ah_getMacAddress)(struct ath_hal *, u_int8_t *);
    HAL_BOOL  __ahdecl(*ah_setMacAddress)(struct ath_hal *, const u_int8_t*);
    void      __ahdecl(*ah_getBssIdMask)(struct ath_hal *, u_int8_t *);
    HAL_BOOL  __ahdecl(*ah_setBssIdMask)(struct ath_hal *, const u_int8_t*);
    HAL_BOOL  __ahdecl(*ah_setRegulatoryDomain)(struct ath_hal*,
                u_int16_t, HAL_STATUS *);
    void      __ahdecl(*ah_setLedState)(struct ath_hal*, HAL_LED_STATE);
    void      __ahdecl(*ah_setpowerledstate)(struct ath_hal *, u_int8_t);
    void      __ahdecl(*ah_setnetworkledstate)(struct ath_hal *, u_int8_t);
    void      __ahdecl(*ah_writeAssocid)(struct ath_hal*,
                const u_int8_t *bssid, u_int16_t assocId);
    HAL_BOOL  __ahdecl(*ah_gpioCfgInput)(struct ath_hal *, u_int32_t gpio);
    HAL_BOOL  __ahdecl(*ah_gpioCfgOutput)(struct ath_hal *, u_int32_t gpio, HAL_GPIO_OUTPUT_MUX_TYPE signalType);
    u_int32_t __ahdecl(*ah_gpioGet)(struct ath_hal *, u_int32_t gpio);
    HAL_BOOL  __ahdecl(*ah_gpioSet)(struct ath_hal *,
                u_int32_t gpio, u_int32_t val);
    void      __ahdecl(*ah_gpioSetIntr)(struct ath_hal*, u_int, u_int32_t);
    u_int32_t __ahdecl(*ah_getTsf32)(struct ath_hal*);
    u_int64_t __ahdecl(*ah_getTsf64)(struct ath_hal*);
    void      __ahdecl(*ah_resetTsf)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_detectCardPresent)(struct ath_hal*);
    void      __ahdecl(*ah_updateMibMacStats)(struct ath_hal*);
    void      __ahdecl(*ah_getMibMacStats)(struct ath_hal*, HAL_MIB_STATS*);
    HAL_RFGAIN __ahdecl(*ah_getRfGain)(struct ath_hal*);
    u_int     __ahdecl(*ah_getDefAntenna)(struct ath_hal*);
    void      __ahdecl(*ah_setDefAntenna)(struct ath_hal*, u_int);
    HAL_BOOL  __ahdecl(*ah_setSlotTime)(struct ath_hal*, u_int);
    u_int     __ahdecl(*ah_getSlotTime)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_setAckTimeout)(struct ath_hal*, u_int);
    u_int     __ahdecl(*ah_getAckTimeout)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_setCTSTimeout)(struct ath_hal*, u_int);
    u_int     __ahdecl(*ah_getCTSTimeout)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_setDecompMask)(struct ath_hal*, u_int16_t, int);
    void      __ahdecl(*ah_setCoverageClass)(struct ath_hal*, u_int8_t, int);
    HAL_STATUS __ahdecl(*ah_setQuiet)(struct ath_hal *,u_int16_t,u_int16_t,u_int16_t,u_int16_t);
    HAL_BOOL  __ahdecl(*ah_setAntennaSwitch)(struct ath_hal *, HAL_ANT_SETTING, HAL_CHANNEL *, u_int8_t *, u_int8_t *, u_int8_t *);
    void      __ahdecl(*ah_getDescInfo)(struct ath_hal*, HAL_DESC_INFO *);
    void      __ahdecl (*ah_markPhyInactive)(struct ath_hal *);
    HAL_STATUS  __ahdecl(*ah_selectAntConfig)(struct ath_hal *ah, u_int32_t cfg);
    u_int      __ahdecl(*ah_getMacVersion)(struct ath_hal *ah);
    /* Key Cache Functions */
    u_int32_t __ahdecl(*ah_getKeyCacheSize)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_resetKeyCacheEntry)(struct ath_hal*, u_int16_t);
    HAL_BOOL  __ahdecl(*ah_isKeyCacheEntryValid)(struct ath_hal *,
                u_int16_t);
    HAL_BOOL  __ahdecl(*ah_setKeyCacheEntry)(struct ath_hal*,
                u_int16_t, const HAL_KEYVAL *,
                const u_int8_t *, int);
    void      __ahdecl(*ah_divant)(struct ath_hal*, int value);
    HAL_BOOL  __ahdecl(*ah_setKeyCacheEntryMac)(struct ath_hal*,
                u_int16_t, const u_int8_t *);

    /* Power Management Functions */
    HAL_BOOL  __ahdecl(*ah_setPowerMode)(struct ath_hal*,
                HAL_POWER_MODE mode, int setChip);
    HAL_POWER_MODE __ahdecl(*ah_getPowerMode)(struct ath_hal*);
    void      __ahdecl(*ah_setSmPsMode)(struct ath_hal*, HAL_SMPS_MODE mode);
#if ATH_WOW        
    void      __ahdecl(*ah_wowApplyPattern)(struct ath_hal *ah, u_int8_t *pAthPattern, u_int8_t *pAthMask, int32_t pattern_count, u_int32_t athPatternLen);
    HAL_BOOL  __ahdecl(*ah_wowEnable)(struct ath_hal *ah, u_int32_t patternEnable);
    u_int32_t __ahdecl(*ah_wowWakeUp)(struct ath_hal *ah);
#endif        

    int16_t   __ahdecl(*ah_getChanNoise)(struct ath_hal *, HAL_CHANNEL *);

    /* Beacon Management Functions */
    void      __ahdecl(*ah_beaconInit)(struct ath_hal *,
                u_int32_t nexttbtt, u_int32_t intval);
    void      __ahdecl(*ah_setStationBeaconTimers)(struct ath_hal*,
                const HAL_BEACON_STATE *);
    void      __ahdecl(*ah_resetStationBeaconTimers)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_waitForBeaconDone)(struct ath_hal *,
                HAL_BUS_ADDR);

    /* Interrupt functions */
    HAL_BOOL  __ahdecl(*ah_isInterruptPending)(struct ath_hal*);
    HAL_BOOL  __ahdecl(*ah_getPendingInterrupts)(struct ath_hal*, HAL_INT*);
    HAL_INT   __ahdecl(*ah_getInterrupts)(struct ath_hal*);
    HAL_INT   __ahdecl(*ah_setInterrupts)(struct ath_hal*, HAL_INT);
    void      __ahdecl(*ah_SetIntrMitigationTimer)(struct ath_hal* ah,
                HAL_INT_MITIGATION reg, u_int32_t value);
    u_int32_t __ahdecl(*ah_GetIntrMitigationTimer)(struct ath_hal* ah,
                HAL_INT_MITIGATION reg);

    /* 11n Functions */
    void      __ahdecl(*ah_set11nTxDesc)(struct ath_hal *ah,
                struct ath_desc *ds, u_int pktLen, HAL_PKT_TYPE type,
                u_int txPower, u_int keyIx, HAL_KEY_TYPE keyType,
                u_int flags);
    void      __ahdecl(*ah_set11nRateScenario)(struct ath_hal *ah,
                struct ath_desc *ds, struct ath_desc *lastds, u_int durUpdateEn, u_int rtsctsRate, u_int rtsctsDuration,
                HAL_11N_RATE_SERIES series[], u_int nseries, u_int flags);
    void      __ahdecl(*ah_set11nAggrFirst)(struct ath_hal *ah,
                struct ath_desc *ds, u_int aggrLen);
    void      __ahdecl(*ah_set11nAggrMiddle)(struct ath_hal *ah,
                struct ath_desc *ds, u_int numDelims);
    void      __ahdecl(*ah_set11nAggrLast)(struct ath_hal *ah,
                struct ath_desc *ds);
    void      __ahdecl(*ah_clr11nAggr)(struct ath_hal *ah,
                struct ath_desc *ds);
    void      __ahdecl(*ah_set11nRifsBurstMiddle)(struct ath_hal *ah,
                struct ath_desc *ds);
    void      __ahdecl(*ah_set11nRifsBurstLast)(struct ath_hal *ah,
                struct ath_desc *ds);
    void      __ahdecl(*ah_clr11nRifsBurst)(struct ath_hal *ah,
                struct ath_desc *ds);
    void      __ahdecl(*ah_set11nAggrRifsBurst)(struct ath_hal *ah,
                struct ath_desc *ds);
    HAL_BOOL  __ahdecl(*ah_set11nRxRifs)(struct ath_hal *ah, HAL_BOOL enable);
    HAL_BOOL  __ahdecl(*ah_get11nRxRifs)(struct ath_hal *ah);
    HAL_BOOL  __ahdecl(*ah_detectBbHang)(struct ath_hal *ah);
    HAL_BOOL  __ahdecl(*ah_detectMacHang)(struct ath_hal *ah);
    void      __ahdecl(*ah_immunity)(struct ath_hal *ah, HAL_BOOL enable);
    void      __ahdecl(*ah_getHangTypes)(struct ath_hal *, hal_hw_hangs_t *);
    void      __ahdecl(*ah_set11nBurstDuration)(struct ath_hal *ah,
                struct ath_desc *ds, u_int burstDuration);
    void      __ahdecl(*ah_set11nVirtualMoreFrag)(struct ath_hal *ah,
                struct ath_desc *ds, u_int vmf);
    u_int32_t __ahdecl(*ah_get11nExtBusy)(struct ath_hal *ah);
    void      __ahdecl(*ah_set11nMac2040)(struct ath_hal *ah, HAL_HT_MACMODE);
    HAL_HT_RXCLEAR __ahdecl(*ah_get11nRxClear)(struct ath_hal *ah);
    void      __ahdecl(*ah_set11nRxClear)(struct ath_hal *ah, HAL_HT_RXCLEAR rxclear);
    int       __ahdecl(*ah_get11nHwPlatform)(struct ath_hal *ah);
    u_int32_t __ahdecl(*ah_getMibCycleCountsPct)(struct ath_hal *ah, u_int32_t*, u_int32_t*, u_int32_t*);
    void      __ahdecl(*ah_dmaRegDump)(struct ath_hal *);
    u_int32_t __ahdecl(*ah_ppmGetRssiDump)(struct ath_hal *);
    u_int32_t __ahdecl(*ah_ppmArmTrigger)(struct ath_hal *);
    int       __ahdecl(*ah_ppmGetTrigger)(struct ath_hal *);
    u_int32_t __ahdecl(*ah_ppmForce)(struct ath_hal *);
    void      __ahdecl(*ah_ppmUnForce)(struct ath_hal *);
    u_int32_t __ahdecl(*ah_ppmGetForceState)(struct ath_hal *);
    int       __ahdecl(*ah_getSpurInfo)(struct ath_hal *, int *, int, u_int16_t *);
    int       __ahdecl(*ah_setSpurInfo)(struct ath_hal *, int, int, u_int16_t *);
#ifdef ATH_CCX
    /* CCX Radio Measurement Specific Functions */
    void      __ahdecl(*ah_getMibCycleCounts)(struct ath_hal *ah, HAL_COUNTERS* pCnts);
    void      __ahdecl(*ah_clearMibCounters)(struct ath_hal *);
    u_int8_t  __ahdecl(*ah_getCcaThreshold)(struct ath_hal *);
    u_int32_t __ahdecl(*ah_getCurRSSI)(struct ath_hal *);
#endif
    
#ifdef ATH_BT_COEX
    /* Bluetooth Coexistence Functions */
    void      __ahdecl(*ah_getBTCoexInfo)(struct ath_hal *, HAL_BT_COEX_INFO *);
    void      __ahdecl(*ah_btCoexConfig)(struct ath_hal *, HAL_BT_COEX_CONFIG *);
    void      __ahdecl(*ah_btCoexSetQcuThresh)(struct ath_hal *, int);
    void      __ahdecl(*ah_btCoexSetWeights)(struct ath_hal *, u_int32_t, u_int32_t);
    void      __ahdecl(*ah_btCoexSetBmissThresh)(struct ath_hal *, u_int32_t);
    void      __ahdecl(*ah_btCoexDisable)(struct ath_hal *);
    int       __ahdecl(*ah_btCoexEnable)(struct ath_hal *);
#endif
    void     __ahdecl(*ah_getAntDviCombConf)(struct ath_hal *, HAL_ANT_COMB_CONFIG *);
    void     __ahdecl(*ah_setAntDviCombConf)(struct ath_hal *, HAL_ANT_COMB_CONFIG *);
};


/* 
 * Check the PCI vendor ID and device ID against Atheros' values
 * and return a printable description for any Atheros hardware.
 * AH_NULL is returned if the ID's do not describe Atheros hardware.
 */
extern  const char *__ahdecl ath_hal_probe(u_int16_t vendorid, u_int16_t devid);

/*
 * Attach the HAL for use with the specified device.  The device is
 * defined by the PCI device ID.  The caller provides an opaque pointer
 * to an upper-layer data structure (HAL_SOFTC) that is stored in the
 * HAL state block for later use.  Hardware register accesses are done
 * using the specified bus tag and handle.  On successful return a
 * reference to a state block is returned that must be supplied in all
 * subsequent HAL calls.  Storage associated with this reference is
 * dynamically allocated and must be freed by calling the ah_detach
 * method when the client is done.  If the attach operation fails a
 * null (AH_NULL) reference will be returned and a status code will
 * be returned if the status parameter is non-zero.
 */
extern  struct ath_hal * __ahdecl ath_hal_attach(u_int16_t devid, HAL_ADAPTER_HANDLE osdev, 
        HAL_SOFTC, HAL_BUS_TAG, HAL_BUS_HANDLE, HAL_BUS_TYPE,
        struct hal_reg_parm *hal_conf_parm, HAL_STATUS* status);

 /*
  * Set the Vendor ID for Vendor SKU's which can modify the
  * channel properties returned by ath_hal_init_channels.
  * Return AH_TRUE if set succeeds
  */
 
extern  HAL_BOOL __ahdecl ath_hal_setvendor(struct ath_hal *, u_int32_t );

/*
 * Return a list of channels available for use with the hardware.
 * The list is based on what the hardware is capable of, the specified
 * country code, the modeSelect mask, and whether or not outdoor
 * channels are to be permitted.
 *
 * The channel list is returned in the supplied array.  maxchans
 * defines the maximum size of this array.  nchans contains the actual
 * number of channels returned.  If a problem occurred or there were
 * no channels that met the criteria then AH_FALSE is returned.
 */
extern  HAL_BOOL __ahdecl ath_hal_init_channels(struct ath_hal *,
        HAL_CHANNEL *chans, u_int maxchans, u_int *nchans,
        u_int8_t *regclassids, u_int maxregids, u_int *nregids,
        HAL_CTRY_CODE cc, u_int32_t modeSelect,
        HAL_BOOL enableOutdoor, HAL_BOOL enableExtendedChannels);

/*
 * Calibrate noise floor data following a channel scan or similar.
 * This must be called prior retrieving noise floor data.
 */
extern  void __ahdecl ath_hal_process_noisefloor(struct ath_hal *ah);

/*
 * Return bit mask of wireless modes supported by the hardware.
 */
extern  u_int __ahdecl ath_hal_getwirelessmodes(struct ath_hal*, HAL_CTRY_CODE);

/*
 * Return rate table for specified mode (11a, 11b, 11g, etc).
 */
extern  const HAL_RATE_TABLE * __ahdecl ath_hal_getratetable(struct ath_hal *,
        u_int mode);

/*
 * Calculate the transmit duration of a frame.
 */
extern u_int16_t __ahdecl ath_hal_computetxtime(struct ath_hal *,
        const HAL_RATE_TABLE *rates, u_int32_t frameLen,
        u_int16_t rateix, HAL_BOOL shortPreamble);

/*
 * Return if device is public safety.
 */
extern HAL_BOOL __ahdecl ath_hal_ispublicsafetysku(struct ath_hal *);

/*
 * Convert between IEEE channel number and channel frequency
 * using the specified channel flags; e.g. CHANNEL_2GHZ.
 */
extern  u_int __ahdecl ath_hal_mhz2ieee(struct ath_hal *, u_int mhz, u_int flags);

/*
 * Find the HAL country code from its ISO name.
 */
extern HAL_CTRY_CODE __ahdecl findCountryCode(u_int8_t *countryString);

/*
 * Find the HAL country code from its domain enum.
 */
extern HAL_CTRY_CODE __ahdecl findCountryCodeByRegDomain(HAL_REG_DOMAIN regdmn);

/*
 * Return the current regulatory domain information
 */
extern void __ahdecl ath_hal_getCurrentCountry(void *ah, HAL_COUNTRY_ENTRY* ctry);

/*
 * Return the 802.11D supported country table
 */
extern u_int __ahdecl ath_hal_get11DCountryTable(void *ah, HAL_COUNTRY_ENTRY* ctries, u_int nCtryBufCnt);

extern void __ahdecl ath_hal_set_singleWriteKC(struct ath_hal *ah, u_int8_t singleWriteKC);

extern HAL_BOOL __ahdecl ath_hal_enabledANI(struct ath_hal *ah);

/*
** Prototypes for the HAL layer configuration get and set functions
*/

u_int32_t ath_hal_set_config_param(struct ath_hal *ah,
                                   HAL_CONFIG_OPS_PARAMS_TYPE p,
                                   void *b);

u_int32_t ath_hal_get_config_param(struct ath_hal *ah,
                                   HAL_CONFIG_OPS_PARAMS_TYPE p,
                                   void *b);

/*
 * Return a version string for the HAL release.
 */
extern  char ath_hal_version[];
/*
 * Return a NULL-terminated array of build/configuration options.
 */
extern  const char* ath_hal_buildopts[];
#ifdef ATH_CCX
extern void ath_hal_get_sernum(struct ath_hal *ah, u_int8_t *pSn);
extern HAL_BOOL ath_hal_get_chandata(struct ath_hal * ah, HAL_CHANNEL * chan, HAL_CHANNEL_DATA* pData);
#endif

#ifdef DBG
extern u_int32_t ath_hal_readRegister(struct ath_hal *ah, u_int32_t offset);
extern void ath_hal_writeRegister(struct ath_hal *ah, u_int32_t offset, u_int32_t value);
#endif

/*
 * Return common mode power in 5Ghz
 */
extern u_int8_t __ahdecl getCommonPower(u_int16_t freq);
#endif /* _ATH_AH_H_ */
