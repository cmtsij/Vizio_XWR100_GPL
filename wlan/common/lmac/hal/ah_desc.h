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

#ifndef _DEV_ATH_DESC_H
#define _DEV_ATH_DESC_H

#ifdef WIN32
#pragma pack(push, ah_desc, 1)
#endif

/*
 * Transmit descriptor status.  This structure is filled
 * in only after the tx descriptor process method finds a
 * ``done'' descriptor; at which point it returns something
 * other than HAL_EINPROGRESS.
 *
 * Note that ts_antenna may not be valid for all h/w.  It
 * should be used only if non-zero.
 *
 * NOTE: Transmit descriptor status now is superset of descriptor
 *       status information cor all h/w types.
 */
struct ath_tx_status {
    u_int32_t    ts_tstamp;     /* h/w assigned timestamp */
    u_int16_t    ts_seqnum;     /* h/w assigned sequence number */
    u_int8_t     ts_status;     /* frame status, 0 => xmit ok */
    u_int8_t     ts_ratecode;   /* h/w transmit rate code */
    u_int8_t     ts_rateindex;  /* h/w transmit rate index */
    int8_t       ts_rssi;       /* tx ack RSSI */
    u_int8_t     ts_shortretry; /* # short retries */
    u_int8_t     ts_longretry;  /* # long retries */
    u_int8_t     ts_virtcol;    /* virtual collision count */
    u_int8_t     ts_antenna;    /* antenna information */

        /* Additional status information */
    u_int8_t     ts_flags;       /* misc flags */
    int8_t       ts_rssi_ctl0;   /* tx ack RSSI [ctl, chain 0] */
    int8_t       ts_rssi_ctl1;   /* tx ack RSSI [ctl, chain 1] */
    int8_t       ts_rssi_ctl2;   /* tx ack RSSI [ctl, chain 1] */
    int8_t       ts_rssi_ext0;   /* tx ack RSSI [ext, chain 1] */
    int8_t       ts_rssi_ext1;   /* tx ack RSSI [ext, chain 1] */
    int8_t       ts_rssi_ext2;   /* tx ack RSSI [ext, chain 1] */
    u_int8_t     pad[3];         /* pad to DWORD alignment */

    u_int32_t    ba_low;         /* blockack bitmap low */
    u_int32_t    ba_high;        /* blockack bitmap high */
    u_int32_t    evm0;           /* evm bytes */
    u_int32_t    evm1;
    u_int32_t    evm2;
};

#define HAL_TXERR_XRETRY                0x01    /* excessive retries */
#define HAL_TXERR_FILT                  0x02    /* blocked by tx filtering */
#define HAL_TXERR_FIFO                  0x04    /* fifo underrun */
#define HAL_TXERR_XTXOP                 0x08    /* txop exceeded */
#define HAL_TXERR_TIMER_EXPIRED         0x10    /* tx timer expired */

/*
 * ts_flags values
 */
#define HAL_TX_BA               0x01     /* BA seen */
#define HAL_TX_PWRMGMT          0x02     /* pwr mgmt bit set */
#define HAL_TX_DESC_CFG_ERR     0x04     /* Error in 20/40 desc config */
#define HAL_TX_DATA_UNDERRUN    0x08     /* Tx buffer underrun */
#define HAL_TX_DELIM_UNDERRUN   0x10     /* Tx delimiter underrun */
#define HAL_TX_SW_ABORTED       0x40     /* aborted by software */
#define HAL_TX_SW_FILTERED      0x80     /* filtered by software */

#define HAL_IS_TX_UNDERRUN(_pdesc) \
        ((_pdesc)->ds_txstat.ts_flags & (HAL_TX_DATA_UNDERRUN | HAL_TX_DELIM_UNDERRUN))

/*
 * Receive descriptor status.  This structure is filled
 * in only after the rx descriptor process method finds a
 * ``done'' descriptor; at which point it returns something
 * other than HAL_EINPROGRESS.
 *
 * If rx_status is zero, then the frame was received ok;
 * otherwise the error information is indicated and rs_phyerr
 * contains a phy error code if HAL_RXERR_PHY is set.  In general
 * the frame contents is undefined when an error occurred thought
 * for some errors (e.g. a decryption error), it may be meaningful.
 *
 * Note that the receive timestamp is expanded using the TSF to
 * 15 bits (regardless of what the h/w provides directly).
 *
 * rx_rssi is in units of dbm above the noise floor.  This value
 * is measured during the preamble and PLCP; i.e. with the initial
 * 4us of detection.  The noise floor is typically a consistent
 * -96dBm absolute power in a 20MHz channel.
 */
struct ath_rx_status {
    u_int32_t   rs_tstamp;    /* h/w assigned timestamp */
    u_int16_t   rs_datalen;    /* rx frame length */
    u_int8_t    rs_status;    /* rx status, 0 => recv ok */
    u_int8_t    rs_phyerr;    /* phy error code */
    u_int8_t    rs_rssi;    /* rx frame RSSI */
    u_int8_t    rs_keyix;    /* key cache index */
    u_int8_t    rs_rate;    /* h/w receive rate index */
    u_int8_t    rs_antenna;    /* antenna information */
    u_int8_t    rs_more;    /* more descriptors follow */

        /* Additional receive status */
    u_int8_t    rs_rssi_ctl0;    /* rx frame RSSI [ctl, chain 0] */
    u_int8_t    rs_rssi_ctl1;    /* rx frame RSSI [ctl, chain 1] */
    u_int8_t    rs_rssi_ctl2;    /* rx frame RSSI [ctl, chain 2] */
    u_int8_t    rs_rssi_ext0;    /* rx frame RSSI [ext, chain 0] */
    u_int8_t    rs_rssi_ext1;    /* rx frame RSSI [ext, chain 1] */
    u_int8_t    rs_rssi_ext2;    /* rx frame RSSI [ext, chain 2] */
    u_int8_t    rs_isaggr;      /* is part of the aggregate */
    u_int8_t    rs_moreaggr;    /* more frames in aggr to follow */
    u_int8_t    rs_num_delims;  /* number of delims in aggr */
    u_int8_t    rs_flags;       /* misc flags */
    u_int32_t   evm0;
    u_int32_t   evm1;           /* evm bytes */
    u_int32_t   evm2;
};

#define    HAL_RXERR_CRC        0x01    /* CRC error on frame */
#define    HAL_RXERR_PHY        0x02    /* PHY error, rs_phyerr is valid */
#define    HAL_RXERR_FIFO       0x04    /* fifo overrun */
#define    HAL_RXERR_DECRYPT    0x08    /* non-Michael decrypt error */
#define    HAL_RXERR_MIC        0x10    /* Michael MIC decrypt error */
#define    HAL_RXERR_INCOMP     0x20    /* Rx Desc processing is incomplete */

/* rs_flags values */
#define HAL_RX_MORE             0x01    /* more descriptors follow */
#define HAL_RX_MORE_AGGR        0x02    /* more frames in aggr */
#define HAL_RX_GI               0x04    /* full gi */
#define HAL_RX_2040             0x08    /* 40 Mhz */
#define HAL_RX_DELIM_CRC_PRE    0x10    /* crc error in delimiter pre */
#define HAL_RX_DELIM_CRC_POST   0x20    /* crc error in delim after */
#define HAL_RX_DECRYPT_BUSY     0x40    /* decrypt was too slow */
#define HAL_RX_HI_RX_CHAIN      0x80    /* SM power save: Rx chain control in high */

enum {
    HAL_PHYERR_UNDERRUN             = 0,    /* Transmit underrun */
    HAL_PHYERR_TIMING               = 1,    /* Timing error */
    HAL_PHYERR_PARITY               = 2,    /* Illegal parity */
    HAL_PHYERR_RATE                 = 3,    /* Illegal rate */
    HAL_PHYERR_LENGTH               = 4,    /* Illegal length */
    HAL_PHYERR_RADAR                = 5,    /* Radar detect */
    HAL_PHYERR_SERVICE              = 6,    /* Illegal service */
    HAL_PHYERR_TOR                  = 7,    /* Transmit override receive */
    /* NB: these are specific to the 5212 */
    HAL_PHYERR_OFDM_TIMING          = 17,    /* */
    HAL_PHYERR_OFDM_SIGNAL_PARITY   = 18,    /* */
    HAL_PHYERR_OFDM_RATE_ILLEGAL    = 19,    /* */
    HAL_PHYERR_OFDM_LENGTH_ILLEGAL  = 20,    /* */
    HAL_PHYERR_OFDM_POWER_DROP      = 21,    /* */
    HAL_PHYERR_OFDM_SERVICE         = 22,    /* */
    HAL_PHYERR_OFDM_RESTART         = 23,    /* */
    HAL_PHYERR_FALSE_RADAR_EXT      = 24,    /* Radar detect */

    HAL_PHYERR_CCK_TIMING           = 25,    /* */
    HAL_PHYERR_CCK_HEADER_CRC       = 26,    /* */
    HAL_PHYERR_CCK_RATE_ILLEGAL     = 27,    /* */
    HAL_PHYERR_CCK_SERVICE          = 30,    /* */
    HAL_PHYERR_CCK_RESTART          = 31,    /* */
    HAL_PHYERR_CCK_LENGTH_ILLEGAL   = 32,   /* */
    HAL_PHYERR_CCK_POWER_DROP       = 33,   /* */

    HAL_PHYERR_HT_CRC_ERROR         = 34,   /* */
    HAL_PHYERR_HT_LENGTH_ILLEGAL    = 35,   /* */
    HAL_PHYERR_HT_RATE_ILLEGAL      = 36,   /* */
};

/* value found in rs_keyix to mark invalid entries */
#define    HAL_RXKEYIX_INVALID    ((u_int8_t) -1)
/* value used to specify no encryption key for xmit */
#define    HAL_TXKEYIX_INVALID    ((u_int) -1)

/* value used to specify invalid signal status */
#define    HAL_RSSI_BAD           0x80
#define    HAL_EVM_BAD            0x80

/* XXX rs_antenna definitions */

/*
 * Definitions for the software frame/packet descriptors used by
 * the Atheros HAL.  This definition obscures hardware-specific
 * details from the driver.  Drivers are expected to fillin the
 * portions of a descriptor that are not opaque then use HAL calls
 * to complete the work.  Status for completed frames is returned
 * in a device-independent format.
 */
struct ath_desc {
    /*
     * The following definitions are passed directly
     * the hardware and managed by the HAL.  Drivers
     * should not touch those elements marked opaque.
     */
    u_int32_t    ds_link;    /* phys address of next descriptor */
    u_int32_t    ds_data;    /* phys address of data buffer */
    u_int32_t    ds_ctl0;    /* opaque DMA control 0 */
    u_int32_t    ds_ctl1;    /* opaque DMA control 1 */
    u_int32_t    ds_hw[20];    /* opaque h/w region */
    /*
     * The remaining definitions are managed by software;
     * these are valid only after the rx/tx process descriptor
     * methods return a non-EINPROGRESS  code.
     */
    union {
        struct ath_tx_status tx;/* xmit status */
        struct ath_rx_status rx;/* recv status */
        void    *stats;
    } ds_us;
    void        *ds_vdata;    /* virtual addr of data buffer */
} __packed;

#define    ds_txstat    ds_us.tx
#define    ds_rxstat    ds_us.rx
#define ds_stat        ds_us.stats

/* flags passed to tx descriptor setup methods */
#define HAL_TXDESC_CLRDMASK     0x0001    /* clear destination filter mask */
#define HAL_TXDESC_NOACK        0x0002    /* don't wait for ACK */
#define HAL_TXDESC_RTSENA       0x0004    /* enable RTS */
#define HAL_TXDESC_CTSENA       0x0008    /* enable CTS */
#define HAL_TXDESC_INTREQ       0x0010    /* enable per-descriptor interrupt */
#define HAL_TXDESC_VEOL         0x0020    /* mark virtual EOL */
#define HAL_TXDESC_EXT_ONLY     0x0040  /* send on ext channel only */
#define HAL_TXDESC_EXT_AND_CTL  0x0080  /* send on ext + ctl channels */
#define HAL_TXDESC_VMF          0x0100  /* virtual more frag */
#define HAL_TXDESC_FRAG_IS_ON   0x0200  /* Fragmentation enabled */
#define HAL_TXDESC_LOWRXCHAIN   0x0400  /* switch to low rx chain */

/* flags passed to rx descriptor setup methods */
#define HAL_RXDESC_INTREQ       0x0020    /* enable per-descriptor interrupt */

#ifdef WIN32
#pragma pack(pop, ah_desc)
#endif

#endif /* _DEV_ATH_AR521XDMA_H */
