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
 * Public Interface for slow antenna diversity control module
 */

#ifndef _DEV_ATH_ANTDIV_H
#define _DEV_ATH_ANTDIV_H

#define ATH_ANT_DIV_MAX_CFG   2
#define ATH_ANT_DIV_MAX_CHAIN 3

typedef enum {
    ATH_ANT_DIV_IDLE,
    ATH_ANT_DIV_SCAN,     /* evaluating antenna */
} ATH_ANT_DIV_STATE;

struct ath_antdiv {
    struct ath_softc        *antdiv_sc;
    u_int8_t                antdiv_start;
    u_int8_t                antdiv_flags;
    ATH_ANT_DIV_STATE       antdiv_state;
    u_int8_t                antdiv_num_antcfg;
    u_int8_t                antdiv_curcfg;
    u_int8_t                antdiv_bestcfg;
    int8_t                  antdiv_rssitrig;
    u_int32_t               antdiv_min_dwell_time;
    u_int32_t               antdiv_settle_time;
    int8_t                  antdiv_lastbrssictl[ATH_ANT_DIV_MAX_CHAIN][ATH_ANT_DIV_MAX_CFG];
/* not yet
    int8_t                  antdiv_lastbrssi[ATH_ANT_DIV_MAX_CFG];
    int8_t                  antdiv_lastbrssiext[ATH_ANT_DIV_MAX_CHAIN][ATH_ANT_DIV_MAX_CFG];
 */
    u_int64_t               antdiv_lastbtsf[ATH_ANT_DIV_MAX_CFG];
    u_int64_t               antdiv_laststatetsf;   
    u_int8_t                antdiv_bssid[IEEE80211_ADDR_LEN];
    int16_t                 laststatesuff[ATH_ANT_DIV_MAX_CFG];
    int16_t                 rssiThrLow;                 // Minimum RSSI value to tirgger scan
    int16_t                 rssiThrHigh;                // RSSI value to stop the scand and select a config.
    int16_t                 enabled;                    // Enable/disable scan
    u_int64_t               bytesThrRx;                 // Rx bytes threhold - to identify active stations
};

void ath_slow_ant_div_init(struct ath_antdiv *antdiv, struct ath_softc *sc, int32_t rssitrig,
                           u_int32_t min_dwell_time, u_int32_t settle_time);
void ath_slow_ant_div_start(struct ath_antdiv *antdiv, u_int8_t num_antcfg, const u_int8_t *bssid);
void ath_slow_ant_div_stop(struct ath_antdiv *antdiv);
void ath_slow_ant_div(struct ath_antdiv *antdiv, struct ieee80211_frame *wh, struct ath_rx_status *rx_stats);

/* Control functions to get and set parameters */
extern void      ath_slow_ant_div_setenable(struct ath_softc *sc, u_int32_t val);

struct ath_antcomb {
    struct ath_softc        *antcomb_sc;

    u_int16_t               total_pkt_count;
    int                     main_total_rssi;
    int                     alt_total_rssi;
    int                     alt_recv_cnt;
    int                     main_recv_cnt;
    int8_t                  rssi_lna1;
    int8_t                  rssi_lna2; 
   


};


#define ANT_DIV_COMB_MAX_PKTCOUNT   (0x200)
#define ANT_DIV_COMB_REG_MASK1 (AR_PHY_KITE_ANT_DIV_ALT_LNACONF | \
                                AR_PHY_KITE_ANT_FAST_DIV_BIAS)
#define ANT_DIV_COMB_REG_MASK2 (AR_PHY_KITE_ANT_DIV_MAIN_LNACONF | \
                                AR_PHY_KITE_ANT_DIV_ALT_LNACONF | \
                                AR_PHY_KITE_ANT_FAST_DIV_BIAS)
#define ANT_DIV_COMB_LNA1_LNA2_SWITCH_DELTA  (-1)


void ath_ant_div_comb_init(struct ath_antcomb *antcomb, struct ath_softc *sc);
void ath_ant_div_comb_scan(struct ath_antcomb *antcomb, struct ath_rx_status *rx_stats);

#endif
