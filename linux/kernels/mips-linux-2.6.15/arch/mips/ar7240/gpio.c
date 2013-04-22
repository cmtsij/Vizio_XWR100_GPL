#ifndef EXPORT_SYMTAB
#define EXPORT_SYMTAB
#endif

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/signal.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/init.h>
#include <linux/resource.h>
#include <linux/proc_fs.h>
#include <linux/miscdevice.h>
#include <asm/types.h>
#include <asm/irq.h>
#include <asm/delay.h>
#include <asm/system.h>

#include "ar7240.h"

#define AR7240_FACTORY_RESET		0x89ABCDEF

static atomic_t ar7240_fr_status = ATOMIC_INIT(0);
static volatile int ar7240_fr_opened=0;
static wait_queue_head_t ar7240_fr_wq;

#define frdbg printk

/*
 * GPIO interrupt stuff
 */
typedef enum {
    INT_TYPE_EDGE,
    INT_TYPE_LEVEL,
}ar7240_gpio_int_type_t;

typedef enum {
    INT_POL_ACTIVE_LOW,
    INT_POL_ACTIVE_HIGH,
}ar7240_gpio_int_pol_t;


/* 
** Simple Config stuff
*/

#if !defined(IRQ_NONE)
#define IRQ_NONE
#define IRQ_HANDLED
#endif /* !defined(IRQ_NONE) */


typedef irqreturn_t(*sc_callback_t)(int, void *, struct pt_regs *);

static sc_callback_t registered_cb = NULL;
static void *cb_arg;
static volatile int ignore_pushbutton = 0;
static struct proc_dir_entry *simple_config_entry = NULL;
static struct proc_dir_entry *simulate_push_button_entry = NULL;
static struct proc_dir_entry *tricolor_led_entry = NULL;


void ar7240_gpio_config_int(int gpio, 
                       ar7240_gpio_int_type_t type,
                       ar7240_gpio_int_pol_t polarity)
{
    u32 val;

    /*
     * allow edge sensitive/rising edge too
     */
    if (type == INT_TYPE_LEVEL) {
        /* level sensitive */
        ar7240_reg_rmw_set(AR7240_GPIO_INT_TYPE, (1 << gpio));
    }
    else {
       /* edge triggered */
       val = ar7240_reg_rd(AR7240_GPIO_INT_TYPE);
       val &= ~(1 << gpio);
       ar7240_reg_wr(AR7240_GPIO_INT_TYPE, val);
    }

    if (polarity == INT_POL_ACTIVE_HIGH) {
        ar7240_reg_rmw_set (AR7240_GPIO_INT_POLARITY, (1 << gpio));
    }
    else {
       val = ar7240_reg_rd(AR7240_GPIO_INT_POLARITY);
       val &= ~(1 << gpio);
       ar7240_reg_wr(AR7240_GPIO_INT_POLARITY, val);
    }

    ar7240_reg_rmw_set(AR7240_GPIO_INT_ENABLE, (1 << gpio));
}

void
ar7240_gpio_config_output(int gpio)
{
    ar7240_reg_rmw_set(AR7240_GPIO_OE, (1 << gpio));
}

void
ar7240_gpio_config_input(int gpio)
{
    ar7240_reg_rmw_clear(AR7240_GPIO_OE, (1 << gpio));
}

void
ar7240_gpio_out_val(int gpio, int val)
{
    if (val & 0x1) {
        ar7240_reg_rmw_set(AR7240_GPIO_OUT, (1 << gpio));
    }
    else {
        ar7240_reg_rmw_clear(AR7240_GPIO_OUT, (1 << gpio));
    }
}

int
ar7240_gpio_in_val(int gpio)
{
    return((1 << gpio) & (ar7240_reg_rd(AR7240_GPIO_IN)));
}

static void
ar7240_gpio_intr_enable(unsigned int irq)
{
    ar7240_reg_rmw_set(AR7240_GPIO_INT_MASK, 
                      (1 << (irq - AR7240_GPIO_IRQ_BASE)));
}

static void
ar7240_gpio_intr_disable(unsigned int irq)
{
    ar7240_reg_rmw_clear(AR7240_GPIO_INT_MASK, 
                        (1 << (irq - AR7240_GPIO_IRQ_BASE)));
}

static unsigned int
ar7240_gpio_intr_startup(unsigned int irq)
{
	ar7240_gpio_intr_enable(irq);
	return 0;
}

static void
ar7240_gpio_intr_shutdown(unsigned int irq)
{
	ar7240_gpio_intr_disable(irq);
}

static void
ar7240_gpio_intr_ack(unsigned int irq)
{
	ar7240_gpio_intr_disable(irq);
}

static void
ar7240_gpio_intr_end(unsigned int irq)
{
	if (!(irq_desc[irq].status & (IRQ_DISABLED | IRQ_INPROGRESS)))
		ar7240_gpio_intr_enable(irq);
}

static void
ar7240_gpio_intr_set_affinity(unsigned int irq, cpumask_t mask)
{
	/* 
     * Only 1 CPU; ignore affinity request
     */
}

struct hw_interrupt_type ar7240_gpio_intr_controller = {
	"AR7240 GPIO",
	ar7240_gpio_intr_startup,
	ar7240_gpio_intr_shutdown,
	ar7240_gpio_intr_enable,
	ar7240_gpio_intr_disable,
	ar7240_gpio_intr_ack,
	ar7240_gpio_intr_end,
	ar7240_gpio_intr_set_affinity,
};

void
ar7240_gpio_irq_init(int irq_base)
{
	int i;

	for (i = irq_base; i < irq_base + AR7240_GPIO_IRQ_COUNT; i++) {
		irq_desc[i].status  = IRQ_DISABLED;
		irq_desc[i].action  = NULL;
		irq_desc[i].depth   = 1;
		irq_desc[i].handler = &ar7240_gpio_intr_controller;
	}
}


void register_simple_config_callback (void *callback, void *arg)
{
    registered_cb = (sc_callback_t) callback;
    cb_arg = arg;
}
EXPORT_SYMBOL(register_simple_config_callback);

void unregister_simple_config_callback (void)
{
    registered_cb = NULL;
    cb_arg = NULL;
}
EXPORT_SYMBOL(unregister_simple_config_callback);

int ar7240_simple_config_invoke_cb(int simplecfg_only, int irq_enable, 
                                   int cpl, struct pt_regs *regs)
{
    if (simplecfg_only) {
        if (ignore_pushbutton) {
            ar7240_gpio_config_int (JUMPSTART_GPIO, INT_TYPE_LEVEL, INT_POL_ACTIVE_HIGH);
            ignore_pushbutton = 0;
            return IRQ_HANDLED;
        }

        ar7240_gpio_config_int (JUMPSTART_GPIO, INT_TYPE_LEVEL, INT_POL_ACTIVE_LOW);
        ignore_pushbutton = 1;
    }

    if (irq_enable)
        local_irq_enable();

    printk ("\nar7240: calling simple_config callback..\n");

    if (registered_cb) {
        return (registered_cb (cpl, cb_arg, regs));
    }

    return IRQ_HANDLED;

}
/*
 * Irq for front panel SW jumpstart switch
 * Connected to XSCALE through GPIO4
 */
irqreturn_t jumpstart_irq(int cpl, void *dev_id, struct pt_regs *regs)
{
    unsigned int delay;

    if (atomic_read(&ar7240_fr_status))
    {
        local_irq_disable();

#define UDELAY_COUNT 4000

        for (delay = UDELAY_COUNT; delay; delay--) {
            if (ar7240_gpio_in_val(JUMPSTART_GPIO)) {
                break;
            }
            udelay(1000);
        }

        if (!delay) {
            atomic_dec(&ar7240_fr_status);
            /* 
             * since we are going to reboot the board, we
             * don't need the interrupt handler anymore,
             * so disable it. 
             */
            disable_irq(AR7240_GPIO_IRQn(JUMPSTART_GPIO));
            wake_up(&ar7240_fr_wq);
            printk("\nar7240: factory configuration restored..\n");
            local_irq_enable();
            return IRQ_HANDLED;
        } else {
            return (ar7240_simple_config_invoke_cb(0, 1, cpl, regs));
        }
    }
    else
        return (ar7240_simple_config_invoke_cb(1, 0, cpl, regs));
}

static int push_button_read (char *page, char **start, off_t off,
                               int count, int *eof, void *data)
{
    return 0;
}

static int push_button_write (struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    if (registered_cb) {
        registered_cb (0, cb_arg, 0);
    }
    return count;
}

#define TRICOLOR_LED_GREEN_PIN  5         /* GPIO 5 */
#define TRICOLOR_LED_YELLOW_PIN 4         /* GPIO 4 */
#define OFF 0
#define ON 1

typedef enum {
        LED_STATE_OFF   =       0,
        LED_STATE_GREEN =       1,
        LED_STATE_YELLOW =      2,
        LED_STATE_ORANGE =      3,
        LED_STATE_MAX =         4
} led_state_e;

static led_state_e gpio_tricolorled = LED_STATE_OFF;

static int gpio_tricolor_led_read (char *page, char **start, off_t off,
               int count, int *eof, void *data)
{
    return sprintf (page, "%d\n", gpio_tricolorled);
}

static int gpio_tricolor_led_write (struct file *file, const char *buf,
                                        unsigned long count, void *data)
{
    u_int32_t val, green_led_onoff = 0, yellow_led_onoff = 0;

    if (sscanf(buf, "%d", &val) != 1)
        return -EINVAL;

    if (val >= LED_STATE_MAX)
        return -EINVAL;

    if (val == gpio_tricolorled)
        return count;

    switch (val) {
        case LED_STATE_OFF :
                green_led_onoff = OFF;   /* both LEDs OFF */
                yellow_led_onoff = OFF;
                break;

        case LED_STATE_GREEN:
                green_led_onoff = ON;    /* green ON, Yellow OFF */
                yellow_led_onoff = OFF;
                break;

        case LED_STATE_YELLOW:
                green_led_onoff = OFF;   /* green OFF, Yellow ON */
                yellow_led_onoff = ON;
                break;

        case LED_STATE_ORANGE:
                green_led_onoff = ON;    /* both LEDs ON */
                yellow_led_onoff = ON;
                break;
    }

    ar7240_gpio_out_val (TRICOLOR_LED_GREEN_PIN, green_led_onoff);
    ar7240_gpio_out_val (TRICOLOR_LED_YELLOW_PIN, yellow_led_onoff);
    gpio_tricolorled = val;

    return count;
}


static int create_simple_config_led_proc_entry (void)
{
    if (simple_config_entry != NULL) {
        printk ("Already have a proc entry for /proc/simple_config!\n");
        return -ENOENT;
    }

    simple_config_entry = proc_mkdir("simple_config", NULL);
    if (!simple_config_entry)
        return -ENOENT;

    simulate_push_button_entry = create_proc_entry ("push_button", 0644,
                                                      simple_config_entry);
    if (!simulate_push_button_entry)
        return -ENOENT;

    simulate_push_button_entry->write_proc = push_button_write;
    simulate_push_button_entry->read_proc = push_button_read;

    tricolor_led_entry = create_proc_entry ("tricolor_led", 0644,
                                            simple_config_entry);
    if (!tricolor_led_entry)
        return -ENOENT;

    tricolor_led_entry->write_proc = gpio_tricolor_led_write;
    tricolor_led_entry->read_proc = gpio_tricolor_led_read;

    /* configure gpio as outputs */
    ar7240_gpio_config_output (TRICOLOR_LED_GREEN_PIN); 
    ar7240_gpio_config_output (TRICOLOR_LED_YELLOW_PIN); 

    /* switch off the led */
    ar7240_gpio_out_val(TRICOLOR_LED_GREEN_PIN, OFF);
    ar7240_gpio_out_val(TRICOLOR_LED_YELLOW_PIN, OFF);

    return 0;
}

static int
ar7240fr_open(struct inode *inode, struct file *file)
{
	if (MINOR(inode->i_rdev) != FACTORY_RESET_MINOR) {
		return -ENODEV;
	}

	if (ar7240_fr_opened) {
		return -EBUSY;
	}

        ar7240_fr_opened = 1;
	return nonseekable_open(inode, file);
}

static int
ar7240fr_close(struct inode *inode, struct file *file)
{
	if (MINOR(inode->i_rdev) != FACTORY_RESET_MINOR) {
		return -ENODEV;
	}

	ar7240_fr_opened = 0;
	return 0;
}

static ssize_t
ar7240fr_read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	return -ENOTSUPP;
}

static ssize_t
ar7240fr_write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	return -ENOTSUPP;
}

static int
ar7240fr_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
		unsigned long arg)
{
	int ret = 0;

	switch(cmd) {
		case AR7240_FACTORY_RESET:
                        atomic_inc(&ar7240_fr_status);
			sleep_on(&ar7240_fr_wq);
			break;

		default: ret = -EINVAL;
	}

	return ret;
}


static struct file_operations ar7240fr_fops = {
	read:	ar7240fr_read,
	write:	ar7240fr_write,
	ioctl:	ar7240fr_ioctl,
	open:	ar7240fr_open,
	release:ar7240fr_close
};

static struct miscdevice ar7240fr_miscdev = 
{ FACTORY_RESET_MINOR, "Factory reset", &ar7240fr_fops };

int __init ar7240_simple_config_init(void)
{
#ifdef CONFIG_CUS100
	u32 mask = 0;
#endif
    int req, ret;

    ret = misc_register(&ar7240fr_miscdev);

    if (ret < 0) {
            printk("*** ar7240 misc_register failed %d *** \n", ret);
            return -1;
    }


#ifdef CONFIG_CUS100
	mask = ar7240_reg_rd(AR7240_MISC_INT_MASK);
	ar7240_reg_wr(AR7240_MISC_INT_MASK, mask | (1 << 2)); /* Enable GPIO interrupt mask */
        ar7240_gpio_config_int (JUMPSTART_GPIO, INT_TYPE_LEVEL,INT_POL_ACTIVE_HIGH);
	ar7240_gpio_intr_enable(JUMPSTART_GPIO);
	ar7240_gpio_config_input(JUMPSTART_GPIO);
#else
	ar7240_gpio_config_input(JUMPSTART_GPIO);
	/* configure Jumpstart GPIO as level triggered interrupt */
	ar7240_gpio_config_int (JUMPSTART_GPIO, INT_TYPE_LEVEL, INT_POL_ACTIVE_HIGH);
	printk("%s (%s) JUMPSTART_GPIO: %d\n", __FILE__, __func__, JUMPSTART_GPIO);

	ar7240_reg_rmw_clear(AR7240_GPIO_FUNCTIONS, (1 << 2));
	ar7240_reg_rmw_clear(AR7240_GPIO_FUNCTIONS, (1 << 16));
	ar7240_reg_rmw_clear(AR7240_GPIO_FUNCTIONS, (1 << 20));
#endif


    req = request_irq (AR7240_GPIO_IRQn(JUMPSTART_GPIO), jumpstart_irq, 0,
                       "SW JUMPSTART/FACTORY RESET", NULL);
    if (req != 0) {
        printk (KERN_ERR "unable to request IRQ for SWJUMPSTART GPIO (error %d)\n", req);
        misc_deregister(&ar7240fr_miscdev);
        ar7240_gpio_intr_shutdown(AR7240_GPIO_IRQn(JUMPSTART_GPIO));
        return -1;
    }

    init_waitqueue_head(&ar7240_fr_wq);

    return 0;
}
/* 
 * used late_initcall so that misc_register will succeed 
 * otherwise, misc driver won't be in a initializated state
 * thereby resulting in misc_register api to fail.
 */
late_initcall(ar7240_simple_config_init);
