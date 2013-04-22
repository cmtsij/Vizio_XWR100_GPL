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

#ifndef _PKTLOG_HAL_H_
#define _PKTLOG_HAL_H_

#include "pktlog_fmt.h"

/* Parameter types for packet logging driver hooks */
struct log_ani {
    u_int8_t phyStatsDisable;
    u_int8_t noiseImmunLvl;
    u_int8_t spurImmunLvl;
    u_int8_t ofdmWeakDet;
    u_int8_t cckWeakThr;
    u_int16_t firLvl;
    u_int16_t listenTime;
    u_int32_t cycleCount;
    u_int32_t ofdmPhyErrCount;
    u_int32_t cckPhyErrCount;
    int8_t rssi;
    int32_t misc[8];            /* Can be used for HT specific or other misc info */
    /* TBD: Add other required log information */
};

struct ath_pktlog_halfuncs {
    void (*pktlog_ani) (HAL_SOFTC, struct log_ani *, u_int16_t);
};

#define ath_log_ani(_sc, _log_data, _flags)                         \
    do {                                                            \
        if (g_pktlog_halfuncs) {                                    \
            g_pktlog_halfuncs->pktlog_ani(_sc, _log_data, _flags);  \
        }                                                           \
    } while (0)

#endif                          /* _PKTLOG_HAL_H_ */
