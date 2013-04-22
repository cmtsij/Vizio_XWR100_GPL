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
 *  Rifs Burst (RB)
 */

#ifndef _DEV_ATH_RB_H
#define _DEV_ATH_RB_H

#define ATH_RB_NUM_SEQS        2

typedef enum {
    ATH_RB_RESET = 1,         /* start all over */
    ATH_RB_RX_SEQ,            /* new seq        */
    ATH_RB_ACTIVATE,          /* activate workaround */
    ATH_RB_RESTORE_TIMEOUT,   /* time to switch back */
} ath_rb_event_t;

typedef void (*ath_rb_state_fn_t)(void *rb, ath_rb_event_t event);

typedef struct {
    ath_rb_state_fn_t       fn;
    void                  *arg;
} ath_rb_sm_t;

typedef struct ath_rb {
    struct ath_softc        *sc;
    struct ath_timer	    restore_timer;
    ath_rb_sm_t             sm;
    u_int16_t               seqs[ATH_RB_NUM_SEQS];
    u_int8_t                skip_cnt;
} ath_rb_t;

#ifdef ATH_RB
/* RB detection routines */
void ath_rb_detect(ath_rb_t *rb, struct ieee80211_qosframe *whqos);
void ath_rb_init(struct ath_softc *sc);
void ath_rb_reset(struct ath_softc *sc);
void ath_rb_set(struct ath_softc *, int enable);
#endif

#define ATH_RB_SET_STATE(_rb, _fn)      \
    ((_rb)->sm.fn = _fn)

#define ATH_RB_SET_ARG(_rb, _arg)       \
    ((_rb)->sm.arg = _arg)

#define ATH_RB_GET_ARG(_rb)             \
    ((_rb)->sm.arg)

#define ATH_RB_SEND_EVENT(_rb, _event) do {\
    if ((_rb)->sm.fn)                  \
        (((_rb)->sm.fn)(_rb, _event)); \
    else                               \
        ASSERT(0);                     \
} while (0)

#define ATH_RB_QUEUE_SEQ(_rb, _seq) do {\
    (_rb)->seqs[0] = (_rb)->seqs[1];    \
    (_rb)->seqs[1] = _seq;              \
} while (0)

#endif /* _DEV_ATH_RB_H */
