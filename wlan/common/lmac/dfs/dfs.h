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

#ifndef _DFS_H_
#define _DFS_H_

#define	ATH_DFSQ_LOCK(_dfs)         spin_lock(&(_dfs)->dfs_radarqlock)
#define	ATH_DFSQ_UNLOCK(_dfs)       spin_unlock(&(_dfs)->dfs_radarqlock)
#define	ATH_DFSQ_LOCK_INIT(_dfs)    spin_lock_init(&(_dfs)->dfs_radarqlock)

#define	ATH_ARQ_LOCK(_dfs)          spin_lock(&(_dfs)->dfs_arqlock)
#define	ATH_ARQ_UNLOCK(_dfs)        spin_unlock(&(_dfs)->dfs_arqlock)
#define	ATH_ARQ_LOCK_INIT(_dfs)     spin_lock_init(&(_dfs)->dfs_arqlock)

#define	ATH_DFSEVENTQ_LOCK(_dfs)    spin_lock(&(_dfs)->dfs_eventqlock)
#define	ATH_DFSEVENTQ_UNLOCK(_dfs)  spin_unlock(&(_dfs)->dfs_eventqlock)
#define	ATH_DFSEVENTQ_LOCK_INIT(_dfs)   spin_lock_init(&(_dfs)->dfs_eventqlock)





#define DFS_TSMASK              0xFFFFFFFF      /* Mask for time stamp from descriptor */
#define DFS_TSSHIFT             32              /* Shift for time stamp from descriptor */
#define	DFS_TSF_WRAP		0xFFFFFFFFFFFFFFFFULL	/* 64 bit TSF wrap value */
#define	DFS_64BIT_TSFMASK	0x0000000000007FFFULL	/* TS mask for 64 bit value */


#define	DFS_AR_RADAR_RSSI_THR		5	/* in dB */
#define	DFS_AR_RADAR_RESET_INT		1	/* in secs */
#define	DFS_AR_RADAR_MAX_HISTORY	500
#define	DFS_AR_REGION_WIDTH		128
#define	DFS_AR_RSSI_THRESH_STRONG_PKTS	17	/* in dB */
#define	DFS_AR_RSSI_DOUBLE_THRESHOLD	15	/* in dB */
#define	DFS_AR_MAX_NUM_ACK_REGIONS	9
#define	DFS_AR_ACK_DETECT_PAR_THRESH	20
#define	DFS_AR_PKT_COUNT_THRESH		20

#define	DFS_MAX_DL_SIZE			64
#define	DFS_MAX_DL_MASK			0x3F

#define DFS_NOL_TIME			30*60*1000000	/* 30 minutes in usecs */

#define DFS_WAIT_TIME			60*1000000	/* 1 minute in usecs */

#define	DFS_DISABLE_TIME		3*60*1000000	/* 3 minutes in usecs */

#define	DFS_MAX_B5_SIZE			128
#define	DFS_MAX_B5_MASK			0x0000007F	/* 128 */

#define	DFS_MAX_RADAR_OVERLAP		16		/* Max number of overlapping filters */

#define	DFS_MAX_EVENTS			1024		/* Max number of dfs events which can be q'd */

#define DFS_RADAR_EN		0x80000000	/* Radar detect is capable */
#define DFS_AR_EN		0x40000000	/* AR detect is capable */
#define	DFS_MAX_RSSI_VALUE	0x7fffffff	/* Max rssi value */

#define DFS_BIN_MAX_PULSES              60      /* max num of pulses in a burst */
#define DFS_BIN5_PRI_LOWER_LIMIT	990	/* us */
#define DFS_BIN5_PRI_HIGHER_LIMIT	2010	/* us */
#define DFS_BIN5_WIDTH_MARGIN	    	6	/* us */
#define DFS_BIN5_RSSI_MARGIN	    	5	/* dBm */
/*Following threshold is not specified but should be okay statistically*/
#define DFS_BIN5_BRI_LOWER_LIMIT	300000  /* us */

#define DFS_MAX_PULSE_BUFFER_SIZE 1024          /* Max number of pulses kept in buffer */
#define DFS_MAX_PULSE_BUFFER_MASK 0x3ff
        
#define DFS_FAST_CLOCK_MULTIPLIER       (800/11)
#define DFS_NO_FAST_CLOCK_MULTIPLIER    (80)

typedef	spinlock_t dfsq_lock_t;

struct dfs_pulseparams {
        u_int64_t       p_time;                 /* time for start of pulse in usecs*/
        u_int8_t        p_dur;                  /* Duration of pulse in usecs*/
        u_int8_t        p_rssi;                 /* Duration of pulse in usecs*/
};


struct dfs_pulseline {
        struct dfs_pulseparams pl_elems[DFS_MAX_PULSE_BUFFER_SIZE];     /* Array of pulses in delay line */
        u_int32_t       pl_firstelem;           /* Index of the first element */
        u_int32_t       pl_lastelem;            /* Index of the last element */
        u_int32_t       pl_numelems;            /* Number of elements in the delay line */
};

struct dfs_event {
	u_int32_t	re_ts;		/* Original 15 bit recv timestamp */
	u_int64_t	re_full_ts;	/* 64-bit full timestamp from interrupt time */
	u_int8_t	re_rssi;	/* rssi of radar event */
	u_int8_t	re_dur;		/* duration of radar pulse */
	u_int8_t	re_chanindex;	/* Channel of event */
	STAILQ_ENTRY(dfs_event)	re_list;	/* List of radar events */
};

#define DFS_AR_MAX_ACK_RADAR_DUR	511
#define DFS_AR_MAX_NUM_PEAKS		3
#define DFS_AR_ARQ_SIZE			2048	/* 8K AR events for buffer size */
#define DFS_AR_ARQ_SEQSIZE		2049	/* Sequence counter wrap for AR */

#define DFS_RADARQ_SIZE		512		/* 1K radar events for buffer size */
#define DFS_RADARQ_SEQSIZE	513		/* Sequence counter wrap for radar */
#define DFS_NUM_RADAR_STATES	64		/* Number of radar channels we keep state for */
#define DFS_MAX_NUM_RADAR_FILTERS 10		/* Max number radar filters for each type */ 
#define DFS_MAX_RADAR_TYPES	32		/* Number of different radar types */

struct dfs_ar_state {
	u_int16_t	ar_prevtimestamp;
	u_int32_t	ar_prevwidth;
	u_int32_t	ar_phyerrcount[DFS_AR_MAX_ACK_RADAR_DUR];
	u_int32_t	ar_acksum;
	u_int16_t	ar_peaklist[DFS_AR_MAX_NUM_PEAKS];
	u_int32_t	ar_packetthreshold;	/* Thresh to determine traffic load */
	u_int32_t	ar_parthreshold;	/* Thresh to determine peak */
	u_int32_t	ar_radarrssi;		/* Rssi threshold for AR event */
};

struct dfs_delayelem {
	u_int32_t	de_time;	/* Current "filter" time for start of pulse in usecs*/
	u_int8_t	de_dur;		/* Duration of pulse in usecs*/
        u_int8_t        de_rssi;        /* rssi of pulse in dB*/

};

/* NB: The first element in the circular buffer is the oldest element */

struct dfs_delayline {
	struct dfs_delayelem dl_elems[DFS_MAX_DL_SIZE];	/* Array of pulses in delay line */
	u_int64_t	dl_last_ts;		/* Last timestamp the delay line was used (in usecs) */
	u_int32_t	dl_firstelem;		/* Index of the first element */
	u_int32_t	dl_lastelem;		/* Index of the last element */
	u_int32_t	dl_numelems;		/* Number of elements in the delay line */
};

struct dfs_filter {
        struct dfs_delayline rf_dl;     /* Delay line of pulses for this filter */
        u_int32_t       rf_numpulses;   /* Number of pulses in the filter */
        u_int32_t       rf_minpri;      /* min pri to be considered for this filter*/
        u_int32_t       rf_maxpri;      /* max pri to be considered for this filter*/
        u_int32_t       rf_threshold;   /* match filter output threshold for radar detect */
        u_int32_t       rf_filterlen;   /* Length (in usecs) of the filter */
        u_int32_t       rf_patterntype; /* fixed or variable pattern type */
        u_int32_t       rf_mindur;      /* Min duration for this radar filter */
        u_int32_t       rf_maxdur;      /* Max duration for this radar filter */
        u_int32_t       rf_pulseid;     /* Unique ID corresponding to the original filter ID */
};

struct dfs_filtertype {
	struct dfs_filter ft_filters[DFS_MAX_NUM_RADAR_FILTERS];	/* array of filters */
	u_int32_t	ft_filterdur;    /* Duration of pulse which specifies filter type */
	u_int32_t	ft_numfilters;   /* Num filters of this type */
	u_int64_t       ft_last_ts;             /* Last timestamp this filtertype was used (in usecs) */
	u_int32_t	ft_mindur;	 /* min pulse duration to be considered
					   for this filter type */
	u_int32_t	ft_maxdur;	 /* max pulse duration to be consdiered
						   for this filter type */
	u_int32_t	ft_rssithresh;	 /* min rssi to be considered for this filter type */
	u_int32_t	ft_numpulses;	 /* Num pulses in each filter of this type */
        u_int32_t       ft_patterntype; /* fixed or variable pattern type */
	u_int32_t 	ft_minpri;	 /* min pri to be considered for this type*/
	u_int32_t	ft_rssimargin;	 /* rssi threshold margin. In Turbo Mode HW reports rssi 3dB */
					/* lower than in non TURBO mode. This will offset that diff.*/
};

struct dfs_state {
	HAL_CHANNEL	rs_chan;		/* Channel info */
	u_int8_t	rs_chanindex;		/* Channel index in radar structure */
	u_int32_t	rs_numradarevents;	/* Number of radar events */
	int32_t		rs_firpwr;		/* Thresh to check radar sig is gone */ 
	u_int32_t	rs_radarrssi;		/* Thresh to start radar det (dB) */
	u_int32_t	rs_height;		/* Thresh for pulse height (dB)*/
	u_int32_t	rs_pulserssi;		/* Thresh to check if pulse is gone (dB) */
	u_int32_t	rs_inband;		/* Thresh to check if pulse is inband (0.5 dB) */
	u_int32_t	rs_relstep_en;		/* Pulse relative step check */
	u_int32_t	rs_relstep;		/* Thresh for pulse relative step check */
	u_int32_t	rs_relpwr_en;		/* Check radar relative power */
	u_int32_t	rs_relpwr;		/* Thresh for radar relative power */
	u_int32_t	rs_maxlen;		/* Max length of radar pulse */
	u_int32_t	rs_usefir128;		/* Use the average inband power measured over 128 cycles */
	u_int32_t	rs_blockradar;		/* Block radar check */
	u_int32_t	rs_enmaxrssi;		/* Use max radar rssi */
};

#define DFS_NOL_TIMEOUT_S  (30*60)    /* 30 minutes in seconds */
#define DFS_NOL_TIMEOUT_MS (DFS_NOL_TIMEOUT_S * 1000)
#define DFS_NOL_TIMEOUT_US (DFS_NOL_TIMEOUT_MS * 1000)

struct dfs_nolelem {
	HAL_CHANNEL	nol_chan;		/* Channel info */
	struct dfs_nolelem *nol_next;		/* next element pointer */
        os_timer_t   	nol_timer;              /* per element NOL timer */
};

/* Pass structure to DFS NOL timer */
struct dfs_nol_timer_arg {
        struct ath_softc *sc;
        u_int16_t         delfreq;
}; 

struct dfs_info {
	int		rn_use_nol;		/* Use the NOL when radar found (default: TRUE) */
	u_int32_t	rn_numradars;		/* Number of different types of radars */
	u_int64_t	rn_lastfull_ts;		/* Last 64 bit timstamp from recv interrupt */
	u_int16_t	rn_last_ts;		/* last 15 bit ts from recv descriptor */
        u_int32_t       rn_last_unique_ts;      /* last unique 32 bit ts from recv descriptor */

	u_int64_t	rn_ts_prefix;		/* Prefix to prepend to 15 bit recv ts */
	u_int32_t	rn_numbin5radars;	/* Number of bin5 radar pulses to search for */
	u_int32_t	rn_fastdivGCval;	/* Value of fast diversity gc limit from init file */
	int32_t		rn_minrssithresh;	/* Min rssi for all radar types */
	u_int32_t	rn_maxpulsedur;		/* Max pulse width in TSF ticks */
};

struct dfs_bin5elem {
	u_int64_t	be_ts;			/* Timestamp for the bin5 element */
	u_int32_t	be_rssi;		/* Rssi for the bin5 element */
	u_int32_t	be_dur;			/* Duration of bin5 element */
};

struct dfs_bin5radars {
	struct dfs_bin5elem br_elems[DFS_MAX_B5_SIZE];	/* List of bin5 elems that fall
							 * within the time window */
	u_int32_t	br_firstelem;		/* Index of the first element */
	u_int32_t	br_lastelem;		/* Index of the last element */
	u_int32_t	br_numelems;		/* Number of elements in the delay line */
	struct dfs_bin5pulse br_pulse;		/* Original info about bin5 pulse */
};

struct dfs_stats {
        u_int32_t       num_radar_detects;      /* total num. of radar detects */
        u_int32_t       total_phy_errors;
        u_int32_t       owl_phy_errors;
        u_int32_t       pri_phy_errors;
        u_int32_t       ext_phy_errors;
        u_int32_t       dc_phy_errors;
        u_int32_t       early_ext_phy_errors;
        u_int32_t       bwinfo_errors;
        u_int32_t       datalen_discards;
        u_int32_t       rssi_discards;
        u_int64_t       last_reset_tstamp;
};

#define ATH_DFS_RESET_TIME_S 7
#define ATH_DFS_WAIT (60 + ATH_DFS_RESET_TIME_S) /* 60 seconds */
#define ATH_DFS_WAIT_MS ((ATH_DFS_WAIT) * 1000)	/*in MS*/

#define ATH_DFS_WEATHER_CHANNEL_WAIT_MIN 10 /*10 minutes*/
#define ATH_DFS_WEATHER_CHANNEL_WAIT_S (ATH_DFS_WEATHER_CHANNEL_WAIT_MIN * 60)
#define ATH_DFS_WEATHER_CHANNEL_WAIT_MS ((ATH_DFS_WEATHER_CHANNEL_WAIT_S) * 1000)	/*in MS*/

#define ATH_DFS_WAIT_POLL_PERIOD 2	/* 2 seconds */
#define ATH_DFS_WAIT_POLL_PERIOD_MS ((ATH_DFS_WAIT_POLL_PERIOD) * 1000)	/*in MS*/
#define	ATH_DFS_TEST_RETURN_PERIOD 2	/* 2 seconds */
#define	ATH_DFS_TEST_RETURN_PERIOD_MS ((ATH_DFS_TEST_RETURN_PERIOD) * 1000)/* n MS*/

#define IS_CHANNEL_WEATHER_RADAR(chan) ((chan->channel >= 5600) && (chan->channel <= 5650))

struct ath_dfs {
	int16_t			dfs_curchan_radindex;	/* cur. channel radar index */
	int16_t			dfs_extchan_radindex;	/* extension channel radar index */
	u_int32_t		dfsdomain;	/* cur. DFS domain */
	u_int32_t		dfs_proc_phyerr;/* Flags for Phy Errs to process */
	STAILQ_HEAD(,dfs_event)	dfs_eventq;	/* Q of free dfs event objects */
	dfsq_lock_t		dfs_eventqlock;	/* Lock for free dfs event list */
	STAILQ_HEAD(,dfs_event)	dfs_radarq;	/* Q of radar events */
	dfsq_lock_t		dfs_radarqlock;	/* Lock for dfs q */
	STAILQ_HEAD(,dfs_event) dfs_arq;	/* Q of AR events */
	dfsq_lock_t		dfs_arqlock;	/* Lock for AR q */

	struct dfs_ar_state	dfs_ar_state;	/* AR state */
	struct dfs_state	dfs_radar[DFS_NUM_RADAR_STATES]; 	/* Per-Channel Radar detector state */
	struct dfs_filtertype	*dfs_radarf[DFS_MAX_RADAR_TYPES]; /* One filter for each radar pulse type */
	struct dfs_info		dfs_rinfo;	/* State variables for radar processing */
	struct dfs_bin5radars	*dfs_b5radars;	/* array of bin5 radar events */
	int8_t			**dfs_radartable;	/* mapping of radar durations to filter types */
	struct dfs_nolelem	*dfs_nol;	/* Non occupancy list for radar */
	HAL_PHYERR_PARAM	dfs_defaultparams; /* Default phy params per radar state */
	struct dfs_stats        ath_dfs_stats;		/* DFS related stats */
        struct dfs_pulseline    *pulses;        /* pulse history */
        struct dfs_event        *events;        /* Events structure */

        unsigned int
        sc_rtasksched:1, /* radar task is scheduled */
        sc_dfswait:1,    /* waiting on channel for radar detect */
        sc_dfstest:1,	/* Test timer in progress */
        sc_dfs_ext_chan_ok:1,	/* Can radar be detected on the extension channel? */
        sc_dfs_combined_rssi_ok:1; /* Can use combined radar RSSI? */
        u_int8_t		sc_dfstest_ieeechan;	/* IEEE channel number to return to after a dfs mute test */
        u_int32_t		sc_dfs_cac_time;	/* Time to stay off chan during dfs test */
        u_int32_t		sc_dfstesttime;	/* Time to stay off chan during dfs test */
        os_timer_t   	sc_dfs_task_timer;/* dfs wait timer */
        os_timer_t   	sc_dfswaittimer;/* dfs wait timer */
        os_timer_t  	sc_dfstesttimer;/* dfs mute test timer */
        u_int8_t        dfs_bangradar;
        int             dur_multiplier;
};

/* This should match the table from if_ath.c */
enum {
        ATH_DEBUG_DFS       = 0x00000100,   /* Minimal DFS debug */
        ATH_DEBUG_DFS1      = 0x00000200,   /* Normal DFS debug */
        ATH_DEBUG_DFS2      = 0x00000400,   /* Maximal DFS debug */
        ATH_DEBUG_DFS3      = 0x00000800,   /* matched filterID display */
};

/* These defines should match the table from ah_internal.h */
enum {
	DFS_UNINIT_DOMAIN	= 0,	/* Uninitialized dfs domain */
	DFS_FCC_DOMAIN		= 1,	/* FCC3 dfs domain */
	DFS_ETSI_DOMAIN		= 2,	/* ETSI dfs domain */
	DFS_MKK4_DOMAIN		= 3	/* Japan dfs domain */
};

#define	HAL_CAP_RADAR	0
#define	HAL_CAP_AR	1
#define HAL_CAP_STRONG_DIV 2

int         dfs_process_radarevent(struct ath_softc *sc, HAL_CHANNEL *chan);
u_int32_t   dfs_check_nol(struct ath_softc *sc, HAL_CHANNEL *chans,
                          u_int32_t nchans, u_int16_t delfreq);
int         dfs_attach(struct ath_softc *sc);
void        dfs_detach(struct ath_softc *sc);
void        ath_ar_enable(struct ath_softc *sc);
int         dfs_radar_enable(struct ath_softc *sc);
void        ath_ar_disable(struct ath_softc *sc);
int         dfs_check_chirping(struct ath_softc *sc, struct ath_desc *ds,
                               int is_ctl, int is_ext, int *slope, int *is_dc);
void        ath_process_phyerr(struct ath_softc *sc, struct ath_desc *ds,
                               u_int64_t fulltsf);
void        dfs_process_ar_event(struct ath_softc *sc, HAL_CHANNEL *chan);
struct dfs_state *dfs_getchanstate(struct ath_softc *sc, u_int8_t *index, int ext_ch_flag);
int         dfs_control(ath_dev_t dev, u_int id,
                            void *indata, u_int32_t insize,
                            void *outdata, u_int32_t *outsize);
int         dfs_get_thresholds(struct ath_softc *sc, HAL_PHYERR_PARAM *param);
int         dfs_init_radar_filters(struct ath_softc *sc);
void        dfs_clear_stats(struct ath_softc *sc);
#endif  /* _DFS_H_ */
