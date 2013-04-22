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
#ifndef _ATH_COMPAT_H_
#define _ATH_COMPAT_H_
/*
 * BSD/Linux compatibility shims.  These are used mainly to
 * minimize differences when importing necesary BSD code.
 */
#define NBBY    8           /* number of bits/byte */

#ifdef __linux__
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))  /* to any y */
#endif
#elif WIN32
#define roundup(x, y)   ((((x)+((y)-1))/(y))*(y))  /* to any y */
#endif

#define howmany(x, y)   (((x)+((y)-1))/(y))

/* Bit map related macros. */
#define setbit(a,i) ((a)[(i)/NBBY] |= 1<<((i)%NBBY))
#define clrbit(a,i) ((a)[(i)/NBBY] &= ~(1<<((i)%NBBY)))
#define isset(a,i)  ((a)[(i)/NBBY] & (1<<((i)%NBBY)))
#define isclr(a,i)  (((a)[(i)/NBBY] & (1<<((i)%NBBY))) == 0)

#ifndef WIN32
#define __printflike(_a,_b) \
    __attribute__ ((__format__ (__printf__, _a, _b)))
#endif

#define __offsetof(t,m) offsetof(t,m)

#ifndef ALIGNED_POINTER
/*
 * ALIGNED_POINTER is a boolean macro that checks whether an address
 * is valid to fetch data elements of type t from on this architecture.
 * This does not reflect the optimal alignment, just the possibility
 * (within reasonable limits). 
 *
 */
#define ALIGNED_POINTER(p,t)    1
#endif

/*
**  For non Linux (gcc compiled) drivers, define the likely() and 
**  unlikely() macros to be simply the argument.  This sould fix build
**  issues for NetBSD and Vista
*/

#ifndef __linux__

#define unlikely(_a)    _a
#define likely(_b)      _b

#endif

/*
** Assert for Linux kernel mode.  This assumes unlikely is defined,
** so it assumes a Linux OS
*/

#ifdef __KERNEL__
#include <asm/page.h>

#define KASSERT(exp, msg) do {          \
    if (unlikely(!(exp))) {         \
        printk msg;         \
        BUG();              \
    }                   \
} while (0)

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,24)
#define CTL_AUTO -2
#define DEV_ATH 9
#else
#define CTL_AUTO CTL_UNNUMBERED
#define DEV_ATH CTL_UNNUMBERED
#endif  /* sysctl */

#endif /* __KERNEL__ */

#ifndef WIN32
#ifndef ATH_SUPPORT_STA
#ifndef __packed
#define __packed   __attribute__((__packed__))
#endif
#endif
#endif

/*
 * NetBSD/FreeBSD defines for file version.
 */
#define __FBSDID(_s)
#define __KERNEL_RCSID(_n,_s)
#endif /* _ATH_COMPAT_H_ */
