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

#ifndef _ATH_LINUX_OSDEP_H
#define _ATH_LINUX_OSDEP_H

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
#include <linux/autoconf.h>
#else
#include <linux/config.h>
#endif
#include <linux/module.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <linux/delay.h>
#include <linux/wait.h>

#include <asm/byteorder.h>
#include <asm/scatterlist.h>

#ifdef AR9100
#include <ar9100.h>
#endif /* AR9100 */

#ifdef AH_WAR_52640
#include <ar7240.h>
#endif

#define INLINE  __inline
#define UNREFERENCED_PARAMETER(an);

#ifdef AR9100
/*
 * Howl needs DDR FIFO flush before any desc/dma data can be read.
 */
#define ATH_FLUSH_FIFO	ar9100_flush_wmac
#elif defined(AH_WAR_52640)
#       define ATH_FLUSH_FIFO  ar7240_flush_pcie
#else
#       define ATH_FLUSH_FIFO()
#endif

#ifndef ASSERT
#define ASSERT(exp) do {	\
	if (unlikely(!(exp))) {	\
		BUG();				\
	}						\
} while (0)
#endif

/*
 * Map Linux spin locks to OS independent names
 */
#define spin_lock_dpc(a)    \
    if (irqs_disabled()) {  \
        spin_lock(a);       \
    } else {                \
        spin_lock_bh(a);    \
    }
#define spin_unlock_dpc(a)  \
    if (irqs_disabled()) {  \
        spin_unlock(a);     \
    } else {                \
        spin_unlock_bh(a);  \
    }

/*
** Need to define byte order based on the CPU configuration.
*/

#ifdef CONFIG_CPU_BIG_ENDIAN
	#define _BYTE_ORDER	_BIG_ENDIAN
#else
	#define _BYTE_ORDER	_LITTLE_ENDIAN
#endif 

/*
 * Deduce if tasklets are available.  If not then
 * fall back to using the immediate work queue.
 */
#include <linux/interrupt.h>
#ifdef DECLARE_TASKLET          /* native tasklets */
#define tq_struct tasklet_struct
#define ATH_INIT_TQUEUE(a,b,c)      tasklet_init((a),(b),(unsigned long)(c))
#define ATH_SCHEDULE_TQUEUE(a,b)    tasklet_schedule((a))
typedef unsigned long TQUEUE_ARG;
#define mark_bh(a)
#else                   /* immediate work queue */
#define ATH_INIT_TQUEUE(a,b,c)      INIT_TQUEUE(a,b,c)
#define ATH_SCHEDULE_TQUEUE(a,b) do {       \
    *(b) |= queue_task((a), &tq_immediate); \
} while(0)
typedef void *TQUEUE_ARG;
#define tasklet_disable(t)  do { (void) t; local_bh_disable(); } while (0)
#define tasklet_enable(t)   do { (void) t; local_bh_enable(); } while (0)
#endif /* !DECLARE_TASKLET */

#include <linux/sched.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,41)
#include <linux/tqueue.h>
#define ATH_WORK_THREAD			tq_struct
#define ATH_SCHEDULE_TASK(t)		schedule_task((t))
#define ATH_INIT_SCHED_TASK(t, f, d)	do { memset((t),0,sizeof(struct tq_struct)); \
						(t)->routine = (void (*)(void*)) (f); \
						(t)->data=(void *) (d); } while (0)
#define ATH_FLUSH_TASKS			flush_scheduled_tasks
#else
#include <linux/workqueue.h>
#define ATH_SCHEDULE_TASK(t)		schedule_work((t))
//#define ATH_INIT_SCHED_TASK(t, f, d)	(DECLARE_WORK((t), (f), (d)))
#define ATH_INIT_SCHED_TASK(t, f, d)	do { memset(((void *) (t)),0,sizeof(struct work_struct)); \
		PREPARE_WORK((t),((void (*)(void*))(f)),((void *) (d))); } while (0)
#define ATH_WORK_THREAD			work_struct
#define	ATH_FLUSH_TASKS			flush_scheduled_work
#endif /* KERNEL_VERSION < 2.5.41 */

/*
 * Guess how the interrupt handler should work.
 */
#if !defined(IRQ_NONE)
typedef void irqreturn_t;
#define IRQ_NONE
#define IRQ_HANDLED
#endif /* !defined(IRQ_NONE) */

#ifndef SET_MODULE_OWNER
#define SET_MODULE_OWNER(dev) do {      \
    dev->owner = THIS_MODULE;       \
} while (0)
#endif

#ifndef SET_NETDEV_DEV
#define SET_NETDEV_DEV(ndev, pdev)
#endif

/*
 * Deal with the sysctl handler api changing.
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8)
#define ATH_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
    f(ctl_table *ctl, int write, struct file *filp, void *buffer, \
        size_t *lenp)
#define ATH_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
    proc_dointvec(ctl, write, filp, buffer, lenp)
#define ATH_SYSCTL_PROC_DOSTRING(ctl, write, filp, buffer, lenp, ppos) \
    proc_dostring(ctl, write, filp, buffer, lenp)
#else /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,8) */
#define ATH_SYSCTL_DECL(f, ctl, write, filp, buffer, lenp, ppos) \
    f(ctl_table *ctl, int write, struct file *filp, void *buffer,\
        size_t *lenp, loff_t *ppos)
#define ATH_SYSCTL_PROC_DOINTVEC(ctl, write, filp, buffer, lenp, ppos) \
    proc_dointvec(ctl, write, filp, buffer, lenp, ppos)
#define ATH_SYSCTL_PROC_DOSTRING(ctl, write, filp, buffer, lenp, ppos) \
    proc_dostring(ctl, write, filp, buffer, lenp, ppos)
#endif

/*
 * Byte Order stuff
 */
#define	le16toh(_x)	le16_to_cpu(_x)
#define	htole16(_x)	cpu_to_le16(_x)
#define htobe16(_x) cpu_to_be16(_x)
#define	le32toh(_x)	le32_to_cpu(_x)
#define	htole32(_x)	cpu_to_le32(_x)
#define	be32toh(_x)	be32_to_cpu(_x)
#define	htobe32(_x)	cpu_to_be32(_x)

#ifdef CONFIG_ARM
	/*
	** This is in support of XScale build.  They have a limit on the udelay
	** value, so we have to make sure we don't approach the limit
	*/

static INLINE void OS_DELAY(unsigned long delay)
{
	unsigned long	mticks;
	unsigned long	leftover;
	int				i;

	/*
	** slice into 1024 usec chunks (simplifies calculation)
	*/

	mticks = delay >> 10;
	leftover = delay - (mticks << 10);

	for(i=0;i<mticks;i++)
	{
		udelay(1024);
	}

	udelay(leftover);
}

#else
	/*
 	* Normal Delay functions. Time specified in microseconds.
 	*/
	#define OS_DELAY(_us)			udelay(_us)

#endif

#define OS_SLEEP(_us) do {					\
	set_current_state(TASK_INTERRUPTIBLE);	\
	schedule_timeout((_us) * HZ / 1000000);	\
} while (0)

#define OS_MEMCPY(_dst, _src, _len)		memcpy(_dst, _src, _len)
#define OS_MEMZERO(_buf, _len)			memset(_buf, 0, _len)
#define OS_MEMSET(_buf, _ch, _len)		memset(_buf, _ch, _len)
#define OS_MEMCMP(_mem1, _mem2, _len)	memcmp(_mem1, _mem2, _len)

/*
 * System time interface
 */
typedef unsigned long	systime_t;
typedef unsigned long	systick_t;

static INLINE systime_t
OS_GET_TIMESTAMP(void)
{
    return ((jiffies / HZ) * 1000) + (jiffies % HZ) * (1000 / HZ);
}

static INLINE systick_t
OS_GET_TICKS(void)
{
	return jiffies;
}

#define CONVERT_SYSTEM_TIME_TO_MS(_t)		jiffies_to_msecs(_t)
#define CONVERT_SYSTEM_TIME_TO_SEC(_t)		(jiffies_to_msecs(_t) / 1000)
#define CONVERT_SEC_TO_SYSTEM_TIME(_t)		((_t) * HZ)
#define CONVERT_MS_TO_SYSTEM_TIME(_t)		((_t) * HZ / 1000)

/*
 * Definition of OS-dependent device structure.
 * It'll be opaque to the actual ATH layer.
 */
struct _NIC_DEV {
	void                *bdev;      /* bus device handle */
    struct net_device   *netdev;    /* net device handle (wifi%d) */
    struct tq_struct    intr_tq;    /* tasklet */
    struct net_device_stats devstats;  /* net device statisitics */
};

typedef struct _NIC_DEV * osdev_t;

#define OS_MALLOC(_osdev, _size, _gfp)	kmalloc(_size, _gfp)
#define OS_FREE(_p)						kfree(_p)

#define OS_MALLOC_WITH_TAG(_ppMem, _size, _tag)	do {	\
	*(_ppMem) = kmalloc(_size, GFP_ATOMIC);				\
} while (0)

#define OS_FREE_WITH_TAG(_pMem, _size)	kfree(_pMem)

#if defined ATH_PCI
#include <if_ath_pci.h>
#elif defined ATH_AHB
#include <if_ath_ahb.h>
#else
#error "No bus type is specified"
#endif

typedef dma_addr_t * dma_context_t;

#define OS_DMA_MEM_CONTEXT(context)         \
    dma_addr_t   context;

#define OS_GET_DMA_MEM_CONTEXT(var, field)  \
    &(var->field)

#define OS_COPY_DMA_MEM_CONTEXT(dst, src)   \
    *dst = *src

void bus_read_cachesize(osdev_t, int *csz);

static INLINE void *
OS_MALLOC_CONSISTENT(osdev_t osdev, u_int32_t size, dma_addr_t *pa, dma_context_t context,
                     u_int32_t shmemalloc_retry)
{
	return bus_alloc_consistent(osdev->bdev, size, pa);
}

static INLINE void
OS_FREE_CONSISTENT(osdev_t osdev, u_int32_t size,
                   void *vaddr, dma_addr_t pa, dma_context_t context)
{
	bus_free_consistent(osdev->bdev, size, vaddr, pa);
}

static INLINE void
OS_SYNC_SINGLE(osdev_t osdev, dma_addr_t pa, u_int32_t size, int dir, dma_context_t context)
{
	bus_dma_sync_single(osdev->bdev, pa, size, dir);
}

static INLINE void
OS_UNMAP_SINGLE(osdev_t osdev, dma_addr_t pa, u_int32_t size, int dir, dma_context_t context)
{
	bus_unmap_single(osdev->bdev, pa, size, dir);
}

#define OS_EXEC_INTSAFE(_osdev, _fn, _arg) do {	\
	unsigned long flags;						\
	local_irq_save(flags);						\
	_fn(_arg);									\
	local_irq_restore(flags);					\
} while (0)

/*
 * Timer Interfaces. Use these macros to declare timer
 * and retrieve timer argument. This is mainly for resolving
 * different argument types for timer function in different OS.
 */
typedef struct timer_list		os_timer_t;
typedef void (*timer_func)(unsigned long);

#define OS_DECLARE_TIMER(_fn)                   \
    void _fn(unsigned long)

#define OS_TIMER_FUNC(_fn)                      \
    void _fn(unsigned long timer_arg)

#define OS_GET_TIMER_ARG(_arg, _type)           \
    (_arg) = (_type)(timer_arg)

#define OS_INIT_TIMER(_osdev, _timer, _fn, _arg) do {	\
	init_timer(_timer);									\
	(_timer)->function = (_fn);							\
	(_timer)->data = (unsigned long)(_arg);				\
} while (0)

#define OS_SET_TIMER(_timer, _ms)	mod_timer(_timer, jiffies + ((_ms)*HZ)/1000)

#define OS_CANCEL_TIMER(_timer)		del_timer(_timer)


/*
** These are required for network manager support
*/

#ifndef SET_NETDEV_DEV
#define	SET_NETDEV_DEV(ndev, pdev)
#endif

#ifdef to_net_dev
#define ATH_GET_NETDEV_DEV(ndev)	((ndev)->dev.parent)
#else
#define ATH_GET_NETDEV_DEV(ndev)	((ndev)->class_dev.dev)
#endif



/*
 * Opaque S/G List Entry
 */
typedef struct scatterlist			sg_t;

#include "hwdef.h"

#ifndef ARRAY_LENGTH
#define ARRAY_LENGTH(a)         (sizeof(a) / sizeof((a)[0]))
#endif

#ifndef MIN
#define MIN(a, b)				((a) < (b) ? a : b)
#endif
#ifndef MAX
#define MAX(a, b)				((a) > (b) ? a : b)
#endif

/*
 * PCI configuration space access
 */
#ifdef ATH_PCI

static INLINE u_int32_t
OS_PCI_READ_CONFIG(osdev_t osdev, u_int32_t offset, void *p, u_int32_t bytes)
{
	struct pci_dev *pdev = (struct pci_dev *)osdev->bdev;
	
	switch (bytes) {
	case 1:
		pci_read_config_byte(pdev, offset, p);
		break;
	case 2:
		pci_read_config_word(pdev, offset, p);
		break;
	case 4:
		pci_read_config_dword(pdev, offset, p);
		break;
	}
	return bytes;
}

static INLINE void
OS_PCI_WRITE_CONFIG(osdev_t osdev, u_int32_t offset, void *p, u_int32_t bytes)
{
	struct pci_dev *pdev = (struct pci_dev *)osdev->bdev;
	
	switch (bytes) {
	case 1:
		pci_write_config_byte(pdev, offset, *(u_int8_t *)p);
		break;
	case 2:
		pci_write_config_word(pdev, offset, *(u_int16_t *)p);
		break;
	case 4:
		pci_write_config_dword(pdev, offset, *(u_int32_t *)p);
		break;
	}
}

#else

static INLINE u_int32_t
OS_PCI_READ_CONFIG(osdev_t osdev, u_int32_t offset, void *p, u_int32_t bytes)
{
	OS_MEMSET(p, 0xff, bytes);
	return 0;
}

#define OS_PCI_WRITE_CONFIG(_osdev, _offset, _p, _bytes)

#endif

void *OS_ALLOC_VAP(osdev_t dev, u_int32_t len);
void OS_FREE_VAP(void *netif);

// ALLOC_DMA_MAP_CONTEXT_AREA is a NULL macro and is implemented only for BSD.
#define ALLOC_DMA_MAP_CONTEXT_AREA(os_handle, p_memctx)
#define ALLOC_DMA_CONTEXT_POOL(os_handle, name, numdesc)

#define ATH_FRAG_PER_MSDU   1

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)) && !defined(UINTPTR_T_IN_TYPES)
#define UINTPTR_T_IN_TYPES
#endif

#endif /* end of _ATH_LINUX_OSDEP_H */
