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

/*
 * Public Interface for Bluetooth coexistence module
 */

#ifndef _DEV_ATH_BT_H
#define _DEV_ATH_BT_H

#ifdef ATH_BT_COEX
typedef enum {
    ATH_BT_COEX_STATE_OFF = 0,      /* Bluetooth coexistence is off */
    ATH_BT_COEX_STATE_ON,           /* Bluetooth coexistence is on, and WLAN is connected */
    ATH_BT_COEX_STATE_OFFCHANNEL,   /* WLAN is off channel or disconnected */
    ATH_BT_COEX_MAX_STATE
} ATH_BT_COEX_STATE;

typedef enum {
    ATH_BT_COEX_DISABLE = 0,        /* disable Bluetooth coexistence */
    ATH_BT_COEX_ENABLE,             /* enable Bluetooth coexistence */
    ATH_BT_COEX_STOMP,              /* stomp all Bluetooth traffic */
} ATH_BT_COEX_MODE;

typedef enum {
    ATH_BT_COEX_CFG_NONE,          /* No bt coex enabled */
    ATH_BT_COEX_CFG_2WIRE_2CH,     /* 2-wire with 2 chains */
    ATH_BT_COEX_CFG_2WIRE_CH1,     /* 2-wire with ch1 */
    ATH_BT_COEX_CFG_2WIRE_CH0,     /* 2-wire with ch0 */
    ATH_BT_COEX_CFG_DCA,           /* Dynamic Channel Avoidance */
} ATH_BT_COEX_CFG;

typedef enum {
    ATH_BT_PROT_MODE_NONE,
    ATH_BT_PROT_MODE_ON,
    ATH_BT_PROT_MODE_DEFER,
} ATH_BT_PROTECT_MODE;

struct ath_bt_info;

struct ath_bt_stream_info {
    int     (*init)(struct ath_softc *sc, struct ath_bt_info *btinfo);
    int     (*resume)(struct ath_softc *sc, struct ath_bt_info *btinfo);
    int     (*pause)(struct ath_softc *sc, struct ath_bt_info *btinfo);
    void    (*free)(struct ath_softc *sc, struct ath_bt_info *btinfo);

    BT_STREAM_TYPE      type;                           /* stream type */
    ATH_BT_COEX_MODE    modes[ATH_BT_COEX_MAX_STATE];   /* coex mode for each state */
};

/*
 * SCO definitions
 */

struct ath_bt_sco_info {
    struct ath_bt_stream_info   sco_stream;
    struct ath_gen_timer        *sco_hwtimer;

    u_int8_t    sco_sync_cnt;
    u_int8_t    sco_overflow_cnt;
    u_int32_t   sco_tstamp;
    u_int8_t    sco_sample : 1;
};

#define SCO_RESET_SYNC_STATE(_sco)  do {    \
    sco->sco_sync_cnt = 0;                  \
    sco->sco_overflow_cnt = 0;              \
    sco->sco_sample = 0;                    \
} while (AH_FALSE);

/*
 * A2DP definitions
 */
struct ath_bt_a2dp_info {
    struct ath_bt_stream_info   a2dp_stream;
    struct ath_timer            a2dp_period_timer;
    struct ath_timer            a2dp_dcycle_timer;

    u_int32_t                   a2dp_period;    /* in msec */
    u_int32_t                   a2dp_dcycle;    /* in percentage of a period */
    HAL_BOOL                    a2dp_stompbt;   /* whether to stomp BT during WLAN transmit */
};

/*
 * HID definitions
 */
struct ath_bt_hid_info {
    struct ath_bt_stream_info   hid_stream;
};

struct ath_bt_info {
    struct ath_bt_stream_info   *bt_stream;

    HAL_BOOL            bt_on;              /* whether Bluetooth device is on */
    ATH_BT_COEX_STATE   bt_state;

    u_int32_t           bt_weightBT;
    u_int32_t           bt_weightWlan;
    int                 bt_bmissThresh;     /* BMISS threshold */
    int                 bt_gpioSelect;      /* GPIO pin for BT_ACTIVE */
    HAL_BOOL            bt_gpioIntEnabled;  /* GPIO interrupt is enabled*/
    HAL_BOOL            bt_coex_enabled;    /* BT coex enabled */
    u_int8_t            bt_protectMode;     /* BT protection mode */
    u_int8_t            bt_activePolarity;  /* BT_ACTIVE polarity */
    u_int32_t           bt_activeCount;     /* BT_ACTIVE count */
    struct ath_timer    bt_activeTimer;     /* Timer for BT_ACTIVE measurement */
    u_int8_t            bt_preGpioState;    /* Previous GPIO state */
    u_int8_t            bt_timeOverThre;    /* Time that BT_ACTIVE count is over threshold*/
    u_int8_t            bt_timeOverThre2;   /* Time that BT_ACTIVE count is over threshold2*/
    HAL_BOOL            bt_initStateDone;   /* End of init state*/
    u_int8_t            bt_initStateTime;   /* Time in init state */
    u_int8_t            bt_activeChainMask; /* Chain mask to use when force to single chain */

    HAL_BOOL            bt_gpioEnabled;     /* used for synchronization with GPIO interrupt */
    HAL_BOOL            bt_hwtimerEnabled;  /* used for synchronization with timer interrupt */
    spinlock_t          bt_lock;            /* btinfo level lock */
};

#define BT_COEX_BT_ACTIVE_MEASURE       1000    /* 1 second */
#define BT_COEX_BT_ACTIVE_THRESHOLD     20      /* Threshold for BT_ACTIVE state change */
#define BT_COEX_BT_ACTIVE_THRESHOLD2    400     /* Threshold2 for BT_ACTIVE state change */
#define BT_COEX_PROT_MODE_ON_TIME       2       /* 2 seconds over threshold to turn on protection mode */
#define BT_COEX_INIT_STATE_TIME         30      /* First 30 seconds after driver loaded is init state */
#define Bt_COEX_INIT_STATE_SCAN_TIME    4       /* Wait for 4 seconds before enable protection mode */

#define ATH_BT_TO_SCO(_btinfo)      ((struct ath_bt_sco_info *)((_btinfo)->bt_stream))
#define ATH_BT_TO_A2DP(_btinfo)     ((struct ath_bt_a2dp_info *)((_btinfo)->bt_stream))
#define ATH_BT_TO_HID(_btinfo)      ((struct ath_bt_hid_info *)((_btinfo)->bt_stream))

#define ATH_BT_LOCK_INIT(_btinfo)   spin_lock_init(&(_btinfo)->bt_lock);
#define ATH_BT_LOCK_DESTROY(_btinfo)
#define ATH_BT_LOCK(_btinfo)        spin_lock(&(_btinfo)->bt_lock);
#define ATH_BT_UNLOCK(_btinfo)      spin_unlock(&(_btinfo)->bt_lock);

#define BT_WGHT                     0xff55
#define BT_SCO_WLAN_WGHT            0xaaaa
#define BT_A2DP_WLAN_WGHT           0xaa00
#define STOMP_BT_WLAN_WGHT          0xffff

/* 
 * BT_ACTIVE signal time limit in usec for BT_ACTIVE signal
 * when Bluetooth is running SCO/A2DP.
 */
#define BT_SCO_TM_LOW_LMT          1010
#define BT_SCO_TM_HIGH_LMT         1220
#define BT_SCO_TM_INTRVL           3750
#define BT_SCO_IDLE_INTRVL         2580

#define BT_SCO_CYCLES_PER_SYNC     52
#define BT_SCO_OVERFLOW_LIMIT      10

/* based on observations with CSR chip */
#define BT_A2DP_PERIOD             59       /* in msec */
#define BT_A2DP_DUTY_CYCLE         47       /* in percent of BT_A2DP_PERIOD */

/* BT ISM band */
#define BT_LOW_FREQ                 2400
#define BT_HIGH_FREQ                2484

int ath_bt_coex_attach(struct ath_softc *sc, u_int8_t cfg);
void ath_bt_coex_detach(struct ath_softc *sc);
HAL_BOOL ath_bt_coex_gpio_isr(struct ath_softc *sc);
void ath_bt_coex_gpio_intr(struct ath_softc *sc);
#endif /* ATH_BT_COEX */

#endif /* end of _DEV_ATH_BT_H */
