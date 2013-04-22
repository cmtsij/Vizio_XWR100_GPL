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
 * Definitions for core driver
 * This is a common header file for all platforms and operating systems.
 */
#ifndef _ATH_RC_H_
#define	_ATH_RC_H_

#include <osdep.h>

#include "ah.h"
#include "if_athrate.h"
#include <_ieee80211.h>

#include <ath_dev.h>

/*
 * Set configuration parameters here; they cover multiple files.
 */
#define	ATHEROS_DEBUG	1

/*
 * Compatibility shims.  We leverage the work already done for the hal.
 */
typedef	u_int8_t	A_UINT8;
typedef	int8_t		A_INT8;
typedef	u_int16_t	A_UINT16;
typedef	int16_t		A_INT16;
typedef	u_int32_t	A_UINT32;
typedef	int32_t		A_INT32;
typedef	u_int64_t	A_UINT64;

#define	FALSE	0
#define	TRUE	1

typedef	int8_t		A_RSSI;
typedef	int32_t		A_RSSI32;

typedef u_int8_t	WLAN_PHY;

#ifndef	INLINE
#define	INLINE		__inline
#endif

#ifndef A_MIN
#define	A_MIN(a,b)	((a)<(b)?(a):(b))
#endif

#ifndef A_MAX
#define	A_MAX(a,b)	((a)>(b)?(a):(b))
#endif

/*
 * Use the hal os glue code to get ms time
 */
#define	A_MS_TICKGET()	((A_UINT32) CONVERT_SYSTEM_TIME_TO_MS(OS_GET_TIMESTAMP()))
#define	A_MEM_ZERO(p,s)	OS_MEMZERO(p,s)

#define	WLAN_PHY_OFDM	IEEE80211_T_OFDM
#define	WLAN_PHY_TURBO	IEEE80211_T_TURBO
#define	WLAN_PHY_CCK	IEEE80211_T_CCK
#define	WLAN_PHY_XR	(IEEE80211_T_TURBO+1)

#define IS_CHAN_TURBO(_c)	(((_c)->channelFlags & CHANNEL_TURBO) != 0)
#define	IS_CHAN_2GHZ(_c)	(((_c)->channelFlags & CHANNEL_2GHZ) != 0)


#define	WIRELESS_MODE_TURBO     WIRELESS_MODE_108a  /* NB: diff reduction */

#define	RX_FLIP_THRESHOLD	3	/* XXX */

enum {
    WLAN_RC_PHY_CCK,
    WLAN_RC_PHY_OFDM,
    WLAN_RC_PHY_TURBO,
    WLAN_RC_PHY_XR,
    WLAN_RC_PHY_HT_20_SS,
    WLAN_RC_PHY_HT_20_DS,
    WLAN_RC_PHY_HT_40_SS,
    WLAN_RC_PHY_HT_40_DS,
    WLAN_RC_PHY_HT_20_SS_HGI,
    WLAN_RC_PHY_HT_20_DS_HGI,
    WLAN_RC_PHY_HT_40_SS_HGI,
    WLAN_RC_PHY_HT_40_DS_HGI,
    WLAN_RC_PHY_MAX
};

/*
 * State structures for new rate adaptation code
 *
 * NOTE: Modifying these structures will impact
 * the Perl script that parses packet logging data.
 * See the packet logging module for more information.
 */
#define	MAX_TX_RATE_TBL	        64
#define MAX_TX_RATE_PHY         48

typedef struct TxRateCrtlState_s {
    A_RSSI rssiThres;           /* required rssi for this rate (dB) */
    A_UINT8 per;                /* recent estimate of packet error rate (%) */
} TxRateCtrlState;

typedef struct TxRateCtrl_s {
    TxRateCtrlState state[MAX_TX_RATE_TBL];    /* state for each rate */
    A_RSSI rssiLast;            /* last ack rssi */
    A_RSSI rssiLastLkup;        /* last ack rssi used for lookup */
    A_RSSI rssiLastPrev;        /* previous last ack rssi */
    A_RSSI rssiLastPrev2;       /* 2nd previous last ack rssi */
    A_RSSI32 rssiSumCnt;        /* count of rssiSum for averaging */
    A_RSSI32 rssiSumRate;       /* rate that we are averaging */
    A_RSSI32 rssiSum;           /* running sum of rssi for averaging */
    A_UINT32 validTxRateMask;   /* mask of valid rates */
    A_UINT8 rateTableSize;      /* rate table size */
    A_UINT8 rateMax;            /* max rate that has recently worked */
    A_UINT8 probeRate;          /* rate we are probing at */
    A_INT8  antFlipCnt;         /* number of consec times retry=2,3 */
    A_UINT8 misc[16];           /* miscellaneous state */
    A_UINT32 rssiTime;          /* msec timestamp for last ack rssi */
    A_UINT32 rssiDownTime;      /* msec timestamp for last down step */
    A_UINT32 probeTime;         /* msec timestamp for last probe */
    A_UINT8 hwMaxRetryRate;     /* rate of last max retry fail */
    A_UINT8 hwMaxRetryPktCnt;   /* num packets since we got HW max retry error */
    A_UINT8 recommendedPrimeState;   /* 0 = regular (11a/11g); 1 = Turbo (Turbo A/108g) */
    A_UINT8 switchCount;        /* num consec frames sent at rate past the mode switch threshold */
    A_UINT8 maxValidRate;       /* maximum number of valid rate */
    A_UINT8 maxValidTurboRate;  /* maximum number of valid turbo rate */
    A_UINT8 validRateIndex[MAX_TX_RATE_TBL]; /* valid rate index */
    A_UINT32 perDownTime;       /* msec timstamp for last PER down step */

    /* 11n state */
    A_UINT8  validPhyRateCount[WLAN_RC_PHY_MAX]; /* valid rate count */
    A_UINT8  validPhyRateIndex[WLAN_RC_PHY_MAX][MAX_TX_RATE_TBL]; /* index */
    A_UINT8  rcPhyMode;
    A_UINT8  rateMaxPhy;        /* Phy index for the max rate */
    A_UINT32 rateMaxLastUsed;   /* msec timstamp of when we last used rateMaxPhy */
    A_UINT32 probeInterval;     /* interval for ratectrl to probe for other rates */

} TX_RATE_CTRL;

#ifdef ATH_SUPPORT_IQUE
typedef struct RateCtrlFunc {
    void    (*rcUpdate)(struct ath_softc *, struct ath_node *, A_RSSI, A_UINT8, int,
                        int, struct ath_rc_series rcs[], int, int, int);
    void    (*rcFind)(struct ath_softc *, struct ath_node *, A_UINT8, int, int,
                      unsigned int, struct ath_rc_series series[], int *, int);
} RATECTRL_FN;
#endif

struct atheros_softc;
struct atheros_vap;

/* per-node state */
struct atheros_node {
    TX_RATE_CTRL txRateCtrl;	/* rate control state proper */
#ifdef ATH_SUPPORT_IQUE
    TX_RATE_CTRL txRateCtrlViVo;	/* rate control state proper */

    RATECTRL_FN rcFunc[WME_NUM_AC]; /* rate control functions */
#endif

    A_UINT32 lastRateKbps;	/* last rate in Kb/s */

    /* map of rate ix -> negotiated rate set ix */
    A_UINT8 rixMap[MAX_TX_RATE_TBL];

    /* map of ht rate ix -> negotiated rate set ix */
    A_UINT8 htrixMap[MAX_TX_RATE_TBL];

    A_UINT16 htcap;		/* ht capabilities */
    A_UINT8 antTx;		/* current transmit antenna */

    A_UINT8 singleStream    :1,   /* When TRUE, only single stream Tx possible */
            stbc            :2;   /* Rx stbc capability */
    struct atheros_softc    *asc; /* back pointer to atheros softc */
    struct atheros_vap      *avp; /* back pointer to atheros vap */
#ifdef ATH_SUPPORT_IQUE
	struct ath_node			*an;
#endif	
};

/*
 * Rate Table structure for various modes - 'b', 'a', 'g', 'xr';
 * order of fields in info structure is important because hardcoded
 * structures are initialized within the hal for these
 */
#define RATE_TABLE_SIZE             32
typedef struct {
    int         rateCount;
    A_UINT8     rateCodeToIndex[RATE_TABLE_SIZE]; /* backward mapping */
    struct {
        int    valid;            /* Valid for use in rate control */
        WLAN_PHY  phy;              /* CCK/OFDM/TURBO/XR */
        A_UINT16  rateKbps;         /* Rate in Kbits per second */
        A_UINT16  userRateKbps;     /* User rate in KBits per second */
        A_UINT8   rateCode;         /* rate that goes into hw descriptors */
        A_UINT8   shortPreamble;    /* Mask for enabling short preamble in rate code for CCK */
        A_UINT8   dot11Rate;        /* Value that goes into supported rates info element of MLME */
        A_UINT8   controlRate;      /* Index of next lower basic rate, used for duration computation */
        A_RSSI    rssiAckValidMin;  /* Rate control related information */
        A_RSSI    rssiAckDeltaMin;  /* Rate control related information */
        A_UINT16  lpAckDuration;    /* long preamble ACK duration */
        A_UINT16  spAckDuration;    /* short preamble ACK duration*/
        A_UINT32  max4msFrameLen;   /* Maximum frame length(bytes) for 4ms tx duration */
        struct {
            A_UINT32  word4Retries;
            A_UINT32  word5Rates;
        } normalSched;
        struct {
            A_UINT32  word4Retries;
            A_UINT32  word5Rates;
        } shortSched;
        struct {
            A_UINT32  word4Retries;
            A_UINT32  word5Rates;
        } probeSched;
        struct {
            A_UINT32  word4Retries;
            A_UINT32  word5Rates;
        } probeShortSched;
    } info[32];
    A_UINT32    probeInterval;        /* interval for ratectrl to probe for
					 other rates */
    A_UINT32    rssiReduceInterval;   /* interval for ratectrl to reduce RSSI */
    A_UINT8     regularToTurboThresh; /* upperbound on regular (11a or 11g)
					 mode's rate before switching to turbo*/
    A_UINT8     turboToRegularThresh; /* lowerbound on turbo mode's rate before
					 switching to regular */
    A_UINT8     pktCountThresh;       /* mode switch recommendation criterion:
					 number of consecutive packets sent at
					 rate beyond the rate threshold */
    A_UINT8     initialRateMax;       /* the initial rateMax value used in
					 rcSibUpdate() */
    A_UINT8     numTurboRates;        /* number of Turbo rates in the rateTable */
    A_UINT8     xrToRegularThresh;    /* threshold to switch to Normal mode */
} RATE_TABLE;

/* per-vap state */
struct atheros_vap {
	const void        *rateTable;
    struct atheros_softc    *asc; /* backpointer */
    struct ath_vap          *athdev_vap; /* backpointer */
};

/* per-device state */
struct atheros_softc {
    /* phy tables that contain rate control data */
    const void *hwRateTable[WIRELESS_MODE_MAX];
    int        fixedrix;      /* -1 or index of fixed rate */
    u_int32_t  txTrigLevelMax;/* max transmit trigger level */
};

/*
 *  Update the SIB's rate control information
 *
 *  This should be called when the supported rates change
 *  (e.g. SME operation, wireless mode change)
 *
 *  It will determine which rates are valid for use.
 */
void
rcSibUpdate(struct atheros_softc *asc, struct atheros_node *pSib,
            int keepState, struct ieee80211_rateset *pRateSet,
            enum ieee80211_phymode curmode);

/*
 *  This routine is called to initialize the rate control parameters
 *  in the SIB. It is called initially during system initialization
 *  or when a station is associated with the AP.
 */
void
rcSibInit(struct atheros_node *pSib);

/*
 * Determines and returns the new Tx rate index.
 */
A_UINT16
rcRateFind(struct ath_softc *sc, struct atheros_node *,
           A_UINT32 frameLen,
           const RATE_TABLE *pRateTable,
           HAL_CHANNEL *chan, int isretry);

/*
 * This routine is called by the Tx interrupt service routine to give
 * the status of previous frames.
 */
void
rcUpdate(struct atheros_node *pSib,
         int Xretries, int txRate, int retries, A_RSSI rssiAck,
         A_UINT8 curTxAnt, const RATE_TABLE *pRateTable,
         enum ieee80211_opmode opmode,
         int diversity,
         SetDefAntenna_callback setDefAntenna, void *context);

void	atheros_setuptable(RATE_TABLE *rt);
void	ar5212SetupRateTables(void);
void	ar5212AttachRateTables(struct atheros_softc *sc);
void 	ar5212SetFullRateTable(struct atheros_softc *sc);
void 	ar5212SetHalfRateTable(struct atheros_softc *sc);
void 	ar5212SetQuarterRateTable(struct atheros_softc *sc);
void    ar5416SetupRateTables(void);
void    ar5416AttachRateTables(struct atheros_softc *sc);

#ifndef ATH_NO_5G_SUPPORT
void    ar5416SetFullRateTable(struct atheros_softc *sc);
void    ar5416SetHalfRateTable(struct atheros_softc *sc);
void    ar5416SetQuarterRateTable(struct atheros_softc *sc);
#endif

#define RATECTRL_MEMTAG 'Mtar'

#define RC_OS_MALLOC(_ppMem, _size, _tag)       OS_MALLOC_WITH_TAG(_ppMem, _size, _tag)
#define RC_OS_FREE(_pMem, _size)                OS_FREE_WITH_TAG(_pMem, _size)

#endif /* _ATH_RC_H_ */

#if ATH_CCX
u_int8_t
rcRateValueToPer(struct ath_softc *sc, struct ath_node *an, int txRateKbps);
#endif /* ATH_CCX */
