/*
 * Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
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
#ifndef _ATH_AR5416_H_
#define _ATH_AR5416_H_

#include "ah_eeprom.h"
#include "ah_devid.h"
#include "ar5416desc.h"
#include "ar5416eep.h"  /* For Eeprom definitions */

#define AR5416_MAGIC            0x19641014

#define AR_SREV_HOWL(ah) ((AH_PRIVATE(ah)->ah_macVersion) == AR_SREV_VERSION_HOWL)

/* DCU Transmit Filter macros */
#define CALC_MMR(dcu, idx) \
    ( (4 * dcu) + (idx < 32 ? 0 : (idx < 64 ? 1 : (idx < 96 ? 2 : 3))) )
#define TXBLK_FROM_MMR(mmr) \
    (AR_D_TXBLK_BASE + ((mmr & 0x1f) << 6) + ((mmr & 0x20) >> 3))
#define CALC_TXBLK_ADDR(dcu, idx)   (TXBLK_FROM_MMR(CALC_MMR(dcu, idx)))
#define CALC_TXBLK_VALUE(idx)       (1 << (idx & 0x1f))

/* MAC register values */

#define INIT_BEACON_CONTROL \
    ((INIT_RESET_TSF << 24)  | (INIT_BEACON_EN << 23) | \
      (INIT_TIM_OFFSET << 16) | INIT_BEACON_PERIOD)

#define INIT_CONFIG_STATUS  0x00000000
#define INIT_RSSI_THR       0x00000700  /* Missed beacon counter initialized to 0x7 (max is 0xff) */
#define INIT_BCON_CNTRL_REG 0x00000000

/*
 * Various fifo fill before Tx start, in 64-byte units
 * i.e. put the frame in the air while still DMAing
 */
#define MIN_TX_FIFO_THRESHOLD   0x1
#define MAX_TX_FIFO_THRESHOLD   (( 4096 / 64) - 1)
#define INIT_TX_FIFO_THRESHOLD  MIN_TX_FIFO_THRESHOLD

#define IS_SPUR_CHAN(_chan) \
    ( (((_chan)->channel % 32) != 0) && \
        ((((_chan)->channel % 32) < 10) || (((_chan)->channel % 32) > 22)) )

/*
 * Gain support.
 */
#define NUM_CORNER_FIX_BITS_2133    7
#define CCK_OFDM_GAIN_DELTA         15
#define MERLIN_TX_GAIN_TABLE_SIZE   22

enum GAIN_PARAMS {
    GP_TXCLIP,
    GP_PD90,
    GP_PD84,
    GP_GSEL
};

enum GAIN_PARAMS_2133 {
    GP_MIXGAIN_OVR,
    GP_PWD_138,
    GP_PWD_137,
    GP_PWD_136,
    GP_PWD_132,
    GP_PWD_131,
    GP_PWD_130,
};

enum {
    HAL_RESET_POWER_ON,
    HAL_RESET_WARM,
    HAL_RESET_COLD,
};

typedef struct _gainOptStep {
    int16_t paramVal[NUM_CORNER_FIX_BITS_2133];
    int32_t stepGain;
    int8_t  stepName[16];
} GAIN_OPTIMIZATION_STEP;

typedef struct {
    u_int32_t   numStepsInLadder;
    u_int32_t   defaultStepNum;
    GAIN_OPTIMIZATION_STEP optStep[10];
} GAIN_OPTIMIZATION_LADDER;

typedef struct {
    u_int32_t   currStepNum;
    u_int32_t   currGain;
    u_int32_t   targetGain;
    u_int32_t   loTrig;
    u_int32_t   hiTrig;
    u_int32_t   gainFCorrection;
    u_int32_t   active;
    const GAIN_OPTIMIZATION_STEP *currStep;
} GAIN_VALUES;

typedef struct {
    u_int16_t   synth_center;
    u_int16_t   ctl_center;
    u_int16_t   ext_center;
} CHAN_CENTERS;

/* RF HAL structures */
typedef struct RfHalFuncs {
    void      (*rfDetach)(struct ath_hal *ah);
    void      (*writeRegs)(struct ath_hal *,
            u_int modeIndex, u_int freqIndex, int regWrites);
    HAL_BOOL  (*setChannel)(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
    HAL_BOOL  (*setRfRegs)(struct ath_hal *,
              HAL_CHANNEL_INTERNAL *, u_int16_t modesIndex);
    HAL_BOOL  (*getChipPowerLim)(struct ath_hal *ah, HAL_CHANNEL *chans,
                       u_int32_t nchancs);
    void      (*decreaseChainPower)(struct ath_hal *ah, HAL_CHANNEL *chans);
} RF_HAL_FUNCS;

/*
 * Per-channel ANI state private to the driver.
 */
struct ar5416AniState {
    HAL_CHANNEL c;
    u_int8_t    noiseImmunityLevel;
    u_int8_t    spurImmunityLevel;
    u_int8_t    firstepLevel;
    u_int8_t    ofdmWeakSigDetectOff;
    u_int8_t    cckWeakSigThreshold;

    /* Thresholds */
    u_int32_t   listenTime;
    u_int32_t   ofdmTrigHigh;
    u_int32_t   ofdmTrigLow;
    int32_t     cckTrigHigh;
    int32_t     cckTrigLow;
    int32_t     rssiThrLow;
    int32_t     rssiThrHigh;

    u_int32_t   noiseFloor; /* The current noise floor */
    u_int32_t   txFrameCount;   /* Last txFrameCount */
    u_int32_t   rxFrameCount;   /* Last rx Frame count */
    u_int32_t   cycleCount; /* Last cycleCount (can detect wrap-around) */
    u_int32_t   ofdmPhyErrCount;/* OFDM err count since last reset */
    u_int32_t   cckPhyErrCount; /* CCK err count since last reset */
    u_int32_t   ofdmPhyErrBase; /* Base value for ofdm err counter */
    u_int32_t   cckPhyErrBase;  /* Base value for cck err counters */
    int16_t     pktRssi[2]; /* Average rssi of pkts for 2 antennas */
    int16_t     ofdmErrRssi[2]; /* Average rssi of ofdm phy errs for 2 ant */
    int16_t     cckErrRssi[2];  /* Average rssi of cck phy errs for 2 ant */
};

#define HAL_PROCESS_ANI     0x00000001  /* ANI state setup */
#define HAL_RADAR_EN        0x80000000  /* Radar detect is capable */
#define HAL_AR_EN       0x40000000  /* AR detect is capable */

#define DO_ANI(ah) \
    ((AH5416(ah)->ah_procPhyErr & HAL_PROCESS_ANI))

struct ar5416Stats {
    u_int32_t   ast_ani_niup;   /* ANI increased noise immunity */
    u_int32_t   ast_ani_nidown; /* ANI decreased noise immunity */
    u_int32_t   ast_ani_spurup; /* ANI increased spur immunity */
    u_int32_t   ast_ani_spurdown;/* ANI descreased spur immunity */
    u_int32_t   ast_ani_ofdmon; /* ANI OFDM weak signal detect on */
    u_int32_t   ast_ani_ofdmoff;/* ANI OFDM weak signal detect off */
    u_int32_t   ast_ani_cckhigh;/* ANI CCK weak signal threshold high */
    u_int32_t   ast_ani_ccklow; /* ANI CCK weak signal threshold low */
    u_int32_t   ast_ani_stepup; /* ANI increased first step level */
    u_int32_t   ast_ani_stepdown;/* ANI decreased first step level */
    u_int32_t   ast_ani_ofdmerrs;/* ANI cumulative ofdm phy err count */
    u_int32_t   ast_ani_cckerrs;/* ANI cumulative cck phy err count */
    u_int32_t   ast_ani_reset;  /* ANI parameters zero'd for non-STA */
    u_int32_t   ast_ani_lzero;  /* ANI listen time forced to zero */
    u_int32_t   ast_ani_lneg;   /* ANI listen time calculated < 0 */
    HAL_MIB_STATS   ast_mibstats;   /* MIB counter stats */
    HAL_NODE_STATS  ast_nodestats;  /* Latest rssi stats from driver */
};

struct ar5416RadReader {
    u_int16_t   rd_index;
    u_int16_t   rd_expSeq;
    u_int32_t   rd_resetVal;
    u_int8_t    rd_start;
};

struct ar5416RadWriter {
    u_int16_t   wr_index;
    u_int16_t   wr_seq;
};

struct ar5416RadarEvent {
    u_int32_t   re_ts;      /* 32 bit time stamp */
    u_int8_t    re_rssi;    /* rssi of radar event */
    u_int8_t    re_dur;     /* duration of radar pulse */
    u_int8_t    re_chanIndex;   /* Channel of event */
};

struct ar5416RadarQElem {
    u_int32_t   rq_seqNum;
    u_int32_t   rq_busy;        /* 32 bit to insure atomic read/write */
    struct ar5416RadarEvent rq_event;   /* Radar event */
};

struct ar5416RadarQInfo {
    u_int16_t   ri_qsize;       /* q size */
    u_int16_t   ri_seqSize;     /* Size of sequence ring */
    struct ar5416RadReader ri_reader;   /* State for the q reader */
    struct ar5416RadWriter ri_writer;   /* state for the q writer */
};

#define HAL_MAX_ACK_RADAR_DUR   511
#define HAL_MAX_NUM_PEAKS   3
#define HAL_ARQ_SIZE        4096        /* 8K AR events for buffer size */
#define HAL_ARQ_SEQSIZE     4097        /* Sequence counter wrap for AR */
#define HAL_RADARQ_SIZE     1024        /* 1K radar events for buffer size */
#define HAL_RADARQ_SEQSIZE  1025        /* Sequence counter wrap for radar */
#define HAL_NUMRADAR_STATES 64      /* Number of radar channels we keep state for */

struct ar5416ArState {
    u_int16_t   ar_prevTimeStamp;
    u_int32_t   ar_prevWidth;
    u_int32_t   ar_phyErrCount[HAL_MAX_ACK_RADAR_DUR];
    u_int32_t   ar_ackSum;
    u_int16_t   ar_peakList[HAL_MAX_NUM_PEAKS];
    u_int32_t   ar_packetThreshold; /* Thresh to determine traffic load */
    u_int32_t   ar_parThreshold;    /* Thresh to determine peak */
    u_int32_t   ar_radarRssi;       /* Rssi threshold for AR event */
};

struct ar5416RadarState {
    HAL_CHANNEL_INTERNAL *rs_chan;      /* Channel info */
    u_int8_t    rs_chanIndex;       /* Channel index in radar structure */
    u_int32_t   rs_numRadarEvents;  /* Number of radar events */
    int32_t     rs_firpwr;      /* Thresh to check radar sig is gone */
    u_int32_t   rs_radarRssi;       /* Thresh to start radar det (dB) */
    u_int32_t   rs_height;      /* Thresh for pulse height (dB)*/
    u_int32_t   rs_pulseRssi;       /* Thresh to check if pulse is gone (dB) */
    u_int32_t   rs_inband;      /* Thresh to check if pusle is inband (0.5 dB) */
};

#define AR5416_OPFLAGS_11A           0x01   /* if set, allow 11a */
#define AR5416_OPFLAGS_11G           0x02   /* if set, allow 11g */
#define AR5416_OPFLAGS_N_5G_HT40     0x04   /* if set, disable 5G HT40 */
#define AR5416_OPFLAGS_N_2G_HT40     0x08   /* if set, disable 2G HT40 */
#define AR5416_OPFLAGS_N_5G_HT20     0x10   /* if set, disable 5G HT20 */
#define AR5416_OPFLAGS_N_2G_HT20     0x20   /* if set, disable 2G HT20 */

#define AR5416_MAX_CHAINS            3
#define FREQ2FBIN(x,y) ((y) ? ((x) - 2300) : (((x) - 4800) / 5))

/* Support for multiple INIs */
struct ar5416IniArray {
    u_int32_t *ia_array;
    u_int32_t ia_rows;
    u_int32_t ia_columns;
};
#define INIT_INI_ARRAY(iniarray, array, rows, columns) do {             \
    (iniarray)->ia_array = (u_int32_t *)(array);    \
    (iniarray)->ia_rows = (rows);       \
    (iniarray)->ia_columns = (columns); \
} while (0)
#define INI_RA(iniarray, row, column) (((iniarray)->ia_array)[(row) * ((iniarray)->ia_columns) + (column)])

#define INIT_CAL(_perCal)   \
    (_perCal)->calState = CAL_WAITING;  \
    (_perCal)->calNext = AH_NULL;

#define INSERT_CAL(_ahp, _perCal)   \
do {                    \
    if ((_ahp)->ah_cal_list_last == AH_NULL) {  \
        (_ahp)->ah_cal_list = (_ahp)->ah_cal_list_last = (_perCal); \
        ((_ahp)->ah_cal_list_last)->calNext = (_perCal);    \
    } else {    \
        ((_ahp)->ah_cal_list_last)->calNext = (_perCal);    \
        (_ahp)->ah_cal_list_last = (_perCal);   \
        (_perCal)->calNext = (_ahp)->ah_cal_list;   \
    }   \
} while (0)
 
typedef enum cal_types {
    ADC_DC_INIT_CAL = 0x1,
    ADC_GAIN_CAL    = 0x2,
    ADC_DC_CAL      = 0x4,
    IQ_MISMATCH_CAL = 0x8
} HAL_CAL_TYPES;

typedef enum cal_state {
    CAL_INACTIVE,
    CAL_WAITING,
    CAL_RUNNING,
    CAL_DONE
} HAL_CAL_STATE;            /* Calibrate state */

#define MIN_CAL_SAMPLES     1
#define MAX_CAL_SAMPLES    64
#define INIT_LOG_COUNT      5
#define PER_MIN_LOG_COUNT   2
#define PER_MAX_LOG_COUNT  10

/* Per Calibration data structure */
typedef struct per_cal_data {
    HAL_CAL_TYPES calType;           // Type of calibration
    u_int32_t     calNumSamples;     // Number of SW samples to collect
    u_int32_t     calCountMax;       // Number of HW samples to collect
    void (*calCollect)(struct ath_hal *);  // Accumulator func
    void (*calPostProc)(struct ath_hal *, u_int8_t); // Post-processing func
} HAL_PERCAL_DATA;

/* List structure for calibration data */
typedef struct cal_list {
    const HAL_PERCAL_DATA  *calData;
    HAL_CAL_STATE          calState;
    struct cal_list        *calNext;
} HAL_CAL_LIST;

struct ath_hal_5416 {
    struct ath_hal_private  ah_priv;    /* base class */

    /*
     * Information retrieved from EEPROM.
     */
    ar5416_eeprom_t  ah_eeprom;

    GAIN_VALUES ah_gainValues;

    u_int8_t    ah_macaddr[IEEE80211_ADDR_LEN];
    u_int8_t    ah_bssid[IEEE80211_ADDR_LEN];
    u_int8_t    ah_bssidmask[IEEE80211_ADDR_LEN];
    u_int16_t   ah_assocId;

    int16_t     ah_curchanRadIndex; /* cur. channel radar index */

    /*
     * Runtime state.
     */
    u_int32_t   ah_maskReg;     /* copy of AR_IMR */
    struct ar5416Stats ah_stats;        /* various statistics */
    RF_HAL_FUNCS    ah_rfHal;
    u_int32_t   ah_txDescMask;      /* mask for TXDESC */
    u_int32_t   ah_txOkInterruptMask;
    u_int32_t   ah_txErrInterruptMask;
    u_int32_t   ah_txDescInterruptMask;
    u_int32_t   ah_txEolInterruptMask;
    u_int32_t   ah_txUrnInterruptMask;
    HAL_TX_QUEUE_INFO ah_txq[HAL_NUM_TX_QUEUES];
    HAL_POWER_MODE  ah_powerMode;
    HAL_SMPS_MODE   ah_smPowerMode;
    HAL_BOOL    ah_chipFullSleep;
    u_int32_t   ah_atimWindow;
    HAL_ANT_SETTING ah_diversityControl;    /* antenna setting */
    u_int16_t   ah_antennaSwitchSwap;       /* Controls mapping of OID request */
    u_int8_t    ah_tx_chainmask_cfg;        /* chain mask config */
    u_int8_t    ah_rx_chainmask_cfg;
    /* Calibration related fields */
    HAL_CAL_TYPES ah_suppCals;
    HAL_CAL_LIST  ah_iqCalData;         /* IQ Cal Data */
    HAL_CAL_LIST  ah_adcGainCalData;    /* ADC Gain Cal Data */
    HAL_CAL_LIST  ah_adcDcCalInitData;  /* Init ADC DC Offset Cal Data */
    HAL_CAL_LIST  ah_adcDcCalData;      /* Periodic ADC DC Offset Cal Data */
    HAL_CAL_LIST  *ah_cal_list;         /* ptr to first cal in list */
    HAL_CAL_LIST  *ah_cal_list_last;    /* ptr to last cal in list */
    HAL_CAL_LIST  *ah_cal_list_curr;    /* ptr to current cal */
// IQ Cal aliases
#define ah_totalPowerMeasI ah_Meas0.unsign
#define ah_totalPowerMeasQ ah_Meas1.unsign
#define ah_totalIqCorrMeas ah_Meas2.sign
// Adc Gain Cal aliases
#define ah_totalAdcIOddPhase  ah_Meas0.unsign
#define ah_totalAdcIEvenPhase ah_Meas1.unsign
#define ah_totalAdcQOddPhase  ah_Meas2.unsign
#define ah_totalAdcQEvenPhase ah_Meas3.unsign
// Adc DC Offset Cal aliases
#define ah_totalAdcDcOffsetIOddPhase  ah_Meas0.sign
#define ah_totalAdcDcOffsetIEvenPhase ah_Meas1.sign
#define ah_totalAdcDcOffsetQOddPhase  ah_Meas2.sign
#define ah_totalAdcDcOffsetQEvenPhase ah_Meas3.sign
    union {
        u_int32_t   unsign[AR5416_MAX_CHAINS];
        int32_t     sign[AR5416_MAX_CHAINS];
    } ah_Meas0;
    union {
        u_int32_t   unsign[AR5416_MAX_CHAINS];
        int32_t     sign[AR5416_MAX_CHAINS];
    } ah_Meas1;
    union {
        u_int32_t   unsign[AR5416_MAX_CHAINS];
        int32_t     sign[AR5416_MAX_CHAINS];
    } ah_Meas2;
    union {
        u_int32_t   unsign[AR5416_MAX_CHAINS];
        int32_t     sign[AR5416_MAX_CHAINS];
    } ah_Meas3;
    u_int16_t   ah_CalSamples;
    /* end - Calibration related fields */
    u_int32_t   ah_tx6PowerInHalfDbm;   /* power output for 6Mb tx */
    u_int32_t   ah_staId1Defaults;  /* STA_ID1 default settings */
    u_int32_t   ah_miscMode;        /* MISC_MODE settings */
    HAL_BOOL    ah_tpcEnabled;      /* per-packet tpc enabled */
    u_int32_t   ah_beaconInterval;  /* XXX */
    enum {
        AUTO_32KHZ,     /* use it if 32kHz crystal present */
        USE_32KHZ,      /* do it regardless */
        DONT_USE_32KHZ,     /* don't use it regardless */
    } ah_enable32kHzClock;          /* whether to sleep at 32kHz */
    u_int32_t   *ah_analogBank0Data;/* RF register banks */
    u_int32_t   *ah_analogBank1Data;
    u_int32_t   *ah_analogBank2Data;
    u_int32_t   *ah_analogBank3Data;
    u_int32_t   *ah_analogBank6Data;
    u_int32_t   *ah_analogBank6TPCData;
    u_int32_t   *ah_analogBank7Data;

    u_int32_t   *ah_addacOwl21;     /* temporary register arrays */
    u_int32_t   *ah_bank6Temp;

    u_int32_t   ah_ofdmTxPower;
    int16_t     ah_txPowerIndexOffset;

    u_int       ah_slottime;        /* user-specified slot time */
    u_int       ah_acktimeout;      /* user-specified ack timeout */
    u_int       ah_ctstimeout;      /* user-specified cts timeout */
    u_int       ah_globaltxtimeout; /* user-specified global tx timeout */
    /*
     * XXX
     * 11g-specific stuff; belongs in the driver.
     */
    u_int8_t    ah_gBeaconRate;     /* fixed rate for G beacons */
    u_int32_t   ah_gpioMask;        /* copy of enabled GPIO mask */
    /*
     * RF Silent handling; setup according to the EEPROM.
     */
    u_int32_t   ah_gpioSelect;      /* GPIO pin to use */
    u_int32_t   ah_polarity;        /* polarity to disable RF */
    u_int32_t   ah_gpioBit;     /* after init, prev value */
    HAL_BOOL    ah_eepEnabled;      /* EEPROM bit for capability */

#ifdef ATH_BT_COEX
    /*
     * Bluetooth coexistence static setup according to the EEPROM
     */
    HAL_BT_MODULE ah_btModule;           /* Bluetooth module identifier */
    u_int8_t    ah_btActiveGpioSelect;   /* GPIO pin for BT_ACTIVE */
    u_int8_t    ah_btPriorityGpioSelect; /* GPIO pin for BT_PRIORITY */
    u_int8_t    ah_wlanActiveGpioSelect; /* GPIO pin for WLAN_ACTIVE */
    u_int8_t    ah_btActivePolarity;     /* Polarity of BT_ACTIVE */
    /*
     * Bluetooth coexistence runtime settings
     */
    HAL_BOOL    ah_btCoexEnabled;        /* If Bluetooth coexistence is enabled */
    u_int32_t   ah_btCoexMode;           /* Register setting for AR_BT_COEX_MODE */
    u_int32_t   ah_btCoexWeights;        /* Register setting for AR_BT_COEX_WEIGHT */
    u_int32_t   ah_btCoexMode2;          /* Register setting for AR_BT_COEX_MODE2 */
#endif

    /*
     * Generic timer support
     */
    u_int32_t   ah_intrGenTimerTrigger;  /* generic timer trigger interrupt state */
    u_int32_t   ah_intrGenTimerThresh;   /* generic timer trigger interrupt state */

    /*
     * ANI & Radar support.
     */
    u_int32_t   ah_procPhyErr;      /* Process Phy errs */
    HAL_BOOL    ah_hasHwPhyCounters;    /* Hardware has phy counters */
    u_int32_t   ah_aniPeriod;       /* ani update list period */
    struct ar5416AniState   *ah_curani; /* cached last reference */
    struct ar5416AniState   ah_ani[255]; /* per-channel state */
    struct ar5416RadarState ah_radar[HAL_NUMRADAR_STATES];  /* Per-Channel Radar detector state */
    struct ar5416RadarQElem *ah_radarq; /* radar event queue */
    struct ar5416RadarQInfo ah_radarqInfo;  /* radar event q read/write state */
    struct ar5416ArState    ah_ar;      /* AR detector state */
    struct ar5416RadarQElem *ah_arq;    /* AR event queue */
    struct ar5416RadarQInfo ah_arqInfo; /* AR event q read/write state */

    /*
     * Ani tables that change between the 5416 and 5312.
     * These get set at attach time.
     * XXX don't belong here
     * XXX need better explanation
     */
        int     ah_totalSizeDesired[5];
        int     ah_coarseHigh[5];
        int     ah_coarseLow[5];
        int     ah_firpwr[5];

    /*
     * Transmit power state.  Note these are maintained
     * here so they can be retrieved by diagnostic tools.
     */
    u_int16_t   ah_ratesArray[16];

    /*
     * Tx queue interrupt state.
     */
    u_int32_t   ah_intrTxqs;
    HAL_BOOL    ah_intrMitigationRx; /* rx Interrupt Mitigation Settings */
    HAL_BOOL    ah_intrMitigationTx; /* tx Interrupt Mitigation Settings */

    HAL_BOOL    ah_intrMitigation;      /* Interrupt Mitigation Settings */

    /*
     * Extension Channel Rx Clear State
     */
    u_int32_t   ah_cycleCount;
    u_int32_t   ah_ctlBusy;
    u_int32_t   ah_extBusy;

    /* HT CWM state */
    HAL_HT_EXTPROTSPACING ah_extprotspacing;
    u_int8_t    ah_txchainmask; /* tx chain mask */
    u_int8_t    ah_rxchainmask; /* rx chain mask */

    int         ah_hwp;
    void        *ah_cal_mem;
    HAL_BOOL    ah_emu_eeprom;

    HAL_ANI_CMD ah_ani_function;
    HAL_BOOL    ah_rifs_enabled;
    u_int32_t   ah_rifs_reg[11];
    u_int32_t   ah_rifs_sec_cnt;

    /* open-loop power control */
    u_int32_t originalGain[22];
    int32_t   initPDADC;
    int32_t   PDADCdelta;

    /* cycle counts for beacon stuck diagnostics */
    u_int32_t   ah_cycles;          
    u_int32_t   ah_rx_clear;
    u_int32_t   ah_rx_frame;
    u_int32_t   ah_tx_frame;

#define BB_HANG_SIG1 0 
#define BB_HANG_SIG2 1 
#define BB_HANG_SIG3 2 
#define BB_HANG_SIG4 3 
#define BB_HANG_SIG5 4
#define BB_HANG_SIG6 5
#define MAC_HANG_SIG1 6
#define MAC_HANG_SIG2 7 
    /* bb hang detection */
    int		ah_hang[8];
    hal_hw_hangs_t  ah_hang_wars;
    /*
     * Support for ar5416 multiple INIs
     */
    struct ar5416IniArray ah_iniModes;
    struct ar5416IniArray ah_iniCommon;
    struct ar5416IniArray ah_iniBank0;
    struct ar5416IniArray ah_iniBB_RfGain;
    struct ar5416IniArray ah_iniBank1;
    struct ar5416IniArray ah_iniBank2;
    struct ar5416IniArray ah_iniBank3;
    struct ar5416IniArray ah_iniBank6;
    struct ar5416IniArray ah_iniBank6TPC;
    struct ar5416IniArray ah_iniBank7;
    struct ar5416IniArray ah_iniAddac;
    struct ar5416IniArray ah_iniPcieSerdes;
    struct ar5416IniArray ah_iniModesAdditional;
    struct ar5416IniArray ah_iniModesRxgain;
    struct ar5416IniArray ah_iniModesTxgain;
#if ATH_WOW
    struct ar5416IniArray ah_iniPcieSerdesWow;  /* SerDes values during WOW sleep */
#endif

    /* To indicate EEPROM mapping used */
    HAL_5416_EEP_MAP ah_eep_map;

    /* filled out Vpd table for all pdGains (chanL) */
    u_int8_t   ah_vpdTableL[AR5416_EEP4K_NUM_PD_GAINS][AR5416_MAX_PWR_RANGE_IN_HALF_DB];
    /* filled out Vpd table for all pdGains (chanR) */
    u_int8_t   ah_vpdTableR[AR5416_EEP4K_NUM_PD_GAINS][AR5416_MAX_PWR_RANGE_IN_HALF_DB];
    /* filled out Vpd table for all pdGains (interpolated) */
    u_int8_t   ah_vpdTableI[AR5416_EEP4K_NUM_PD_GAINS][AR5416_MAX_PWR_RANGE_IN_HALF_DB];
    u_int32_t ah_immunity[6];
    HAL_BOOL ah_immunity_on;
    /*
     * snap shot of counter register for debug purposes
     */
#ifdef AH_DEBUG
    u_int32_t lasttf;
    u_int32_t lastrf;
    u_int32_t lastrc;
    u_int32_t lastcc;
#endif
};

#define AH5416(_ah) ((struct ath_hal_5416 *)(_ah))

#define IS_5416_EMU(ah) \
    ((AH_PRIVATE(ah)->ah_devid == AR5416_DEVID_EMU) || \
     (AH_PRIVATE(ah)->ah_devid == AR5416_DEVID_EMU_PCIE))

#define ar5416RfDetach(ah) do {             \
    if (AH5416(ah)->ah_rfHal.rfDetach != AH_NULL)   \
        AH5416(ah)->ah_rfHal.rfDetach(ah);  \
} while (0)

#define ar5416EepDataInFlash(_ah) \
    (!(AH_PRIVATE(_ah)->ah_flags & AH_USE_EEPROM))

#define IS_5GHZ_FAST_CLOCK_EN(_ah, _c)  (IS_CHAN_5GHZ(_c) && \
                                           ((AH_PRIVATE(_ah))->ah_config.ath_hal_fastClockEnable) && \
                                           ((ar5416EepromGet(AH5416(_ah), EEP_MINOR_REV) <= AR5416_EEP_MINOR_VER_16) || \
                                           (ar5416EepromGet(AH5416(_ah), EEP_FSTCLK_5G))))

/*
 * WAR for bug 6773.  OS_DELAY() does a PIO READ on the PCI bus which allows
 * other cards' DMA reads to complete in the middle of our reset.
 */
#define WAR_6773(x) do {                \
        if ((++(x) % 64) == 0)          \
                OS_DELAY(1);            \
} while (0)

/*
 *  There are two methods to fix PCI write unreliable to analog register
 *    space (0x7800- 0x7900):
 *    1. Use analog long shift register.
 *    2. Use delay.
 *    but, the new value can not be written correcly by using method 1. So
 *      we select method 2 to work around this issue. (Reported Bug:32840)
 */
#define WAR_32840(_ah, _reg) do {			\
	if ((AR_SREV_MERLIN_10_OR_LATER(_ah)) && 	\
		((_reg) >= 0x00007800 &&		\
		(_reg) < 0x00007900))			\
		OS_DELAY(100);				\
} while (0)

#define REG_WRITE_ARRAY(iniarray, column, regWr) do {                   \
        int r;                                                          \
        for (r = 0; r < ((iniarray)->ia_rows); r++) {    \
                OS_REG_WRITE(ah, INI_RA((iniarray), (r), 0), INI_RA((iniarray), r, (column)));\
                WAR_6773(regWr);                                        \
        }                                                               \
} while (0)

/******************************************************************************/
/*!
**  \brief Analog Shift Register Read-Modify-Write
**
**  This routine will either perform the standard RMW procedure, or apply the
**  Merlin specific write process depending on the setting of the configuration
**  parameter.  For AP it's on by default, for Station it's controlled by
**  registry parameters.
**
**  \param ah   HAL instance variable
**  \param reg  register offset to program
**  \param val  value to insert into the register.
**  \return N/A
*/
static inline void
analogShiftRegRMW(struct ath_hal *ah, u_int reg, u_int32_t mask,
                  u_int32_t shift, u_int32_t val)
{
    u_int32_t               regVal;
    struct ath_hal_private  *ap = AH_PRIVATE(ah);

    /*
    ** Code Begins
    ** Get the register value, and mask the appropriate bits
    */

     regVal = OS_REG_READ(ah,reg) & ~mask;

     /*
     ** Insert the value into the appropriate slot
     */

     regVal |= (val << shift) & mask;

     /*
     ** Determine if we use standard write or the "magic" write
     */

     OS_REG_WRITE(ah,reg,regVal);

     if (ap->ah_config.ath_hal_analogShiftRegWAR) {
        OS_DELAY(100);
     }

    return;
}

extern  HAL_BOOL ar2133RfAttach(struct ath_hal *, HAL_STATUS *);

struct ath_hal;

extern  u_int32_t ar5416RadioAttach(struct ath_hal *ah);
extern  struct ath_hal_5416 * ar5416NewState(u_int16_t devid,
        HAL_ADAPTER_HANDLE osdev, HAL_SOFTC sc,
        HAL_BUS_TAG st, HAL_BUS_HANDLE sh, HAL_BUS_TYPE bustype,
    HAL_STATUS *status);
extern  struct ath_hal * ar5416Attach(u_int16_t devid,
        HAL_ADAPTER_HANDLE osdev, HAL_SOFTC sc,
        HAL_BUS_TAG st, HAL_BUS_HANDLE sh, HAL_BUS_TYPE bustype, 
        struct hal_reg_parm *hal_conf_parm,
        HAL_STATUS *status);
extern  void ar5416Detach(struct ath_hal *ah);
extern void ar5416ReadRevisions(struct ath_hal *ah);
extern  HAL_BOOL ar5416ChipTest(struct ath_hal *ah);
extern  HAL_BOOL ar5416GetChannelEdges(struct ath_hal *ah,
                u_int16_t flags, u_int16_t *low, u_int16_t *high);
extern  HAL_BOOL ar5416FillCapabilityInfo(struct ath_hal *ah);

extern  void ar5416BeaconInit(struct ath_hal *ah,
        u_int32_t next_beacon, u_int32_t beacon_period);
extern  HAL_BOOL ar5416WaitForBeaconDone(struct ath_hal *, HAL_BUS_ADDR baddr);
extern  void ar5416ResetStaBeaconTimers(struct ath_hal *ah);
extern  void ar5416SetStaBeaconTimers(struct ath_hal *ah,
        const HAL_BEACON_STATE *);

extern  HAL_BOOL ar5416IsInterruptPending(struct ath_hal *ah);
extern  HAL_BOOL ar5416GetPendingInterrupts(struct ath_hal *ah, HAL_INT *);
extern  HAL_INT ar5416GetInterrupts(struct ath_hal *ah);
extern  HAL_INT ar5416SetInterrupts(struct ath_hal *ah, HAL_INT ints);
extern  void ar5416SetIntrMitigationTimer(struct ath_hal* ah,
        HAL_INT_MITIGATION reg, u_int32_t value);
extern  u_int32_t ar5416GetIntrMitigationTimer(struct ath_hal* ah,
        HAL_INT_MITIGATION reg);
extern  u_int32_t ar5416GetKeyCacheSize(struct ath_hal *);
extern  HAL_BOOL ar5416IsKeyCacheEntryValid(struct ath_hal *, u_int16_t entry);
extern void ar5416DivAnt(struct ath_hal *ah, int value);
extern  HAL_BOOL ar5416ResetKeyCacheEntry(struct ath_hal *ah, u_int16_t entry);
extern  HAL_BOOL ar5416SetKeyCacheEntryMac(struct ath_hal *,
            u_int16_t entry, const u_int8_t *mac);
extern  HAL_BOOL ar5416SetKeyCacheEntry(struct ath_hal *ah, u_int16_t entry,
                       const HAL_KEYVAL *k, const u_int8_t *mac, int xorKey);

extern  void ar5416GetMacAddress(struct ath_hal *ah, u_int8_t *mac);
extern  HAL_BOOL ar5416SetMacAddress(struct ath_hal *ah, const u_int8_t *);
extern  void ar5416GetBssIdMask(struct ath_hal *ah, u_int8_t *mac);
extern  HAL_BOOL ar5416SetBssIdMask(struct ath_hal *, const u_int8_t *);
extern  u_int8_t ar5416GetNumAntConfig(struct ath_hal_5416 *ahp, HAL_FREQ_BAND freq_band);
extern  HAL_STATUS ar5416EepromGetAntCfg(struct ath_hal_5416 *ahp, HAL_CHANNEL_INTERNAL *chan,
                                     u_int8_t index, u_int32_t *config);
extern  HAL_STATUS ar5416SelectAntConfig(struct ath_hal *ah, u_int32_t cfg);
extern  int ar5416GetNumAntCfg( struct ath_hal *ah);
extern  u_int32_t  ar5416GetMacVersion(struct ath_hal *ah);
extern  HAL_BOOL ar5416SetRegulatoryDomain(struct ath_hal *ah,
                                    u_int16_t regDomain, HAL_STATUS *stats);
extern  u_int ar5416GetWirelessModes(struct ath_hal *ah);
extern  void ar5416EnableRfKill(struct ath_hal *);
extern  HAL_BOOL ar5416GpioCfgOutput(struct ath_hal *, u_int32_t gpio, HAL_GPIO_OUTPUT_MUX_TYPE signalType);
extern  HAL_BOOL ar5416GpioCfgInput(struct ath_hal *, u_int32_t gpio);
extern  HAL_BOOL ar5416GpioSet(struct ath_hal *, u_int32_t gpio, u_int32_t val);
extern  u_int32_t ar5416GpioGet(struct ath_hal *ah, u_int32_t gpio);
extern  void ar5416GpioSetIntr(struct ath_hal *ah, u_int, u_int32_t ilevel);
extern  void ar5416SetLedState(struct ath_hal *ah, HAL_LED_STATE state);
extern  void ar5416SetPowerLedState(struct ath_hal *ah, u_int8_t enable);
extern  void ar5416SetNetworkLedState(struct ath_hal *ah, u_int8_t enable);
extern  void ar5416WriteAssocid(struct ath_hal *ah, const u_int8_t *bssid,
        u_int16_t assocId);
extern  u_int32_t ar5416PpmGetRssiDump(struct ath_hal *);
extern  u_int32_t ar5416PpmArmTrigger(struct ath_hal *);
extern  int ar5416PpmGetTrigger(struct ath_hal *);
extern  u_int32_t ar5416PpmForce(struct ath_hal *);
extern  void ar5416PpmUnForce(struct ath_hal *);
extern  u_int32_t ar5416PpmGetForceState(struct ath_hal *);
extern  u_int32_t ar5416PpmGetForceState(struct ath_hal *);
extern  u_int32_t ar5416GetTsf32(struct ath_hal *ah);
extern  u_int64_t ar5416GetTsf64(struct ath_hal *ah);
extern  void ar5416ResetTsf(struct ath_hal *ah);
extern  void ar5416SetBasicRate(struct ath_hal *ah, HAL_RATE_SET *pSet);
extern  u_int32_t ar5416GetRandomSeed(struct ath_hal *ah);
extern  HAL_BOOL ar5416DetectCardPresent(struct ath_hal *ah);
extern  void ar5416UpdateMibMacStats(struct ath_hal *ah);
extern  void ar5416GetMibMacStats(struct ath_hal *ah, HAL_MIB_STATS* stats);
extern  HAL_BOOL ar5416IsJapanChannelSpreadSupported(struct ath_hal *ah);
extern  u_int32_t ar5416GetCurRssi(struct ath_hal *ah);
extern  u_int ar5416GetDefAntenna(struct ath_hal *ah);
extern  void ar5416SetDefAntenna(struct ath_hal *ah, u_int antenna);
extern  HAL_BOOL ar5416SetAntennaSwitch(struct ath_hal *ah,
        HAL_ANT_SETTING settings, HAL_CHANNEL *chan, u_int8_t *, u_int8_t *, u_int8_t *);
extern  HAL_BOOL ar5416IsSleepAfterBeaconBroken(struct ath_hal *ah);
extern  HAL_BOOL ar5416SetSlotTime(struct ath_hal *, u_int);
extern  u_int ar5416GetSlotTime(struct ath_hal *);
extern  HAL_BOOL ar5416SetAckTimeout(struct ath_hal *, u_int);
extern  u_int ar5416GetAckTimeout(struct ath_hal *);
extern  HAL_BOOL ar5416SetCTSTimeout(struct ath_hal *, u_int);
extern  HAL_STATUS ar5416SetQuiet(struct ath_hal *ah,u_int16_t period, u_int16_t duration, u_int16_t nextStart, u_int16_t enabled);
extern  u_int ar5416GetCTSTimeout(struct ath_hal *);
extern  void ar5416SetPCUConfig(struct ath_hal *);
extern  HAL_STATUS ar5416GetCapability(struct ath_hal *, HAL_CAPABILITY_TYPE,
        u_int32_t, u_int32_t *);
extern  HAL_BOOL ar5416SetCapability(struct ath_hal *, HAL_CAPABILITY_TYPE,
        u_int32_t, u_int32_t, HAL_STATUS *);
extern  HAL_BOOL ar5416GetDiagState(struct ath_hal *ah, int request,
        const void *args, u_int32_t argsize,
        void **result, u_int32_t *resultsize);
extern void ar5416GetDescInfo(struct ath_hal *ah, HAL_DESC_INFO *desc_info);
extern  u_int32_t ar5416Get11nExtBusy(struct ath_hal *ah);
extern  void ar5416Set11nMac2040(struct ath_hal *ah, HAL_HT_MACMODE mode);
extern  HAL_HT_RXCLEAR ar5416Get11nRxClear(struct ath_hal *ah);
extern  void ar5416Set11nRxClear(struct ath_hal *ah, HAL_HT_RXCLEAR rxclear);
extern  HAL_BOOL ar5416SetPowerMode(struct ath_hal *ah, HAL_POWER_MODE mode,
        int setChip);
extern  HAL_POWER_MODE ar5416GetPowerMode(struct ath_hal *ah);
extern HAL_BOOL ar5416SetPowerModeAwake(struct ath_hal *ah, int setChip);
extern  void ar5416SetSmPowerMode(struct ath_hal *ah, HAL_SMPS_MODE mode);

extern void ar5416ConfigPciPowerSave(struct ath_hal *ah, int restore);

#if ATH_WOW
extern  void ar5416WowApplyPattern(struct ath_hal *ah, u_int8_t *pAthPattern, u_int8_t *pAthMask, int32_t pattern_count, u_int32_t athPatternLen);
extern  u_int32_t ar5416WowWakeUp(struct ath_hal *ah);
extern  HAL_BOOL ar5416WowEnable(struct ath_hal *ah, u_int32_t patternEnable);
#endif

extern HAL_BOOL ar5416Reset(struct ath_hal *ah, HAL_OPMODE opmode, HAL_CHANNEL *chan, HAL_HT_MACMODE macmode, u_int8_t txchainmask, u_int8_t rxchainmask, HAL_HT_EXTPROTSPACING extprotspacing, HAL_BOOL bChannelChange, HAL_STATUS *status);

extern  HAL_BOOL ar5416SetResetReg(struct ath_hal *ah, u_int32_t type);
extern  void ar5416SetOperatingMode(struct ath_hal *ah, int opmode);
extern  HAL_BOOL ar5416PhyDisable(struct ath_hal *ah);
extern  HAL_BOOL ar5416Disable(struct ath_hal *ah);
extern  HAL_BOOL ar5416ChipReset(struct ath_hal *ah, HAL_CHANNEL *);
extern  HAL_BOOL ar5416Calibration(struct ath_hal *ah,  HAL_CHANNEL *chan, 
        u_int8_t rxchainmask, HAL_BOOL longcal, HAL_BOOL *isIQdone);
extern  void ar5416ResetCalValid(struct ath_hal *ah,  HAL_CHANNEL *chan,
                                 HAL_BOOL *isIQdone);
extern void ar5416IQCalCollect(struct ath_hal *ah);
extern void ar5416IQCalibration(struct ath_hal *ah, u_int8_t numChains);
extern void ar5416AdcGainCalCollect(struct ath_hal *ah);
extern void ar5416AdcGainCalibration(struct ath_hal *ah, u_int8_t numChains);
extern void ar5416AdcDcCalCollect(struct ath_hal *ah);
extern void ar5416AdcDcCalibration(struct ath_hal *ah, u_int8_t numChains);
extern  void ar5416GetNoiseFloor(struct ath_hal *ah, int16_t nfarray[]);
extern  HAL_BOOL ar5416SetTxPowerLimit(struct ath_hal *ah, u_int32_t limit, u_int16_t tpcInDb);

extern  HAL_RFGAIN ar5416GetRfgain(struct ath_hal *ah);
extern  const HAL_RATE_TABLE *ar5416GetRateTable(struct ath_hal *, u_int mode);

extern  void ar5416EnableMIBCounters(struct ath_hal *);
extern  void ar5416DisableMIBCounters(struct ath_hal *);
extern  void ar5416AniAttach(struct ath_hal *);
extern  void ar5416AniDetach(struct ath_hal *);
extern  struct ar5416AniState *ar5416AniGetCurrentState(struct ath_hal *);
extern  struct ar5416Stats *ar5416AniGetCurrentStats(struct ath_hal *);
extern  HAL_BOOL ar5416AniControl(struct ath_hal *, HAL_ANI_CMD cmd, int param);
struct ath_rx_status;
extern  void ar5416AniPhyErrReport(struct ath_hal *ah,
        const struct ath_rx_status *rs);

extern  void ar5416ProcessMibIntr(struct ath_hal *, const HAL_NODE_STATS *);
extern  void ar5416AniArPoll(struct ath_hal *, const HAL_NODE_STATS *,
                 HAL_CHANNEL *);
extern  void ar5416AniReset(struct ath_hal *);

/* DFS declarations */

extern  void ar5416CheckDfs(struct ath_hal *ah, HAL_CHANNEL *chan);
extern  void ar5416DfsFound(struct ath_hal *ah, HAL_CHANNEL *chan, u_int64_t nolTime);
extern  void ar5416EnableDfs(struct ath_hal *ah, HAL_PHYERR_PARAM *pe); 
extern  void ar5416GetDfsThresh(struct ath_hal *ah, HAL_PHYERR_PARAM *pe);
extern  HAL_BOOL ar5416RadarWait(struct ath_hal *ah, HAL_CHANNEL *chan);
extern struct dfs_pulse * ar5416GetDfsRadars(struct ath_hal *ah, u_int32_t dfsdomain, int *numradars, struct dfs_bin5pulse **bin5pulses, int *numb5radars, HAL_PHYERR_PARAM *pe);
extern HAL_CHANNEL* ar5416GetExtensionChannel(struct ath_hal *ah);
extern void ar5416MarkPhyInactive(struct ath_hal *ah);
extern HAL_BOOL ar5416IsFastClockEnabled(struct ath_hal *ah);

extern  HAL_BOOL ar5416Reset11n(struct ath_hal *ah, HAL_OPMODE opmode,
        HAL_CHANNEL *chan, HAL_BOOL bChannelChange, HAL_STATUS *status);
extern void ar5416SetCoverageClass(struct ath_hal *ah, u_int8_t coverageclass, int now);

extern int ar5416Get11nHwPlatform(struct ath_hal *ah);
extern void ar5416GetChannelCenters(struct ath_hal *ah,
                                    HAL_CHANNEL_INTERNAL *chan,
                                    CHAN_CENTERS *centers);
extern u_int16_t ar5416GetCtlCenter(struct ath_hal *ah,
                                        HAL_CHANNEL_INTERNAL *chan);
extern u_int16_t ar5416GetExtCenter(struct ath_hal *ah,
                                        HAL_CHANNEL_INTERNAL *chan);
extern u_int32_t ar5416GetMibCycleCountsPct(struct ath_hal *, u_int32_t*, u_int32_t*, u_int32_t*);

extern void ar5416DmaRegDump(struct ath_hal *);
extern HAL_BOOL ar5416Set11nRxRifs(struct ath_hal *ah, HAL_BOOL enable);
extern HAL_BOOL ar5416Get11nRxRifs(struct ath_hal *ah);
extern HAL_BOOL ar5416SetRifsDelay(struct ath_hal *ah, HAL_BOOL enable);
extern HAL_BOOL ar5416DetectBbHang(struct ath_hal *ah);
extern HAL_BOOL ar5416DetectMacHang(struct ath_hal *ah);

#ifdef ATH_BT_COEX
extern void ar5416GetBTCoexInfo(struct ath_hal *ah, HAL_BT_COEX_INFO *btinfo);
extern void ar5416BTCoexConfig(struct ath_hal *ah, HAL_BT_COEX_CONFIG *btconf);
extern void ar5416BTCoexSetQcuThresh(struct ath_hal *ah, int qnum);
extern void ar5416BTCoexSetWeights(struct ath_hal *ah, u_int32_t bt_weight, u_int32_t wlan_weight);
extern void ar5416BTCoexSetupBmissThresh(struct ath_hal *ah, u_int32_t thresh);
extern void ar5416BTCoexDisable(struct ath_hal *ah);
extern int ar5416BTCoexEnable(struct ath_hal *ah);
extern void ar5416InitBTCoex(struct ath_hal *ah);
#endif

extern int ar5416_getSpurInfo(struct ath_hal * ah, int *enable, int len, u_int16_t *freq);
extern int ar5416_setSpurInfo(struct ath_hal * ah, int enable, int len, u_int16_t *freq);
extern void ar5416WowSetGpioResetLow(struct ath_hal * ah);
#ifdef ATH_CCX
extern void ar5416GetMibCycleCounts(struct ath_hal *, HAL_COUNTERS*);
extern void ar5416ClearMibCounters(struct ath_hal *ah);
extern u_int8_t ar5416GetCcaThreshold(struct ath_hal *ah);
#endif

/* EEPROM interface functions */
/* Common Interface functions */
extern  HAL_STATUS ar5416EepromAttach(struct ath_hal *);
extern  u_int32_t ar5416EepromGet(struct ath_hal_5416 *ahp, EEPROM_PARAM param);

extern  u_int32_t ar5416INIFixup(struct ath_hal *ah,
                                    ar5416_eeprom_t *pEepData,
                                    u_int32_t reg, 
                                    u_int32_t val);

extern  HAL_STATUS ar5416EepromSetTransmitPower(struct ath_hal *ah,
                     ar5416_eeprom_t *pEepData, HAL_CHANNEL_INTERNAL *chan,
                     u_int16_t cfgCtl, u_int16_t twiceAntennaReduction,
                     u_int16_t twiceMaxRegulatoryPower, u_int16_t powerLimit);
extern  void ar5416EepromSetAddac(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  HAL_BOOL ar5416EepromSetParam(struct ath_hal *ah, EEPROM_PARAM param, u_int32_t value);
extern  HAL_BOOL ar5416EepromSetBoardValues(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  HAL_BOOL ar5416EepromRead(struct ath_hal *, u_int off, u_int16_t *data);
extern  HAL_BOOL ar5416FlashRead(struct ath_hal *, u_int off, u_int16_t *data);
#if AH_SUPPORT_WRITE_EEPROM
extern  HAL_BOOL ar5416EepromWrite(struct ath_hal *, u_int off, u_int16_t data);
#endif
extern  HAL_BOOL ar5416FlashWrite(struct ath_hal *, u_int off, u_int16_t data);
extern  u_int ar5416EepromDumpSupport(struct ath_hal *ah, void **ppE);
extern  u_int8_t ar5416GetNumAntConfig(struct ath_hal_5416 *ahp, HAL_FREQ_BAND freq_band);
extern  HAL_STATUS ar5416EepromGetAntCfg(struct ath_hal_5416 *ahp, HAL_CHANNEL_INTERNAL *chan,
                                     u_int8_t index, u_int32_t *config);
extern u_int8_t* ar5416EepromGetCustData(struct ath_hal_5416 *ahp);



/* EEPROM Default MAP - interface functions */ 
extern u_int32_t ar5416EepromDefGet(struct ath_hal_5416 *ahp, EEPROM_PARAM param);
extern  HAL_BOOL ar5416EepromDefSetParam(struct ath_hal *ah, EEPROM_PARAM param, u_int32_t value);
extern  HAL_BOOL ar5416EepromDefSetBoardValues(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  u_int32_t ar5416EepromDefINIFixup(struct ath_hal *ah,
                                    ar5416_eeprom_def_t *pEepData,
                                    u_int32_t reg, 
                                    u_int32_t val);
extern  HAL_STATUS ar5416EepromDefSetTransmitPower(struct ath_hal *ah,
                     ar5416_eeprom_def_t *pEepData, HAL_CHANNEL_INTERNAL *chan,
                     u_int16_t cfgCtl, u_int16_t twiceAntennaReduction,
                     u_int16_t twiceMaxRegulatoryPower, u_int16_t powerLimit);
extern  void ar5416EepromDefSetAddac(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  u_int8_t ar5416EepromDefGetNumAntConfig(struct ath_hal_5416 *ahp, HAL_FREQ_BAND freq_band);
extern  HAL_STATUS ar5416EepromDefGetAntCfg(struct ath_hal_5416 *ahp, HAL_CHANNEL_INTERNAL *chan,
                                     u_int8_t index, u_int32_t *config);
extern HAL_STATUS ar5416CheckEepromDef(struct ath_hal *ah);
extern HAL_BOOL ar5416FillEepromDef(struct ath_hal *ah);

/* EEPROM  4KBits MAP - interface functions */  
extern u_int32_t ar5416Eeprom4kGet(struct ath_hal_5416 *ahp, EEPROM_PARAM param);
extern  HAL_BOOL ar5416Eeprom4kSetParam(struct ath_hal *ah, EEPROM_PARAM param, u_int32_t value);
extern  HAL_BOOL ar5416Eeprom4kSetBoardValues(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  u_int32_t ar5416Eeprom4kINIFixup(struct ath_hal *ah,
                                    ar5416_eeprom_4k_t *pEepData,
                                    u_int32_t reg, 
                                    u_int32_t val);
extern  HAL_STATUS ar5416Eeprom4kSetTransmitPower(struct ath_hal *ah,
                     ar5416_eeprom_4k_t *pEepData, HAL_CHANNEL_INTERNAL *chan,
                     u_int16_t cfgCtl, u_int16_t twiceAntennaReduction,
                     u_int16_t twiceMaxRegulatoryPower, u_int16_t powerLimit);
extern  void ar5416Eeprom4kSetAddac(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  u_int8_t ar5416Eeprom4kGetNumAntConfig(struct ath_hal_5416 *ahp, HAL_FREQ_BAND freq_band);
extern  HAL_STATUS ar5416Eeprom4kGetAntCfg(struct ath_hal_5416 *ahp, HAL_CHANNEL_INTERNAL *chan,
                                     u_int8_t index, u_int32_t *config);
extern HAL_STATUS ar5416CheckEeprom4k(struct ath_hal *ah);
extern HAL_BOOL ar5416FillEeprom4k(struct ath_hal *ah);


/* EEPROM  AR9287 MAP - interface functions */  
#ifdef AH_SUPPORT_EEPROM_AR9287
extern u_int32_t ar9287EepromGet(struct ath_hal_5416 *ahp, EEPROM_PARAM param);
extern  HAL_BOOL ar9287EepromSetParam(struct ath_hal *ah, EEPROM_PARAM param, u_int32_t value);
extern  HAL_BOOL ar9287EepromSetBoardValues(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  u_int32_t ar9287EepromINIFixup(struct ath_hal *ah,
                                    ar9287_eeprom_t *pEepData,
                                    u_int32_t reg, 
                                    u_int32_t val);
extern  HAL_STATUS ar9287EepromSetTransmitPower(struct ath_hal *ah,
                     ar9287_eeprom_t *pEepData, HAL_CHANNEL_INTERNAL *chan,
                     u_int16_t cfgCtl, u_int16_t twiceAntennaReduction,
                     u_int16_t twiceMaxRegulatoryPower, u_int16_t powerLimit);
extern  void ar9287EepromSetAddac(struct ath_hal *, HAL_CHANNEL_INTERNAL *);
extern  u_int8_t ar9287EepromGetNumAntConfig(struct ath_hal_5416 *ahp, HAL_FREQ_BAND freq_band);
extern  HAL_STATUS ar9287EepromGetAntCfg(struct ath_hal_5416 *ahp, HAL_CHANNEL_INTERNAL *chan,
                                     u_int8_t index, u_int32_t *config);
extern HAL_STATUS ar9287CheckEeprom(struct ath_hal *ah);
extern void ar9287FillEmuEeprom(struct ath_hal_5416 *ahp);
extern HAL_BOOL ar9287FillEeprom(struct ath_hal *ah);
#else
/* The EEPROM Ar9287 map type is not supported if AH_SUPPORT_EEPROM_4K
   is undefined.  Complain if the driver tries to use it anyway.
 */
#define ar9287EepromNotSupported(ah, func) \
    ar5416FeatureNotSupported(EepromAr9287, ah, func)

#define ar9287EepromGet(ahp, param)                                       \
    ( ar9287EepromNotSupported((struct ath_hal *) (ahp), __func__), 0 )
#define ar9287EepromSetParam(ah, param, value)                            \
    ( ar9287EepromNotSupported(ah, __func__), 0 )
#define ar9287EepromSetBoardValues(ah, chan)                              \
    ( ar9287EepromNotSupported(ah, __func__), 0 )
#define ar9287EepromINIFixup(ah, pEepData, reg, val)                      \
    ( ar9287EepromNotSupported(ah, __func__), 0 )
#define ar9287EepromSetTransmitPower(                                     \
    ah, pEepData, chan, cfgCtl, twiceAntennaReduction,                      \
    twiceMaxRegulatoryPower, powerLimit)                                    \
    ( ar9287EepromNotSupported(ah, __func__), 0 )
#define ar9287EepromSetAddac(ah, chan)                                    \
    ( ar9287EepromNotSupported(ah, __func__) )
#define ar9287EepromGetNumAntConfig(ahp, freq_band)                       \
    ( ar9287EepromNotSupported((struct ath_hal *) (ahp), __func__), 0 )
#define ar9287EepromGetAntCfg(ahp, chan, index, config)                   \
    ( ar9287EepromNotSupported((struct ath_hal *) (ahp), __func__), 0 )
#define ar9287CheckEeprom(ah)                                             \
    ( ar9287EepromNotSupported(ah, __func__), 0 )
#define ar9287FillEmuEeprom(ahp)                                          \
    ( ar9287EepromNotSupported((struct ath_hal *) (ahp), __func__) )
#define ar9287FillEeprom(ah)                                              \
    ( ar9287EepromNotSupported(ah, __func__), 0 )

#endif /* AH_SUPPORT_EEPROM_AR9287 */


/* Common EEPROM Help function */
extern void ar5416GetTargetPowers(struct ath_hal *ah,
    HAL_CHANNEL_INTERNAL *chan, CAL_TARGET_POWER_HT *powInfo,
    u_int16_t numChannels, CAL_TARGET_POWER_HT *pNewPower,
    u_int16_t numRates, HAL_BOOL isHt40Target);
extern void ar5416GetTargetPowersLeg(struct ath_hal *ah,
    HAL_CHANNEL_INTERNAL *chan, CAL_TARGET_POWER_LEG *powInfo,
    u_int16_t numChannels, CAL_TARGET_POWER_LEG *pNewPower,
    u_int16_t numRates, HAL_BOOL isExtTarget);
extern void ar5416SetImmunity(struct ath_hal *ah, HAL_BOOL enable);
extern void ar5416GetHwHangs(struct ath_hal *ah, hal_hw_hangs_t *hangs);
extern void ar5416AntDivCombGetConfig(struct ath_hal *ah, HAL_ANT_COMB_CONFIG* divCombConf);
extern void ar5416AntDivCombSetConfig(struct ath_hal *ah, HAL_ANT_COMB_CONFIG* divCombConf);
#endif  /* _ATH_AR5416_H_ */
