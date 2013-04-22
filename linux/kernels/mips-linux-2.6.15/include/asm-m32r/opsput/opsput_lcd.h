/*
 * include/asm/opsput_lcd.h
 *
 * OPSPUT-LCD board
 *
 * Copyright (c) 2002	Takeo Takahashi
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file "COPYING" in the main directory of
 * this archive for more details.
 *
 * $Id: //depot/sw/releases/7.3_AP/linux/kernels/mips-linux-2.6.15/include/asm-m32r/opsput/opsput_lcd.h#1 $
 */

#ifndef _OPSPUT_OPSPUT_LCD_H
#define _OPSPUT_OPSPUT_LCD_H

#include <linux/config.h>

#ifndef __ASSEMBLY__
/*
 * C functions use non-cache address.
 */
#define OPSPUT_LCD_BASE	(0x10000000 /* + NONCACHE_OFFSET */)
#else
#define OPSPUT_LCD_BASE	(0x10000000 + NONCACHE_OFFSET)
#endif	/* __ASSEMBLY__ */

/*
 * ICU
 */
#define OPSPUT_LCD_IRQ_BAT_INT	(OPSPUT_LCD_PLD_IRQ_BASE + 1)
#define OPSPUT_LCD_IRQ_USB_INT1	(OPSPUT_LCD_PLD_IRQ_BASE + 2)
#define OPSPUT_LCD_IRQ_AUDT0		(OPSPUT_LCD_PLD_IRQ_BASE + 3)
#define OPSPUT_LCD_IRQ_AUDT2		(OPSPUT_LCD_PLD_IRQ_BASE + 4)
#define OPSPUT_LCD_IRQ_BATSIO_RCV	(OPSPUT_LCD_PLD_IRQ_BASE + 16)
#define OPSPUT_LCD_IRQ_BATSIO_SND	(OPSPUT_LCD_PLD_IRQ_BASE + 17)
#define OPSPUT_LCD_IRQ_ASNDSIO_RCV	(OPSPUT_LCD_PLD_IRQ_BASE + 18)
#define OPSPUT_LCD_IRQ_ASNDSIO_SND	(OPSPUT_LCD_PLD_IRQ_BASE + 19)
#define OPSPUT_LCD_IRQ_ACNLSIO_SND	(OPSPUT_LCD_PLD_IRQ_BASE + 21)

#define OPSPUT_LCD_ICUISTS	__reg16(OPSPUT_LCD_BASE + 0x300002)
#define OPSPUT_LCD_ICUISTS_VECB_MASK	(0xf000)
#define OPSPUT_LCD_VECB(x)	((x) & OPSPUT_LCD_ICUISTS_VECB_MASK)
#define OPSPUT_LCD_ICUISTS_ISN_MASK	(0x07c0)
#define OPSPUT_LCD_ICUISTS_ISN(x)	((x) & OPSPUT_LCD_ICUISTS_ISN_MASK)
#define OPSPUT_LCD_ICUIREQ0	__reg16(OPSPUT_LCD_BASE + 0x300004)
#define OPSPUT_LCD_ICUIREQ1	__reg16(OPSPUT_LCD_BASE + 0x300006)
#define OPSPUT_LCD_ICUCR1	__reg16(OPSPUT_LCD_BASE + 0x300020)
#define OPSPUT_LCD_ICUCR2	__reg16(OPSPUT_LCD_BASE + 0x300022)
#define OPSPUT_LCD_ICUCR3	__reg16(OPSPUT_LCD_BASE + 0x300024)
#define OPSPUT_LCD_ICUCR4	__reg16(OPSPUT_LCD_BASE + 0x300026)
#define OPSPUT_LCD_ICUCR16	__reg16(OPSPUT_LCD_BASE + 0x300030)
#define OPSPUT_LCD_ICUCR17	__reg16(OPSPUT_LCD_BASE + 0x300032)
#define OPSPUT_LCD_ICUCR18	__reg16(OPSPUT_LCD_BASE + 0x300034)
#define OPSPUT_LCD_ICUCR19	__reg16(OPSPUT_LCD_BASE + 0x300036)
#define OPSPUT_LCD_ICUCR21	__reg16(OPSPUT_LCD_BASE + 0x30003a)

#endif	/* _OPSPUT_OPSPUT_LCD_H */
