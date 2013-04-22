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

 * $Id: //depot/sw/releases/7.3_AP/wlan/linux/net80211/ieee80211_me.h#1 $
 */

#ifndef _ATH_IQUE_ME_H_
#define _ATH_IQUE_ME_H_

#ifdef IEEE80211_MCAST_ENHANCEMENT
#include <linux/wireless.h>


//#include "ah.h"
//#include <ath_dev.h>
/*
 * If Linux can't tell us if irqs are disabled, then they are not.
 */ 
#ifndef irqs_disabled
#define irqs_disabled()     (0)
#endif

/*
 * Table locking definitions.
 */
typedef rwlock_t ieee80211_snoop_lock_t;
#define	IEEE80211_SNOOP_LOCK_INIT(_st, _name)	\
	rwlock_init(&(_st)->msl_lock)
#define	IEEE80211_SNOOP_LOCK_DESTROY(_st)
#define	IEEE80211_SNOOP_LOCK(_st)				\
	write_lock(&(_st)->msl_lock)
#define	IEEE80211_SNOOP_UNLOCK(_st)				\
	write_unlock(&(_st)->msl_lock)
#define	IEEE80211_SNOOP_LOCK_BH(_st)			\
	if (irqs_disabled()) {						\
		write_lock(&(_st)->msl_lock);			\
	} else {									\
		write_lock_bh(&(_st)->msl_lock); 		\
	}
#define	IEEE80211_SNOOP_UNLOCK_BH(_st)			\
	if (irqs_disabled()) {						\
		write_unlock(&(_st)->msl_lock);			\
	} else {									\
		write_unlock_bh(&(_st)->msl_lock);		\
	}

#define	IEEE80211_ME_LOCK_INIT(_vap)    spin_lock_init(&(_vap)->iv_me->ieee80211_melock)
#define	IEEE80211_ME_LOCK_DESTROY(_vap)
#define	IEEE80211_ME_LOCK(_vap)         spin_lock(&(_vap)->iv_me->ieee80211_melock)
#define	IEEE80211_ME_UNLOCK(_vap)       spin_unlock(&(_vap)->iv_me->ieee80211_melock)


#ifndef MAX_SNOOP_ENTRIES
#define MAX_SNOOP_ENTRIES	32	/* max number*/
#endif
#ifndef DEF_SNOOP_ENTRIES
#define DEF_SNOOP_ENTRIES	8	/* default list length */
#endif

#define IGMP_SNOOP_CMD_OTHER 0
#define IGMP_SNOOP_CMD_JOIN  1
#define IGMP_SNOOP_CMD_LEAVE 2

#define DEF_ME_TIMER	30000	/* timer interval as 30 secs */
#define DEF_ME_TIMEOUT	120000	/* 2 minutes for timeout  */

#define IQUE_ME_MEMTAG 'metag'
#define IQUE_ME_OPS_MEMTAG 'meopstag'

#define GRPADDR_FILTEROUT_N 1

static const u_int8_t grpaddr_filterout[GRPADDR_FILTEROUT_N][IEEE80211_ADDR_LEN] =
{
		{0, 0, 0, 0x7F, 0xFF, 0xFA}
};

/* TODO: Demo uses single combo list, not optimized */
/* list entry */
struct MC_GROUP_LIST_ENTRY {
    u_int8_t							mgl_group_addr[IEEE80211_ADDR_LEN];
    u_int8_t							mgl_group_member[IEEE80211_ADDR_LEN];
    u_int32_t							mgl_timestamp;
	unsigned int						mgl_xmited;
	struct ieee80211_node				*mgl_ni;
	TAILQ_ENTRY(MC_GROUP_LIST_ENTRY)	mgl_list;
};

/* global (for demo only) struct to manage snoop */
struct MC_SNOOP_LIST {
	u_int16_t							msl_group_list_count;
	u_int16_t							msl_misc;
	u_int16_t							msl_max_length;
    ieee80211_snoop_lock_t				msl_lock;	/* lock snoop table */
    TAILQ_HEAD(MSL_HEAD_TYPE, MC_GROUP_LIST_ENTRY)	msl_node;	/* head of list of all snoop entries */
};

struct ieee80211vap;
struct ieee80211_node;

struct ieee80211_ique_me {
	struct ieee80211vap *me_iv;	/* Backward pointer to vap instance */
	struct MC_SNOOP_LIST ieee80211_me_snooplist;
	int ieee80211_mc_snoop_debug;
	struct timer_list snooplist_timer;
	spinlock_t	ieee80211_melock;
};


void ieee80211_me_SnoopListDump(struct ieee80211vap *);
void ieee80211_me_SnoopInspecting(struct ieee80211_node *, struct sk_buff *);
void ieee80211_me_SnoopWDSNodeCleanup(struct ieee80211_node *);
int  ieee80211_me_SnoopConvert(struct ieee80211vap *, struct sk_buff *);
int  ieee80211_me_attach(struct ieee80211vap *);
void ieee80211_me_detach(struct ieee80211vap *);


#endif /*IEEE80211_MCAST_ENHANCEMENT*/
#endif /*_ATH_IQUE_ME_H_*/

