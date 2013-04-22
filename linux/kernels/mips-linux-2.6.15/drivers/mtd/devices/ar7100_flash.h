/*
 * Copyright (c) 2008, Atheros Communications Inc.
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
#ifndef _AR7100_SPI_H
#define _AR7100_SPI_H

#define AR7100_SPI_FS           0x1f000000
#define AR7100_SPI_CLOCK        0x1f000004
#define AR7100_SPI_WRITE        0x1f000008
#define AR7100_SPI_READ         0x1f000000
#define AR7100_SPI_RD_STATUS    0x1f00000c

#define AR7100_SPI_CS_DIS       0x70000
#define AR7100_SPI_CE_LOW       0x60000
#define AR7100_SPI_CE_HIGH      0x60100

#define AR7100_SPI_CMD_WREN         0x06
#define AR7100_SPI_CMD_RD_STATUS    0x05
#define AR7100_SPI_CMD_FAST_READ    0x0b
#define AR7100_SPI_CMD_PAGE_PROG    0x02
#define AR7100_SPI_CMD_SECTOR_ERASE 0xd8

#define AR7100_SPI_SECTOR_SIZE      (1024*64)
#define AR7100_SPI_PAGE_SIZE        256

#define AR7100_FLASH_MAX_BANKS  1


#define display(_x)     ar7100_reg_wr_nf(0x18040008, (_x))

/*
 * primitives
 */

#define ar7100_be_msb(_val, __i) (((_val) & (1 << (7 - __i))) >> (7 - __i))

#define ar7100_spi_bit_banger(_byte)  do {        \
    int _i;                                      \
    for(_i = 0; _i < 8; _i++) {                    \
        ar7100_reg_wr_nf(AR7100_SPI_WRITE,      \
                        AR7100_SPI_CE_LOW | ar7100_be_msb(_byte, _i));  \
        ar7100_reg_wr_nf(AR7100_SPI_WRITE,      \
                        AR7100_SPI_CE_HIGH | ar7100_be_msb(_byte, _i)); \
    }       \
}while(0);

#define ar7100_spi_go() do {        \
    ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CE_LOW); \
    ar7100_reg_wr_nf(AR7100_SPI_WRITE, AR7100_SPI_CS_DIS); \
}while(0);


#define ar7100_spi_send_addr(_addr) do {                    \
    ar7100_spi_bit_banger(((addr & 0xff0000) >> 16));                 \
    ar7100_spi_bit_banger(((addr & 0x00ff00) >> 8));                 \
    ar7100_spi_bit_banger(addr & 0x0000ff);                 \
}while(0);

#define ar7100_spi_delay_8()    ar7100_spi_bit_banger(0)
#define ar7100_spi_done()       ar7100_reg_wr(AR7100_SPI_FS, 0)

#endif /*_AR7100_SPI_H*/
