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
#ifndef _ATH_CWM_H_
#define _ATH_CWM_H_

/*
 * Channel Width Management (CWM)
 */
 

/*
 * External Definitions 
 */
#define ATH_CWM_EXTCH_BUSY_THRESHOLD  20  /* Extension Channel Busy Threshold (0-100%) */


/*
 * External Structures 
 */

/* CWM hardware state */
struct ath_cwm_hwstate {
    HAL_HT_MACMODE              ht_prev_macmode;             /* MAC - 20/40 mode */
    HAL_HT_MACMODE              ht_macmode;             /* MAC - 20/40 mode */
    HAL_HT_EXTPROTSPACING       ht_extprotspacing;      /* ext channel protection spacing */
};

int  ath_cwm_attach(struct ath_softc_net80211 *scn, struct ath_reg_parm *ath_conf_parm);
void ath_cwm_detach(struct ath_softc_net80211 *scn);
void ath_cwm_init(struct ath_softc_net80211 *scn);
void ath_cwm_stop(struct ath_softc_net80211 *scn);
int  ath_cwm_ioctl(struct ath_softc_net80211 *scn, int cmd, caddr_t data);
u_int32_t ath_cwm_getextbusy(struct ath_softc_net80211 *scn);
void ath_cwm_gethwstate(struct ath_softc_net80211 *scn, struct ath_cwm_hwstate *cwm_hwstate);
enum ieee80211_cwm_width ath_cwm_getlocalchwidth(struct ieee80211com *ic);
HAL_HT_MACMODE ath_net80211_cwm_macmode(ieee80211_handle_t ieee);

/* Notifications */
void ath_cwm_newstate(struct ieee80211vap *vap, enum ieee80211_state state);
void ath_cwm_newchwidth(struct ieee80211_node *ni);
void ath_cwm_txtimeout(struct ath_softc_net80211 *scn);

void ath_cwm_scan_start(struct ieee80211com *ic);
void ath_cwm_scan_end(struct ieee80211com *ic);

void ath_cwm_radio_disable(struct ath_softc_net80211 *scn);
void ath_cwm_radio_enable(struct ath_softc_net80211 *scn);

void ath_chwidth_change(struct ieee80211_node *ni);
void ath_cwm_switch_mode_static20(struct ieee80211com *ic);
void ath_cwm_switch_mode_dynamic2040(struct ieee80211com *ic);
void ath_cwm_switch_to40(struct ieee80211com *ic);
void ath_cwm_switch_to20(struct ieee80211com *ic);

#endif /* _ATH_CWM_H_ */
