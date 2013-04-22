/*
 *  linux/drivers/char/mem.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  Added devfs support. 
 *    Jan-11-1998, C. Scott Ananian <cananian@alumni.princeton.edu>
 *  Shared /dev/zero mmaping support, Feb 2000, Kanoj Sarcar <kanoj@sgi.com>
 */

#include <linux/config.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/tpqic02.h>
#include <linux/ftape.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/raw.h>
#include <linux/tty.h>
#include <linux/capability.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/pgalloc.h>
#include <linux/proc_fs.h>
#include "../../../../drivers/ar531x-watchdog/ar531xlnx.h"
#include "../../../../drivers/ar531x-watchdog/ar531x.h"
#include "../../../../user/include/utility.h"
//#define __OSCAR
#ifdef __OSCAR
#define MSG(string, args...)	printk(KERN_DEBUG "oscar:"string, ##args)
#else
#define MSG(string, args...)
#endif

#define OSCAR_IOCTL_BASE	0xbb
#define	OSCAR_LED_ON		_IO(OSCAR_IOCTL_BASE , 1)
#define	OSCAR_LED_OFF		_IO(OSCAR_IOCTL_BASE , 2)

#define SCHAR_MAJOR		42
#define GPIO_WLLINK_LED		6

static struct gpio_cmd {
	int command;
	int gpio_pin;
	int	action;
};
devfs_handle_t led_handle;
int led_res;
static int led_blink_started=0;
static struct	timer_list timer_led_on;
static struct	timer_list timer_led_off;
static int current_led=0;

static int schar_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int schar_open(struct inode *inode, struct file *file);
static int schar_release(struct inode *inode, struct file *file);
static void blink_led_on();
static void blink_led_off();

static struct file_operations schar_fops = {
	ioctl:		schar_ioctl,
	open:		schar_open,
	release:	schar_release,
};

static void *gpio_sbh;


int led_init(void)
{
	
	//if (!(gpio_sbh = sb_kattach(SB_OSH)))
	//	return -ENODEV;

	//sb_gpiosetcore(gpio_sbh);

	MSG("init module");

	led_res = register_chrdev(SCHAR_MAJOR, "ledctrl", &schar_fops);
	
	led_handle = devfs_register(NULL, "ledctrl", DEVFS_FL_DEFAULT,
	                                led_res, 0, S_IFCHR | S_IRUGO | S_IWUGO,
	                                &schar_fops, NULL);
		
	if(led_res)
		MSG("can't register device with kernel\n");
	
	//current_led = POWER_LED;
	//blink_led_on();	
	//led_blink_started = 1;
		
	return led_res;
}
static void __exit
led_exit(void)
{
	if (led_handle != NULL)
		devfs_unregister(led_handle);
	led_handle = NULL;
	devfs_unregister_chrdev(led_res, "ledctrl");
	//sb_detach(gpio_sbh);
	
}

/* Enabling the specific BIT of the control register of GPIO */
void	start_GPIO_Access ( unsigned int GPIO_LED )
{
	unsigned int		GPIOValue = 0;

	/* read the current value of GPIO control register */
	GPIOValue = sysRegRead ( AR531XPLUS_GPIO_CR );

	/* Enabling the specific BIT of the control register of GPIO */
	GPIOValue |= ( 1 << GPIO_LED );

	/* Write back the control register of GPIO */
	sysRegWrite ( AR531XPLUS_GPIO_CR , GPIOValue );

	return;
}

/* Disabling the specific BIT of the control register of GPIO */
void	stop_GPIO_Access ( unsigned int GPIO_LED )
{
	unsigned int		GPIOValue = 0;

	/* read the current value of GPIO control register */
	GPIOValue = sysRegRead ( AR531XPLUS_GPIO_CR );

	/* disable the specific BIT of the control register of GPIO */
	GPIOValue &= ~( 1 << GPIO_LED );

	/* Write back the control register of GPIO */
	sysRegWrite ( AR531XPLUS_GPIO_CR , GPIOValue );

	return;
}

static void blink_led_on()
{
	unsigned long k=0;
	
	if( !timer_pending(&timer_led_on) )
	{
		led_on(current_led);
		del_timer(&timer_led_off);
		init_timer(&timer_led_on);
		timer_led_on.data = k;
		timer_led_on.function = blink_led_off;
		timer_led_on.expires = jiffies + HZ/3;
		add_timer(&timer_led_on);
	}
}

static void blink_led_off()
{
	unsigned long k=0;
	
	if( !timer_pending(&timer_led_off) )
	{
		led_off(current_led);
		if( timer_pending(&timer_led_on) )
				del_timer(&timer_led_on);
				
		init_timer(&timer_led_off);
		timer_led_off.data = k;
		timer_led_off.function = blink_led_on;	
		timer_led_off.expires = jiffies + HZ/3;
		add_timer(&timer_led_off);
	}	
}


static int schar_open(struct inode *inode, struct file *file)
{
	MOD_INC_USE_COUNT;
	MSG("Module open\n");
	return 0;
}

static int schar_release(struct inode *inode, struct file *file)
{
	MOD_DEC_USE_COUNT;
	MSG("Module close\n");
	return 0;
}

/*
 * turn on led
 */
void led_on(unsigned int GPIO_LED)
{
#if 0
 	sb_gpioout	( gpio_sbh , ( 1 << GPIO_LED ) , 0 ,0);
	sb_gpioouten( gpio_sbh , ( 1 << GPIO_LED ) , ( 1 << GPIO_LED ) ,0);
#endif
	unsigned int		GPIOValue = 0;

	/* read the current value of GPIO register ( INPUT ) */
	GPIOValue = sysRegRead ( AR531XPLUS_GPIO_DI );

	/* Enabling the specific BIT of the OUTPUT register of GPIO */
	GPIOValue |= ( 1 << GPIO_LED );

	/* Enabling the specific BIT of the control register of GPIO */
	start_GPIO_Access ( GPIO_LED );

	/* Write back the OUTPUT register of GPIO */
	sysRegWrite ( AR531XPLUS_GPIO_DO , GPIOValue );

}

/*
 * turn off led
 */
void led_off(unsigned int GPIO_LED)
{
#if 0
	sb_gpioout	( gpio_sbh , ( 1 << GPIO_LED ) , ( 1 << GPIO_LED ) ,GPIO_DRV_PRIORITY);
  sb_gpioouten( gpio_sbh , ( 1 << GPIO_LED ) , ( 1 << GPIO_LED ) ,GPIO_DRV_PRIORITY);
#endif

	unsigned int		GPIOValue = 0;

	GPIOValue = sysRegRead ( AR531XPLUS_GPIO_DI );
	GPIOValue &= ~( 1 << GPIO_LED );
	start_GPIO_Access ( GPIO_LED );
	sysRegWrite ( AR531XPLUS_GPIO_DO , GPIOValue );
}

static int schar_ioctl(struct inode *inode, struct file *file,
		unsigned int cmd, unsigned long arg)
{
	MSG("Enter IOCTL\n");
	struct gpio_cmd ioctl_data;
	copy_from_user( &ioctl_data , (struct gpio_cmd *)arg , sizeof(ioctl_data) );
	switch(ioctl_data.command)
	{
		case NORMAL:
			if( ioctl_data.action == ON )
				led_off(ioctl_data.gpio_pin);
			else
				led_on(ioctl_data.gpio_pin);
			break;
		case BLINK:
			if( ioctl_data.action == ON )
			{
				if( !led_blink_started )
				{
					blink_led_on();
					current_led = ioctl_data.gpio_pin;
					led_blink_started = 1;
				}
				else
					return -1;				
			}
			else	
			{
				if( timer_pending(&timer_led_off) )
						del_timer(&timer_led_off);
				if( timer_pending(&timer_led_on) )
						del_timer(&timer_led_on);
				led_blink_started = 0;
				led_on(ioctl_data.gpio_pin);		
			}
			break;
		default:
			break;
	}
	return 0;
}

int ledctrl( int command , int pin , int action )
{
	switch(command)
	{
		case NORMAL:
			if( action == ON )
				led_on(pin);
			else
				led_off(pin);
			break;
		case BLINK:
			if( action == ON )
			{
				if( !led_blink_started )
				{
					blink_led_on();
					current_led = pin;
					led_blink_started = 1;
				}
				else
					return -1;				
			}
			else	
			{
				if( timer_pending(&timer_led_off) )
						del_timer(&timer_led_off);
				if( timer_pending(&timer_led_on) )
						del_timer(&timer_led_on);
				led_blink_started = 0;
				led_on(pin);		
			}
			break;
		default:
			break;
	}
	return 0;
}

EXPORT_SYMBOL(ledctrl);
module_init(led_init);
module_exit(led_exit);
