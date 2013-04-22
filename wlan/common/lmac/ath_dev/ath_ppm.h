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
 * Public Interface for LED control module
 */

/*
 * Definitions for the Atheros LED control module.
 */
#ifndef _DEV_ATH_PPM_H
#define _DEV_ATH_PPM_H

#include "ath_timer.h"


#define ATH_FORCE_PPM_PERIOD                     1000  /* ms, support timer period. Def 1 second */
#define ATH_FORCE_PPM_UPDATE_TIMEOUT             2000  /* ms, between sample window updates */
#define ATH_FORCE_PPM_INACTIVITY_TIMEOUT        20000  /* ms, wd timeout for rx frames */

#define ATH_FORCE_PPM_RECOVERY_TIMEOUT         0x3000  /* us, recovery timeout.  Def 0x3000=12288 (12 ms) */
 
enum ath_force_ppm_event_t {
    ATH_FORCE_PPM_DISABLE,
    ATH_FORCE_PPM_ENABLE,
    ATH_FORCE_PPM_SUSPEND,
    ATH_FORCE_PPM_RESUME,

    ATH_FORCE_PPM_NUMBER_EVENTS
};

/*
 * Force PPM tracking workaround
 */
typedef struct ath_force_ppm {
    osdev_t             osdev;                        // pointer to OS device
    struct ath_softc    *sc;                          // pointer to sc
    struct ath_hal      *ah;                          // pointer to HAL object

    spinlock_t          ppmLock;                      // Synchronization object

    u_int8_t            ppmActive;

    struct ath_timer    timer;
    int                 timer_running;

    int                 timerStart1;
    u_int32_t           timerCount1;
    u_int32_t           timerThrsh1;
    int32_t             timerStart2;
    u_int32_t           timerCount2;
    u_int32_t           timerThrsh2;
    u_int32_t           lastTsf1;
    u_int32_t           lastTsf2;
    u_int32_t           latchedRssi;
    int                 forceState;
    HAL_BOOL            isConnected;
    HAL_BOOL            isSuspended;
    u_int8_t            bssid[IEEE80211_ADDR_LEN];  
    u_int32_t           force_ppm_update_timeout;
    u_int32_t           force_ppm_inactivity_timeout;
    u_int32_t           force_ppm_recovery_timeout;
} ath_force_ppm_t;

void ath_initialize_force_ppm_module (ath_force_ppm_t      *afp, 
                                      struct ath_softc     *sc, 
                                      osdev_t              sc_osdev,
                                      struct ath_hal       *ah,
                                      struct ath_reg_parm  *pRegParam);
void ath_start_force_ppm_module      (ath_force_ppm_t      *afp);
void ath_halt_force_ppm_module       (ath_force_ppm_t      *afp);
int  ath_force_ppm_logic             (ath_force_ppm_t      *afp, 
                                      struct ath_buf       *bf, 
                                      HAL_STATUS           status, 
                                      struct ath_rx_status *rx_stats);
void ath_force_ppm_notify            (ath_force_ppm_t      *afp,
                                      int                  event,
                                      u_int8_t             *bssid);

#endif /* _DEV_ATH_PPM_H */
