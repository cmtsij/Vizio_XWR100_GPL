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

#ifndef _ATH_LIMIT_LEGACY_H
#define _ATH_LIMIT_LEGACY_H

#include "osdep.h"

struct ath_softc_net80211;

/* See bug 38437*/
#define LIMIT_LEGACY_TIMEOUT_MS 100

#define	ATH_LIMIT_LEGACY_TXQ_LOCK_INIT(_scn)    spin_lock_init(&(_scn)->sc_limit_legacy_txq_lock)
#define	ATH_LIMIT_LEGACY_TXQ_LOCK_DESTROY(_scn) 
#define	ATH_LIMIT_LEGACY_TXQ_LOCK(_scn)         spin_lock(&(_scn)->sc_limit_legacy_txq_lock)
#define	ATH_LIMIT_LEGACY_TXQ_UNLOCK(_scn)       spin_unlock(&(_scn)->sc_limit_legacy_txq_lock)

#define	ATH_LIMIT_LEGACY_FREEQ_LOCK_INIT(_scn)    spin_lock_init(&(_scn)->sc_limit_legacy_freeq_lock)
#define	ATH_LIMIT_LEGACY_FREEQ_LOCK_DESTROY(_scn) 
#define	ATH_LIMIT_LEGACY_FREEQ_LOCK(_scn)         spin_lock(&(_scn)->sc_limit_legacy_freeq_lock)
#define	ATH_LIMIT_LEGACY_FREEQ_UNLOCK(_scn)       spin_unlock(&(_scn)->sc_limit_legacy_freeq_lock)

struct ath_limit_legacy_buf {
    TAILQ_ENTRY(ath_limit_legacy_buf) qelem; /* round-robin 38437 tx entry */
    wbuf_t wb;
};


OS_TIMER_FUNC(limit_legacy_flush_timeout);
int ath_limit_legacy_attach(struct ath_softc_net80211 *scn);
void ath_limit_legacy_detach(struct ath_softc_net80211 *scn);
void enq_legacy_wbuf(struct ath_softc_net80211 *scn, wbuf_t wb);
int deq_legacy_wbuf(struct ath_softc_net80211 *scn);
int ath_limit_legacy_create_freeq(struct ath_softc_net80211 *scn);
int ath_limit_legacy_destroy_freeq(struct ath_softc_net80211 *scn);

#endif
