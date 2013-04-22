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

#ifndef _ATH_IEEE80211_INTERFACES_H
#define _ATH_IEEE80211_INTERFACES_H

/*
 * Interfaces to IEEE80211 data structures. ATH layer should use these API's
 * instead of access the data structures directly.
 */
#include "ieee80211_var.h"
#ifndef INLINE
#define INLINE      __inline
#endif
#ifndef OS_MEMCPY
#define OS_MEMCPY(_dst, _src, _len)     memcpy(_dst, _src, _len)
#endif

/*
 * ************************************
 * IEEE80211COM Interfaces
 * ************************************
 */
int ieee80211_ifattach(struct ieee80211com *ic);
void ieee80211_ifdetach(struct ieee80211com *ic);
void ieee80211_update_country_channellist(struct ieee80211com *ic);
int ieee80211_setmode(struct ieee80211com *ic, enum ieee80211_phymode mode);
enum ieee80211_phymode ieee80211_chan2mode(const struct ieee80211_channel *chan);
void ieee80211_start_running(struct ieee80211com *ic);
void ieee80211_stop_running(struct ieee80211com *ic);
void ieee80211_reset(struct ieee80211com *ic);
void ieee80211_dump_pkt(struct ieee80211com *ic,
                        const u_int8_t *buf, int len, int rate, int rssi);
#ifdef NODE_FREE_DEBUG
void ieee80211_add_trace(struct ieee80211_node *ni, char *funcp, char *descp, u_int64_t value);
void ieee80211_print_trace(struct ieee80211_node *ni);
#endif
/*
 * Capabilities
 */
static INLINE void
ieee80211com_set_cap(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_caps |= cap;
}

static INLINE void
ieee80211com_clear_cap(struct ieee80211com *ic, u_int32_t cap)
{
    ic->ic_caps &= ~cap;
}

static INLINE int
ieee80211com_has_cap(struct ieee80211com *ic, u_int32_t cap)
{
    return ((ic->ic_caps & cap) != 0);
}

/*
 * Atheros Capabilities
 */
static INLINE void
ieee80211com_set_athcap(struct ieee80211com *ic, u_int32_t athcap)
{
    ic->ic_ath_cap |= athcap;
}

static INLINE void
ieee80211com_clear_athcap(struct ieee80211com *ic, u_int32_t athcap)
{
    ic->ic_ath_cap &= ~athcap;
}

static INLINE int
ieee80211com_has_athcap(struct ieee80211com *ic, u_int32_t athcap)
{
    return ((ic->ic_ath_cap & athcap) != 0);
}

/*
 * Atheros Extended Capabilities
 */
static INLINE void
ieee80211com_set_athextcap(struct ieee80211com *ic, u_int16_t athextcap)
{
    ic->ic_ath_extcap |= athextcap;
}

static INLINE void
ieee80211com_clear_athextcap(struct ieee80211com *ic, u_int16_t athextcap)
{
    ic->ic_ath_extcap &= ~athextcap;
}

static INLINE int
ieee80211com_has_athextcap(struct ieee80211com *ic, u_int16_t athextcap)
{
    return ((ic->ic_ath_extcap & athextcap) != 0);
}

/*
 * IC flags
 */
static INLINE void
ieee80211com_set_flag(struct ieee80211com *ic, u_int32_t flag)
{
    ic->ic_flags |= flag;
}

static INLINE void
ieee80211com_clear_flag(struct ieee80211com *ic, u_int32_t flag)
{
    ic->ic_flags &= ~flag;
}

static INLINE int
ieee80211com_has_flag(struct ieee80211com *ic, u_int32_t flag)
{
    return ((ic->ic_flags & flag) != 0);
}

/*
 * IC extended flags
 */
static INLINE void
ieee80211com_set_extflag(struct ieee80211com *ic, u_int32_t extflag)
{
    ic->ic_flags_ext |= extflag;
}

static INLINE void
ieee80211com_clear_extflag(struct ieee80211com *ic, u_int32_t extflag)
{
    ic->ic_flags_ext &= ~extflag;
}

static INLINE int
ieee80211com_has_extflag(struct ieee80211com *ic, u_int32_t extflag)
{
    return ((ic->ic_flags_ext & extflag) != 0);
}

/*
 * Operation Mode
 */
static INLINE enum ieee80211_opmode
ieee80211com_get_opmode(struct ieee80211com *ic)
{
    return ic->ic_opmode;
}

/*
 * PHY type
 */
static INLINE enum ieee80211_phytype
ieee80211com_get_phytype(struct ieee80211com *ic)
{
    return ic->ic_phytype;
}

static INLINE void
ieee80211com_set_phytype(struct ieee80211com *ic, enum ieee80211_phytype phytype)
{
    ic->ic_phytype = phytype;
}

/*
 * Pre-conditions for ForcePPM: must have just one STA associated.
 */
static INLINE int
ieee80211com_can_enable_force_ppm(struct ieee80211com *ic)
{
    return (ic->ic_sta_assoc == 1);
}

/*
 * Regulatory
 */
static INLINE u_int *
ieee80211com_get_nregclass(struct ieee80211com *ic)
{
    return &ic->ic_nregclass;
}

static INLINE u_int8_t *
ieee80211com_get_regclassids(struct ieee80211com *ic)
{
    return ic->ic_regclassids;
}

int ieee80211com_set_country_code(struct ieee80211com *ic, char* isoName);

void ieee80211_update_spectrumrequirement(struct ieee80211com *ic,
                                          struct ieee80211vap *vap
                                          );

void ieee80211com_set_country_outdoor(struct ieee80211com *ic);

/*
 * Channel
 */
struct ieee80211_channel_list {
    int                         cl_nchans;
    struct ieee80211_channel    *cl_channels;
};

static INLINE void
ieee80211com_get_channel_list(struct ieee80211com *ic,
                          struct ieee80211_channel_list *cl)
{
    cl->cl_nchans = ic->ic_nchans;
    cl->cl_channels = ic->ic_channels;
}

static INLINE void
ieee80211com_set_nchannels(struct ieee80211com *ic, int nchans)
{
    ic->ic_nchans = nchans;
}

static INLINE struct ieee80211_channel *
ieee80211com_get_curchan(struct ieee80211com *ic)
{
    return ic->ic_curchan;
}

static INLINE struct ieee80211_channel *
ieee80211com_get_bsschan(struct ieee80211com *ic)
{
    return ic->ic_bsschan;
}

static INLINE void
ieee80211com_set_curchanmaxpwr(struct ieee80211com *ic, u_int8_t maxpower)
{
	if (ic->ic_curchanmaxpwr == 0)
		ic->ic_curchanmaxpwr = maxpower;
}

static INLINE u_int8_t
ieee80211com_get_coverageclass(struct ieee80211com *ic)
{
    return ic->ic_coverageclass;
}

/*
 * Rates
 */
static INLINE struct ieee80211_rateset *
ieee80211com_get_rates(struct ieee80211com *ic, u_int mode)
{
    return &ic->ic_sup_rates[mode];
}

static INLINE struct ieee80211_rateset *
ieee80211com_get_ht_rates(struct ieee80211com *ic, u_int mode)
{
    return &ic->ic_sup_ht_rates[mode];
}

static INLINE struct ieee80211_rateset *
ieee80211com_get_xr_rates(struct ieee80211com *ic)
{
    return &ic->ic_sup_xr_rates;
}

static INLINE struct ieee80211_rateset *
ieee80211com_get_half_rates(struct ieee80211com *ic)
{
    return &ic->ic_sup_half_rates;
}

static INLINE struct ieee80211_rateset *
ieee80211com_get_quarter_rates(struct ieee80211com *ic)
{
    return &ic->ic_sup_quarter_rates;
}

/*
 * Return beacon interval of the AP or ADHOC mode
 */
static INLINE u_int16_t
ieee80211com_get_beacon_interval(struct ieee80211com *ic)
{
    return ic->ic_intval;
}

static INLINE u_int16_t
ieee80211com_get_bmiss_timeout(struct ieee80211com *ic)
{
    return ic->ic_bmisstimeout;
}

/*
 * 11n
 */
static INLINE void
ieee80211com_set_htcap(struct ieee80211com *ic, u_int16_t htcap)
{
    ic->ic_htcap |= htcap;
}

static INLINE void
ieee80211com_clear_htcap(struct ieee80211com *ic, u_int16_t htcap)
{
    ic->ic_htcap &= ~htcap;
}

static INLINE int
ieee80211com_has_htcap(struct ieee80211com *ic, u_int16_t htcap)
{
    return ((ic->ic_htcap & htcap) != 0);
}

static INLINE void
ieee80211com_set_htextcap(struct ieee80211com *ic, u_int16_t htextcap)
{
    ic->ic_htextcap |= htextcap;
}

static INLINE void
ieee80211com_clear_htextcap(struct ieee80211com *ic, u_int16_t htextcap)
{
    ic->ic_htextcap &= ~htextcap;
}

static INLINE int
ieee80211com_has_htextcap(struct ieee80211com *ic, u_int16_t htextcap)
{
    return ((ic->ic_htextcap & htextcap) != 0);
}

static INLINE void
ieee80211com_set_htflags(struct ieee80211com *ic, u_int16_t htflags)
{
    ic->ic_htflags |= htflags;
}

static INLINE void
ieee80211com_clear_htflags(struct ieee80211com *ic, u_int16_t htflags)
{
    ic->ic_htflags &= ~htflags;
}

static INLINE int
ieee80211com_has_htflags(struct ieee80211com *ic, u_int16_t htflags)
{
    return ((ic->ic_htflags & htflags) != 0);
}

static INLINE u_int8_t
ieee80211com_get_maxampdu(struct ieee80211com *ic)
{
    return ic->ic_maxampdu;
}

static INLINE void
ieee80211com_set_maxampdu(struct ieee80211com *ic, u_int8_t maxampdu)
{
    ic->ic_maxampdu = maxampdu;
}

static INLINE u_int8_t
ieee80211com_get_mpdudensity(struct ieee80211com *ic)
{
    return ic->ic_mpdudensity;
}

static INLINE void
ieee80211com_set_mpdudensity(struct ieee80211com *ic, u_int8_t mpdudensity)
{
    ic->ic_mpdudensity = mpdudensity;
}

static INLINE u_int8_t
ieee80211com_get_weptkipaggr_rxdelim(struct ieee80211com *ic)
{
    return ic->ic_weptkipaggr_rxdelim;
}

static INLINE void
ieee80211com_set_weptkipaggr_rxdelim(struct ieee80211com *ic, u_int8_t weptkipaggr_rxdelim)
{
    ic->ic_weptkipaggr_rxdelim = weptkipaggr_rxdelim;
}

/*
 * Misc
 */
static INLINE void
ieee80211com_set_macaddr(struct ieee80211com *ic, u_int8_t *macaddr)
{
    OS_MEMCPY(ic->ic_myaddr, macaddr, IEEE80211_ADDR_LEN);
}

static INLINE const u_int8_t *
ieee80211com_get_macaddr(struct ieee80211com *ic)
{
    return ic->ic_myaddr;
}

static INLINE enum ieee80211_protmode
ieee80211com_get_protmode(struct ieee80211com *ic)
{
    return ic->ic_protmode;
}

static INLINE u_int16_t
ieee80211com_get_txpowerlimit(struct ieee80211com *ic)
{
    return ic->ic_txpowlimit;
}

static INLINE void
ieee80211com_set_txpowerlimit(struct ieee80211com *ic, u_int16_t txpowlimit)
{
    ic->ic_txpowlimit = txpowlimit;
}

static INLINE int
ieee80211com_get_ampdu_limit(struct ieee80211com *ic)
{
    return ic->ic_ampdu_limit;
}

static INLINE void
ieee80211com_set_ampdu_limit(struct ieee80211com *ic, int limit)
{
    ic->ic_ampdu_limit = limit;
}

static INLINE int
ieee80211com_get_ampdu_subframes(struct ieee80211com *ic)
{
    return ic->ic_ampdu_subframes;
}

static INLINE void
ieee80211com_set_ampdu_subframes(struct ieee80211com *ic, int subframes)
{
    ic->ic_ampdu_subframes = subframes;
}

static INLINE u_int8_t
ieee80211com_get_tx_chainmask(struct ieee80211com *ic)
{
    return ic->ic_tx_chainmask;
}

static INLINE void
ieee80211com_set_tx_chainmask(struct ieee80211com *ic, u_int8_t chainmask)
{
    ic->ic_tx_chainmask = chainmask;
}

static INLINE u_int8_t
ieee80211com_get_rx_chainmask(struct ieee80211com *ic)
{
    return ic->ic_rx_chainmask;
}

static INLINE void
ieee80211com_set_rx_chainmask(struct ieee80211com *ic, u_int8_t chainmask)
{
    ic->ic_rx_chainmask = chainmask;
}

#define ieee80211com_first_vap(_ic)         \
    TAILQ_FIRST(&(_ic)->ic_vaps)

#define ieee80211_enumerate_vaps(_vap, _ic)  \
    TAILQ_FOREACH(_vap, &(_ic)->ic_vaps, iv_next)

/*
 * ************************************
 * IEEE80211VAP Interfaces
 * ************************************
 */

/*
 * Capabilities
 */
static INLINE void
ieee80211vap_set_cap(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_caps |= cap;
}

static INLINE void
ieee80211vap_clear_cap(struct ieee80211vap *vap, u_int32_t cap)
{
    vap->iv_caps &= ~cap;
}

static INLINE int
ieee80211vap_has_cap(struct ieee80211vap *vap, u_int32_t cap)
{
    return ((vap->iv_caps & cap) != 0);
}

/*
 * Atheros Capabilities
 */
static INLINE void
ieee80211vap_set_athcap(struct ieee80211vap *vap, u_int32_t athcap)
{
    vap->iv_ath_cap |= athcap;
}

static INLINE void
ieee80211vap_clear_athcap(struct ieee80211vap *vap, u_int32_t athcap)
{
    vap->iv_ath_cap &= ~athcap;
}

static INLINE int
ieee80211vap_has_athcap(struct ieee80211vap *vap, u_int32_t athcap)
{
    return ((vap->iv_ath_cap & athcap) != 0);
}

static INLINE void
ieee80211vap_set_flag(struct ieee80211vap *vap, u_int32_t flag)
{
    vap->iv_flags |= flag;
}

static INLINE void
ieee80211vap_clear_flag(struct ieee80211vap *vap, u_int32_t flag)
{
    vap->iv_flags &= ~flag;
}

static INLINE int
ieee80211vap_has_flag(struct ieee80211vap *vap, u_int32_t flag)
{
    return ((vap->iv_flags & flag) != 0);
}

/*
 * IC extended flags
 */
static INLINE void
ieee80211vap_set_extflag(struct ieee80211vap *vap, u_int32_t extflag)
{
    vap->iv_flags_ext |= extflag;
}

static INLINE void
ieee80211vap_clear_extflag(struct ieee80211vap *vap, u_int32_t extflag)
{
    vap->iv_flags_ext &= ~extflag;
}

static INLINE int
ieee80211vap_has_extflag(struct ieee80211vap *vap, u_int32_t extflag)
{
    return ((vap->iv_flags_ext & extflag) != 0);
}

/*
 * Misc
 */
static INLINE enum ieee80211_opmode
ieee80211vap_get_opmode(struct ieee80211vap *vap)
{
    return vap->iv_opmode;
}

static INLINE enum ieee80211_state
ieee80211vap_get_state(struct ieee80211vap *vap)
{
    return vap->iv_state;
}

static INLINE struct ieee80211com *
ieee80211vap_get_common(struct ieee80211vap *vap)
{
    return vap->iv_ic;
}

static INLINE void
ieee80211vap_get_macaddr(struct ieee80211vap *vap, u_int8_t *macaddr)
{
    OS_MEMCPY(macaddr, vap->iv_myaddr, IEEE80211_ADDR_LEN);
}

static INLINE void
ieee80211vap_set_macaddr(struct ieee80211vap *vap, u_int8_t *macaddr)
{
    /* Normally shouldn't be called for a station */
    OS_MEMCPY(vap->iv_myaddr, macaddr, IEEE80211_ADDR_LEN);
}

static INLINE struct ieee80211_node *
ieee80211vap_get_bssnode(struct ieee80211vap *vap)
{
    return vap->iv_bss;
}

static INLINE u_int16_t
ieee80211vap_get_rtsthreshold(struct ieee80211vap *vap)
{
    return vap->iv_rtsthreshold;
}

static INLINE u_int16_t
ieee80211vap_get_fragthreshold(struct ieee80211vap *vap)
{
    return vap->iv_fragthreshold;
}

/* XXX: only used for XR so far */
static INLINE void
ieee80211vap_set_fragthreshold(struct ieee80211vap *vap, u_int16_t fragthreshold)
{
    vap->iv_fragthreshold = fragthreshold;
}

static INLINE u_int8_t
ieee80211vap_get_dtimcount(struct ieee80211vap *vap)
{
    return vap->iv_dtim_count;
}

static INLINE u_int8_t
ieee80211vap_get_dtimperiod(struct ieee80211vap *vap)
{
    return vap->iv_dtim_period;
}

static INLINE int
ieee80211vap_has_pssta(struct ieee80211vap *vap)
{
    return (vap->iv_ps_sta != 0);
}

static INLINE int
ieee80211vap_get_mcast_rate(struct ieee80211vap *vap)
{
    return vap->iv_mcast_rate;
}

static INLINE int
ieee80211vap_get_fixed_rate(struct ieee80211vap *vap)
{
    return (vap->iv_fixed_rateset & 0xff);
}

/*
 * ************************************
 * IEEE80211_NODE Interfaces
 * ************************************
 */
static INLINE struct ieee80211com *
ieee80211node_get_common(struct ieee80211_node *ni)
{
    return ni->ni_ic;
}

static INLINE struct ieee80211vap *
ieee80211node_get_vap(struct ieee80211_node *ni)
{
    return ni->ni_vap;
}

static INLINE void
ieee80211node_set_flag(struct ieee80211_node *ni, u_int16_t flag)
{
    ni->ni_flags |= flag;
}

static INLINE void
ieee80211node_clear_flag(struct ieee80211_node *ni, u_int16_t flag)
{
    ni->ni_flags &= ~flag;
}

static INLINE int
ieee80211node_has_flag(struct ieee80211_node *ni, u_int16_t flag)
{
    return ((ni->ni_flags & flag) != 0);
}

#define IEEE80211_NODE_USEAMPDU(_ni)                         \
     ( (ieee80211node_has_flag((_ni), IEEE80211_NODE_HT)) && \
       (ieee80211vap_has_extflag(ni->ni_vap, IEEE80211_FEXT_AMPDU)) && \
       (ieee80211_node_wdswar_isaggrdeny(_ni) == 0 ) && \
       (((_ni)->ni_flags & IEEE80211_NODE_NOAMPDU)== 0))

#define IEEE80211_NODE_AC_UAPSD_ENABLED(_ni, _ac) ((_ni)->ni_uapsd_ac_delivena[(_ac)])

static INLINE void
ieee80211node_set_athflag(struct ieee80211_node *ni, u_int8_t flag)
{
    ni->ni_ath_flags |= flag;
}

static INLINE void
ieee80211node_clear_athflag(struct ieee80211_node *ni, u_int8_t flag)
{
    ni->ni_ath_flags &= ~flag;
}

static INLINE int
ieee80211node_has_athflag(struct ieee80211_node *ni, u_int8_t flag)
{
    return ((ni->ni_ath_flags & flag) != 0);
}

static INLINE u_int16_t
ieee80211node_get_txpower(struct ieee80211_node *ni)
{
    return ni->ni_txpower;
}

static INLINE void
ieee80211node_set_txpower(struct ieee80211_node *ni, u_int16_t txpower)
{
    ni->ni_txpower = txpower;
}

static INLINE u_int8_t
ieee80211node_get_rssi(struct ieee80211_node *ni)
{
    return ni->ni_rssi;
}

static INLINE int
ieee80211node_get_txrate(struct ieee80211_node *ni)
{
    return ni->ni_txrate;
}

static INLINE void
ieee80211node_set_txrate(struct ieee80211_node *ni, int txrate)
{
    ni->ni_txrate = txrate;
}

static INLINE struct ieee80211_rateset *
ieee80211node_get_rateset(struct ieee80211_node *ni)
{
    return &ni->ni_rates;
}

/*
 * Return the beacon interval of associated BSS
 */
static INLINE u_int16_t
ieee80211node_get_beacon_interval(struct ieee80211_node *ni)
{
    return ni->ni_intval;
}

static INLINE u_int8_t *
ieee80211node_get_tstamp(struct ieee80211_node *ni)
{
    return ni->ni_tstamp.data;
}

static INLINE u_int64_t
ieee80211node_get_tsf(struct ieee80211_node *ni)
{
    return ni->ni_tstamp.tsf;
}

static INLINE u_int16_t
ieee80211node_get_associd(struct ieee80211_node *ni)
{
    return ni->ni_associd;
}

static INLINE u_int16_t
ieee80211node_get_capinfo(struct ieee80211_node *ni)
{
    return ni->ni_capinfo;
}

static INLINE int
ieee80211node_has_cap(struct ieee80211_node *ni, u_int16_t cap)
{
    return ((ni->ni_capinfo & cap) != 0);
}

static INLINE u_int8_t *
ieee80211node_get_bssid(struct ieee80211_node *ni)
{
    return ni->ni_bssid;
}

static INLINE u_int8_t *
ieee80211_node_macaddr_get(struct ieee80211_node *ni)
{
    return ni->ni_macaddr;
}

/*
 * 11n
 */
static INLINE void
ieee80211_node_set_htcap(struct ieee80211_node *ni, u_int16_t htcap)
{
    ni->ni_htcap |= htcap;
}

static INLINE void
ieee80211_node_clear_htcap(struct ieee80211_node *ni, u_int16_t htcap)
{
    ni->ni_htcap &= ~htcap;
}

static INLINE int
ieee80211_node_has_htcap(struct ieee80211_node *ni, u_int16_t htcap)
{
    return ((ni->ni_htcap & htcap) != 0);
}

static INLINE u_int16_t
ieee80211_node_get_maxampdu(struct ieee80211_node *ni)
{
    return ni->ni_maxampdu;
}


/* XXX */
#define ieee80211_iterate_node(_ic, _func, _arg)	\
	ieee80211_iterate_nodes(&(_ic)->ic_sta, _func, _arg)

#define ieee80211_iterate_node_nolock(_ic, _func, _arg)	\
    ieee80211_iterate_nodes_nolock(&(_ic)->ic_sta,  _func, _arg)

/*
 * MISC
 */
static INLINE struct wmeParams *
ieee80211_wmm_chanparams(struct ieee80211com *ic, int ac)
{
    return &ic->ic_wme.wme_chanParams.cap_wmeParams[ac];
}

#define ieee80211_complete_wbuf(_wbuf, _ts, _err) do {        \
    struct ieee80211_node *ni = wbuf_get_node(_wbuf);   \
    if(!_err)                                           \
        wbuf_complete(_wbuf);                           \
    if (ni != NULL) {                                    \
        ieee80211_free_node(ni);                        \
    }                                                    \
} while (0)

#define IEEE80211_TX_COMPLETE_WITH_ERROR(_wbuf, _err) \
    ieee80211_complete_wbuf(_wbuf, NULL, _err)

#define IEEE80211NODE_WDSWAR_ISSENDDELBA(_ni)   \
    ( ieee80211_node_wdswar_issenddelba(_ni) )

#endif /* end of _ATH_STA_IEEE80211_INTERFACES_H */
