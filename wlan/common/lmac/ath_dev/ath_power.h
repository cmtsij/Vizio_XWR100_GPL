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
 * Public Interface for power control module
 */


#ifndef _DEV_ATH_POWER_H
#define _DEV_ATH_POWER_H

#include "ath_timer.h"

/*
 * Definitions for the Atheros power control module.
 */
#define ATH_MAX_SLEEP_TIME           10          /* # of beacon intervals to sleep in MAX PWRSAVE*/
#define ATH_NORMAL_SLEEP_TIME        1           /* # of beacon intervals to sleep in NORMAL PWRSAVE*/
#define	ATH_PS_LOCK_INIT(_sc)        spin_lock_init(&(_sc)->sc_pwrsave.ps_pwrsave_lock)
#define	ATH_PS_LOCK_DESTROY(_sc)
#define	ATH_PS_LOCK(_sc)             spin_lock(&(_sc)->sc_pwrsave.ps_pwrsave_lock)
#define	ATH_PS_UNLOCK(_sc)           spin_unlock(&(_sc)->sc_pwrsave.ps_pwrsave_lock)


/*
 * macros to wake up and restore the HW powersave state
 * for temporary hal access.
 * when calling any HAL function that accesses registers,
 * the chip should be awake otherwise the PCI bus will hang.
 * use ATH_HAL_PS_WAKEUP to wakeup the chip before any hal
 * function calls that access registers.
 * use ATH_HAL_PS_SLEEP after all hal function calls to push
 * it to SLEEP state.
 */
#define ATH_PS_WAKEUP(sc)                                                       \
    ATH_PS_LOCK(sc);                                                            \
    if(!(sc)->sc_pwrsave.ps_hal_usecount) {                                     \
        (sc)->sc_pwrsave.ps_restore_state = (sc)->sc_pwrsave.ps_pwrsave_state;  \
        ath_pwrsave_set_state((sc), ATH_PWRSAVE_AWAKE);                         \
    }                                                                           \
    (sc)->sc_pwrsave.ps_hal_usecount++;                                         \
    ATH_PS_UNLOCK(sc);

#define ATH_PS_SLEEP(sc)                                                        \
    ATH_PS_LOCK(sc);                                                            \
    (sc)->sc_pwrsave.ps_hal_usecount--;                                         \
    if(!(sc)->sc_pwrsave.ps_hal_usecount) {                                     \
        ath_pwrsave_set_state((sc), (sc)->sc_pwrsave.ps_restore_state);         \
    }                                                                           \
    ATH_PS_UNLOCK(sc);

#define ATH_FUNC_ENTRY_CHECK(sc, error)                                         \
    if(!((sc)->sc_sw_phystate && (sc)->sc_hw_phystate)) {                       \
       return (error);                                                          \
    }

#define ATH_FUNC_ENTRY_VOID(sc)	                                                \
    if(!((sc)->sc_sw_phystate && (sc)->sc_hw_phystate)) {                       \
       return;                                                                  \
    }


typedef enum {
        ATH_PWRSAVE_AWAKE=0,
        ATH_PWRSAVE_FULL_SLEEP,
        ATH_PWRSAVE_NETWORK_SLEEP,
} ATH_PWRSAVE_STATE;                /* slot time update fsm */

typedef struct _ath_pwrsave {
    u_int32_t               ps_hal_usecount;      /* number of hal users */
    ATH_PWRSAVE_STATE       ps_restore_state;     /* power save state to restore*/
    ATH_PWRSAVE_STATE	    ps_pwrsave_state;     /* power save state */
    ATH_PWRSAVE_STATE	    ps_set_state;         /* power save state to set*/
    spinlock_t              ps_pwrsave_lock;      /* softc-level lock */
} ath_pwrsave_t;

/*
 * Support interfaces
 */

void ath_pwrsave_set_state(struct ath_softc *sc, ATH_PWRSAVE_STATE newstate);
void ath_pwrsave_proc(struct ath_softc *sc);
int  ath_pwrsave_timer(struct ath_softc *sc);
void ath_pwrsave_init(struct ath_softc *sc);
void ath_pwrsave_awake(ath_dev_t);
void ath_pwrsave_netsleep(ath_dev_t);
void ath_pwrsave_fullsleep(ath_dev_t);
int  ath_pwrsave_get_state(struct ath_softc *sc);
void ath_pwrsave_proc_intdone(struct ath_softc *sc, u_int32_t intrStatus);

/*
 * PCI Express core vendor specific registers
 * ASPM (Active State Power Management)
 */
#define PCIE_CAP_ID                 0x10    /* PCIe Capability ID */
#define PCIE_CAP_LNKCTL_OFFSET      0x10    /* Link control offset in PCIE capability */ 
#define PCIE_CAP_OFFSET             0x60    /* PCIe cabability offset on Atheros Cards */
#define PCIE_CAP_LINK_CTRL          0x70    /* PCIe Link Capability */
#define PCIE_CAP_LINK_L0S           1       /* ASPM L0s */
#define PCIE_CAP_LINK_L1            2       /* ASPM L1 */


#endif /* _DEV_ATH_POWER_H */
