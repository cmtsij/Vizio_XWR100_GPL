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
#ifndef _ATH_FF_H_
#define _ATH_FF_H_

/*
 * Fast Frame
 */
#define	ATH_FF_LOCK_INIT(_aff)    spin_lock_init(&(_aff)->aff_lock)
#define	ATH_FF_LOCK_DESTROY(_aff)
#define	ATH_FF_LOCK(_aff)         spin_lock(&(_aff)->aff_lock) 
#define	ATH_FF_UNLOCK(_aff)       spin_unlock(&(_aff)->aff_lock)

/*
 * External Definitions 
 */
struct ath_ff {
    wbuf_t      aff_tx_buf[WME_NUM_AC]; /* ff staging area */
    spinlock_t  aff_lock;               /* ff spinlock */
};

int  ath_ff_attach(struct ath_softc_net80211 *scn, struct ath_node_net80211 *anode);
void ath_ff_detach(struct ath_node_net80211 *anode);
wbuf_t ath_ff(wbuf_t wbuf);
void ath_ff_stageq_flush(struct ieee80211_node *ni, int priority);
                               

#endif /* _ATH_FF_H_ */
