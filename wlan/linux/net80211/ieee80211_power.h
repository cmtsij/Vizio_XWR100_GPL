/*-
 * Copyright (c) 2001 Atsushi Onoe
 * Copyright (c) 2002-2005 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD: src/sys/net80211/ieee80211_node.h,v 1.15 2005/04/04 04:27:20 sam Exp $
 */
#ifndef _NET80211_IEEE80211_POWER_H_
#define _NET80211_IEEE80211_POWER_H_

#define IEEE80211_PWRSAVE_TIMER_INTERVAL   200 /* 200 msec */
#define IEEE80211_PS_INACTIVITYTIME        2000  /* pwrsave inactivity time in sta mode (in msec) */

#define IEEE80211_PWRSAVE_SUCCESS          0
#define IEEE80211_PWRSAVE_FAIL             -1

/*
 * station power save mode.
 */
typedef enum ieee80211_psmode {
	IEEE80211_PWRSAVE_NONE=0,
	IEEE80211_PWRSAVE_LOW,
	IEEE80211_PWRSAVE_NORMAL,
	IEEE80211_PWRSAVE_MAXIMUM
} IEEE80211_PWRSAVE_MODE;

/*
 * station power save state.
 */
typedef enum {
    IEEE80211_PWRSAVE_INIT=0,
    IEEE80211_PWRSAVE_AWAKE,
    IEEE80211_PWRSAVE_FULL_SLEEP,
    IEEE80211_PWRSAVE_NETWORK_SLEEP,
    IEEE80211_PWRSAVE_FAKE_SLEEP
} IEEE80211_PWRSAVE_STATE;             

/*
 * station power save data structure.
 */
struct ieee80211_pwrsave {
	IEEE80211_PWRSAVE_MODE  ips_sta_psmode;
	IEEE80211_PWRSAVE_STATE ips_state;
	u_int32_t               ips_presleep; 
	u_int32_t               ips_connected; 
	u_int32_t               ips_fakesleep_pend;
	u_int32_t               ips_fullsleep_enable;
	struct timer_list       ips_timer;
	int32_t                 ips_timercnt;
	u_int32_t               ips_inactivitytime;
	spinlock_t              ips_lock; 
	u_int32_t               ips_max_sleeptime;         /* station wakes after this many mS in max power save mode */
	u_int32_t               ips_normal_sleeptime;      /* station wakes after this many mS in normal power save mode */
	u_int32_t               ips_low_sleeptime;         /* station wakes after this many mS in low power save mode */
	u_int32_t               ips_max_inactivitytime;    /* in max PS mode, how long (in mS) w/o Tx/Rx before going back to sleep */
	u_int32_t               ips_normal_inactivitytime; /* in mornal PS mode, how long (in mS) w/o Tx/Rx before going back to sleep */
	u_int32_t               ips_low_inactivitytime;    /* in low PS mode, how long (in mS) w/o Tx/Rx before going back to sleep */
};

/*
 * Locking interface for power
 */
#define IEEE80211_PWRSAVE_LOCK_INIT(_vap)       spin_lock_init(&(_vap)->iv_pwrsave.ips_lock)
#define	IEEE80211_PWRSAVE_LOCK_DESTROY(_vap)
#define IEEE80211_PWRSAVE_LOCK(_vap)            spin_lock(&(_vap)->iv_pwrsave.ips_lock)
#define IEEE80211_PWRSAVE_UNLOCK(_vap)          spin_unlock(&(_vap)->iv_pwrsave.ips_lock)

/*
 * power save sleep reason
 */
typedef enum {
	SCAN_COMPLETE=0,
	FAKE_SLEEP,
} IEEE80211_PWRSAVE_SLEEP_REASON; 

/*
 * power save wakeup reason
 */
typedef enum {
	START_SCAN=0,
	START_CONNECT,
	TRANSMIT,
	RECEIVE,
	FAKE_WAKEUP
} IEEE80211_PWRSAVE_WAKEUP_REASON; 

struct ieee80211com;
struct ieee80211vap;

void	ieee80211_power_attach(struct ieee80211com *);
void	ieee80211_power_detach(struct ieee80211com *);
void	ieee80211_power_vattach(struct ieee80211vap *);
void	ieee80211_power_latevattach(struct ieee80211vap *);
void	ieee80211_power_vdetach(struct ieee80211vap *);

struct ieee80211_node;

int	ieee80211_node_saveq_drain(struct ieee80211_node *);
int	ieee80211_node_saveq_age(struct ieee80211_node *,
                                 struct sk_buff_head *);
void	ieee80211_pwrsave(struct ieee80211_node *, struct sk_buff *);
void	ieee80211_node_pwrsave(struct ieee80211_node *, int enable);
void	ieee80211_sta_pwrsave(struct ieee80211vap *, int enable);

/* initialize functions */
void	ieee80211_pwrsave_init(struct ieee80211vap *);

/* common api for other modules */
void	ieee80211_pwrsave_wakeup(struct ieee80211vap *vap, int reson);
int		ieee80211_pwrsave_sleep(struct ieee80211vap *vap, int reson);
void	ieee80211_pwrsave_disable_fullsleep(struct ieee80211vap *vap);
void	ieee80211_pwrsave_connect(struct ieee80211vap *vap);
void	ieee80211_pwrsave_disconnect(struct ieee80211vap *vap);

/* misc functions */
int		ieee80211_pwrsave_cannot_send(struct ieee80211vap *vap);
void	ieee80211_pwrsave_complete_wbuf(struct ieee80211vap *vap, u_int32_t );
void	ieee80211_pwrsave_rcv_lastbmcast(struct ieee80211vap *vap);
void	ieee80211_pwrsave_proc_tim(struct ieee80211com *ic);
void	ieee80211_pwrsave_proc_dtim(struct ieee80211com *ic);
void	ieee80211_pwrsave_txq_empty(struct ieee80211com *ic);

//Macro to call ieee80211_pwrsave_complete_wbuf
#define PWRSAVE_COMPLETE_WBUF(_vap,status) ieee80211_pwrsave_complete_wbuf(_vap, status);

/* get info functions */
u_int32_t	ieee80211_pwrsave_get_max_sleeptime(struct ieee80211vap *vap);
u_int32_t	ieee80211_pwrsave_get_normal_sleeptime(struct ieee80211vap *vap);
u_int32_t	ieee80211_pwrsave_get_low_sleeptime(struct ieee80211vap *vap);
u_int32_t	ieee80211_pwrsave_get_max_inactivitytime(struct ieee80211vap *vap);
u_int32_t	ieee80211_pwrsave_get_normal_inactivitytime(struct ieee80211vap *vap);
u_int32_t	ieee80211_pwrsave_get_low_inactivitytime(struct ieee80211vap *vap);
void		ieee80211_pwrsave_set_mode(struct ieee80211vap *, u_int32_t mode);
int			ieee80211_pwrsave_get_mode(struct ieee80211vap *vap);

int ieee80211_pwrsave_fakesleep(struct ieee80211vap *vap);
void		ieee80211_pwrsave_timer(unsigned long);

#endif /* _NET80211_IEEE80211_POWER_H_ */
