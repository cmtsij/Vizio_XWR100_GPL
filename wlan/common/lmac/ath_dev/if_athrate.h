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

/*-
 * Copyright (c) 2004 Sam Leffler, Errno Consulting
 * Copyright (c) 2004 Video54 Technologies, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer,
    without modification.
 * 2. Redistributions in binary form must reproduce at minimum a disclaimer
 *    similar to the "NO WARRANTY" disclaimer below ("Disclaimer") and any
 *    redistribution must be conditioned upon including a substantially
 *    similar Disclaimer requirement for further binary redistribution.
 * 3. Neither the names of the above-listed copyright holders nor the names
 *    of any contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 *
 * NO WARRANTY
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF NONINFRINGEMENT, MERCHANTIBILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.
 *
 * $Id: //depot/sw/releases/7.3_AP/wlan/common/lmac/ath_dev/if_athrate.h#1 $
 */
#ifndef _ATH_RATECTRL_H_
#define _ATH_RATECTRL_H_

/*
 * Interface definitions for transmit rate control modules for the
 * Atheros driver.
 *
 * A rate control module is responsible for choosing the transmit rate
 * for each data frame.  Management+control frames are always sent at
 * a fixed rate.
 *
 * Only one module may be present at a time; the driver references
 * rate control interfaces by symbol name.  If multiple modules are
 * to be supported we'll need to switch to a registration-based scheme
 * as is currently done, for example, for authentication modules.
 *
 * An instance of the rate control module is attached to each device
 * at attach time and detached when the device is destroyed.  The module
 * may associate data with each device and each node (station).  Both
 * sets of storage are opaque except for the size of the per-node storage
 * which must be provided when the module is attached.
 *
 * The rate control module is notified for each state transition and
 * station association/reassociation.  Otherwise it is queried for a
 * rate for each outgoing frame and provided status from each transmitted
 * frame.  Any ancillary processing is the responsibility of the module
 * (e.g. if periodic processing is required then the module should setup
 * it's own timer).
 *
 * In addition to the transmit rate for each frame the module must also
 * indicate the number of attempts to make at the specified rate.  If this
 * number is != ATH_TXMAXTRY then an additional callback is made to setup
 * additional transmit state.  The rate control code is assumed to write
 * this additional data directly to the transmit descriptor.
 */
struct ath_softc;
struct ath_node;
struct ath_desc;
struct ath_vap;
struct ieee80211_rateset;

#define ATH_RC_PROBE_ALLOWED            0x00000001
#define ATH_RC_MINRATE_LASTRATE         0x00000002
#define ATH_RC_SHORT_PREAMBLE           0x00000004

struct ath_rc_series {
    u_int8_t    rix;
    u_int8_t    tries;
    u_int8_t    flags;
    u_int32_t   max4msframelen;
};

/* rcs_flags definition */
#define ATH_RC_DS_FLAG               0x01
#define ATH_RC_CW40_FLAG             0x02    /* CW 40 */
#define ATH_RC_SGI_FLAG              0x04    /* Short Guard Interval */
#define ATH_RC_HT_FLAG               0x08    /* HT */
#define ATH_RC_RTSCTS_FLAG           0x10    /* RTS-CTS */
#define ATH_RC_TX_STBC_FLAG          0x20    /* TX STBC */
#define ATH_RC_RX_STBC_FLAG          0xC0    /* RX STBC ,2 bits */
#define ATH_RC_RX_STBC_FLAG_S           6   
#define ATH_RC_WEP_TKIP_FLAG         0x100    /* WEP/TKIP encryption */

/* 
 * packet duration with maxium allowable retries, in us.
 * The time based retry feature is disabled if this value is 0
 */
#define ATH_RC_DURATION_WITH_RETRIES 0

struct ath_hal;

/*
 * Attach/detach a rate control module.
 */
struct atheros_softc *
ath_rate_attach(struct ath_hal *ah);

void
ath_rate_detach(struct atheros_softc *asc);

struct atheros_vap *
ath_rate_create_vap(struct atheros_softc *asc, struct ath_vap *athdev_vap);

void
ath_rate_free_vap(struct atheros_vap *avp);

struct atheros_node *
ath_rate_node_alloc(struct atheros_vap *avp);

void
ath_rate_node_free(struct atheros_node *anode);

/*
 * State storage handling.
 */
/*
 * Initialize per-node state already allocated for the specified
 * node; this space can be assumed initialized to zero.
 */
void ath_rate_node_init(struct atheros_node *);
/*
 * Cleanup any per-node state prior to the node being reclaimed.
 */
void ath_rate_node_cleanup(struct atheros_node *);
/*
 * Update rate control state on station associate/reassociate
 * (when operating as an ap or for nodes discovered when operating
 * in ibss mode).
 */
int
ath_rate_newassoc(struct ath_softc *sc, struct ath_node *an,
                  int isnew, unsigned int capflag,
                  struct ieee80211_rateset *negotiated_rates,
                  struct ieee80211_rateset *negotiated_htrates);
/*
 * Update/reset rate control state for 802.11 state transitions.
 * Important mostly as the analog to ath_rate_newassoc when operating
 * in station mode.
 */
void
ath_rate_newstate(struct ath_softc *sc, struct ath_vap *avp, int up);

/*
 * Update rate control state to reflect the changes in SM power save modes.
 */
void
ath_rate_node_update(struct ath_node *);

/*
 * Transmit handling.
 */
/*
 * Return the transmit info for a data packet.  If multi-rate state
 * is to be setup then try0 should contain a value other than ATH_TXMATRY
 * and ath_rate_setupxtxdesc will be called after deciding if the frame
 * can be transmitted with multi-rate retry.
 */
void
ath_rate_findrate(struct ath_softc *sc, struct ath_node *an,
                  int shortPreamble, u_int32_t frameLen, int numTries,
                  unsigned int rcflag, u_int8_t ac, struct ath_rc_series[],
                  int *isProbe, int isretry);
/*
 * Return rate index for given Dot11 Rate.
 */
u_int8_t
ath_rate_findrateix(struct ath_softc *sc, struct ath_node *an, u_int8_t dot11Rate);

/*
 * Update rate control state for a packet associated with the
 * supplied transmit descriptor.  The routine is invoked both
 * for packets that were successfully sent and for those that
 * failed (consult the descriptor for details).
 */
void
ath_rate_tx_complete(struct ath_softc *sc, struct ath_node *an, const struct ath_desc *ds,
                     struct ath_rc_series series[], u_int8_t ac, int nframes, int nbad);

int
ath_rate_table_init(void);

typedef void (*SetDefAntenna_callback)(void *, u_int);

#endif /* _ATH_RATECTRL_H_ */
