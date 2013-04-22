/*-
 * Copyright (c) 2003-2008 Atheros Communications, Inc.
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

 * $Id: //depot/sw/releases/7.3_AP/wlan/linux/net80211/ieee80211_hbr.h#1 $
 */

/*
 * Timer function and state machine for Headline Block Removal as a part of iQue
 * For design details, please refer to the design document
 */

#ifndef _ATH_NET80211_HBR_H_
#define _ATH_NET80211_HBR_H_

#ifdef ATH_SUPPORT_IQUE


/*
 * If Linux can't tell us if irqs are disabled, then they are not.
 */ 
#ifndef irqs_disabled
#define irqs_disabled()     (0)
#endif

/*
 * Table locking definitions.
 */
typedef rwlock_t ieee80211_hbr_lock_t;
#define	IEEE80211_HBR_LOCK_INIT(_st, _name)		\
	rwlock_init(&(_st)->hbr_lock)
#define	IEEE80211_HBR_LOCK_DESTROY(_st)
#define	IEEE80211_HBR_LOCK(_st)					\
	write_lock(&(_st)->hbr_lock)
#define	IEEE80211_HBR_UNLOCK(_st)				\
	write_unlock(&(_st)->hbr_lock)
#define	IEEE80211_HBR_LOCK_BH(_st)				\
	if (irqs_disabled()) {						\
		write_lock(&(_st)->hbr_lock);			\
	} else {									\
		write_lock_bh(&(_st)->hbr_lock); 		\
	}
#define	IEEE80211_HBR_UNLOCK_BH(_st)			\
	if (irqs_disabled()) {						\
		write_unlock(&(_st)->hbr_lock);			\
	} else {									\
		write_unlock_bh(&(_st)->hbr_lock);		\
	}

#define DEF_HBR_TIMEOUT				2000 /* 2 seconds */
/*
 * HBR_TRIGGER_(BACK/FORWARD/STALL) is a tri-state trigger
 * for state machine of headline block removal.
 * BACK: mean to trigger the state from ACTIVE to BLOCKING
 * FORWARD: means to trigger the state from PROBING to ACTIVE
 * STALL: means the trigger is in high resistor state, no trigging event
 */
#define HBR_TRIGGER_FORWARD	1
#define HBR_TRIGGER_BACK	-1
#define HBR_TRIGGER_STALL	0

#define HBR_SIGNAL_ACTIVE		1
#define HBR_SIGNAL_BLOCKING		2
#define HBR_SIGNAL_PROBING		3

struct ieee80211vap;

/*
 * structure of state machine for headline block removal
 */

/* Entry for each node and state machine (per MAC address) */
struct ieee80211_hbr_sm {
	struct ieee80211vap * hbr_vap;		/* backpointer to vap */	
	u_int8_t	hbr_addr[IEEE80211_ADDR_LEN];	/* node's mac addr */
	int8_t		hbr_trigger;	/* a tri-state switch for state machine */
	u_int8_t	hbr_block;		/* block outgoing traffic ? */
	enum {
		HBR_SM_ACTIVE, HBR_SM_BLOCKING, HBR_SM_PROBING
	} hbr_sm_state;				/* Headline block removal states */	
	TAILQ_ENTRY(ieee80211_hbr_sm)	hbr_list;	
};
#define STATE_MACHINE_DATA struct ieee80211_hbr_sm

/* A list for all nodes (and state machines) */
struct ieee80211_hbr_list {
	struct ieee80211vap *hbrlist_vap;	/* backpointer to vap */	
	u_int16_t	hbr_count;
	ieee80211_hbr_lock_t	hbr_lock;
	TAILQ_HEAD(HBR_HEAD_TYPE, ieee80211_hbr_sm) hbr_node;
};

/* this type of function will be called while iterating the state machine list for each entry,
 * with two arguments, and the return value indicate to break (if return 1) or continue (if return 0)
 * iterating 
 */
typedef int ieee80211_hbr_iterate_func(struct ieee80211_hbr_sm *, void *, void *);

/*
 * Declare timer for headline block removal
 */
void ieee80211_hbr_timer(unsigned long);
void ieee80211_hbr_setstate_all(struct ieee80211vap *, int);
void ieee80211_hbr_getstate_all(struct ieee80211vap *);
struct ieee80211_hbr_sm * ieee80211_hbr_find_byaddr(struct ieee80211vap *vap, u_int8_t *addr);
void ieee80211_hbr_iterate(struct ieee80211vap *vap, ieee80211_hbr_iterate_func *f, void *arg1, void *arg2);
void ieee80211_hbr_addentry(struct ieee80211vap *vap, u_int8_t *addr);
void ieee80211_hbr_delentry(struct ieee80211vap *vap, u_int8_t *addr);
void ieee80211_hbr_init(struct ieee80211vap *vap);
void ieee80211_hbr_settrigger_byaddr(struct ieee80211vap *vap, u_int8_t *addr, int signal);
int ieee80211_hbr_reportstate_byaddr(struct ieee80211vap *vap, u_int8_t *addr);
#endif /* ATH_SUPPORT_IQUE */

#endif /* _ATH_NET80211_HBR_H_ */


