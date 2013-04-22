/*
 * Linux GPIO char driver
 *
 * Copyright 2008, Gemtek Corporation
 * All Rights Reserved.
 * 
 */

#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/poll.h>
//#include <asm/gdb-stub.h>
#include "ar7100.h"
#include "../../include/utility.h"

// +++ Gemtek

#define GPIO_TOTAL 					16
#define GPIO_TEXT_LEN 			10

#define GPIO00							0
#define GPIO01							1
#define GPIO02							2
#define GPIO03							3
#define GPIO04							4
#define GPIO05							5
#define GPIO06							6
#define GPIO07							7
#define GPIO08							8
#define GPIO09							9
#define GPIO10							10
#define GPIO11							11
#define GPIO12							12
#define GPIO13							13
#define GPIO14							14
#define GPIO15							15

#define TICK_RES				  1000
#define HWCTL_DELAY				200	/* 300 msec */
#define HWCTL_RESTDELAY		9 * TICK_RES/HWCTL_DELAY	/* 9 sec for Restore To Default */
#define HWCTL_WPSDELAY		300	/* 0.5 sec for WPS */
#define HWCTL_WPSTIME			119 * TICK_RES*HZ/HWCTL_DELAY
static struct timer_list	hwctl_timer;
static struct timer_list	watchdog_timer;
wait_queue_head_t read_queue;
static unsigned int		btick = 0, wpsbtick = 0, flag=0, res = 0, data_avail_to_read=0, wpstime = 0;
extern int disabled_fastnat;

unsigned long gpio = 0;
struct GPIO_data_d {
	char GPIO_NAME[GPIO_TEXT_LEN];
	unsigned long GPIO_PIN_NUM;
}GPIO_data[GPIO_TOTAL];

unsigned char GPIO_LIST_NAME[GPIO_TOTAL][GPIO_TEXT_LEN] =
{
	"GPIO00" ,
	"GPIO01" ,
	"GPIO02" ,
	"GPIO03" ,
	"GPIO04" ,
	"GPIO05" ,
	"GPIO06" ,
	"GPIO07" ,
	"GPIO08" ,
	"GPIO09" ,
	"GPIO10" ,
	"GPIO11" ,
	"GPIO12" ,
	"GPIO13" ,
	"GPIO14" ,
	"GPIO15"
};

unsigned long GPIO_LIST_PIN[GPIO_TOTAL] =
{
	0 ,
	1 ,
	2 ,
	3 ,
	4 ,
	5 ,
	6 ,
	7 ,
	8 ,
	9 ,
	10 ,
	11 ,
	12 ,
	13 ,
	14 ,
	15
};

#if 0
#define DEBUGP(format, args...) printk(KERN_EMERG"%s:%s: " format, __FILE__, __FUNCTION__, ## args)
#else
#define DEBUGP(x, args...)
#endif
#define OFF 0
#define ON 1
static int start_monitor=1;
static int button_test=0;
static int watchdog_status=0;
static int stop_clock=0;

static void LED_ON(int GPIO_LED)
{
	ar7100_gpio_out_val(GPIO_LED, ON);
}

static void LED_OFF(int GPIO_LED)
{	
	ar7100_gpio_out_val(GPIO_LED, OFF);
}

static void enabled_watchdog(unsigned long d)
{	
	LED_OFF(WATCHDOG_ENABLED);
	del_timer(&watchdog_timer);
}

static void hwctl_gpiomonitor(unsigned long d)
{	
	u32 reg=0;
#if 1
	if ( start_monitor == 1 ) {
		if(stop_clock == 0) {
			if (watchdog_status==0)
			{
			 	LED_OFF(WATCHDOG_CLK);
				watchdog_status=1;
			}
			else
			{
			 	LED_ON(WATCHDOG_CLK);	
				watchdog_status=0;
			}
		}
	
		if (button_test==1)
		{	// Production Function
			// Reset Button
			if (ar7100_gpio_in_val(RESET_BUTTON))
			{	// Button Release
				LED_ON(POWER_LED);
				//LED_ON(GPIO04);
			}		
			else
			{	// Button Press
				printk("<1>Reset Button\n");
				if (flag%2)
				{
					LED_ON(POWER_LED);
//					LED_OFF(WSC_GLED_GPIO);
				}
				else
				{
					LED_OFF(POWER_LED);
//					LED_ON(WSC_GLED_GPIO);
				}
				flag ++;
			}
				
			// Securtiy Button	
			if (ar7100_gpio_in_val(WLSEC_BUTTON))
			{	// Button Release
				LED_ON(WSC_OLED_GPIO);
//				LED_ON(WSC_GLED_GPIO);
			}		
			else
			{	// Button Press
				printk("<1>WPS Button\n");
				if (flag%2)
				{
//					LED_ON(WSC_GLED_GPIO);
					LED_OFF(WSC_OLED_GPIO);
				}
				else
				{
//					LED_OFF(WSC_GLED_GPIO);
					LED_ON(WSC_OLED_GPIO);
				}
				flag ++;
			}
		}
		else
		{
			// Normal Function
			// Reset default Button
			
			if (ar7100_gpio_in_val(RESET_BUTTON))
			{	// Button Release
				if(btick)
				{
					if((btick <= HWCTL_RESTDELAY))
					{
						printk("<1>#      << REBOOT >> %d\n", btick);
						res = RES_REBOOT;
						kill_proc(1, SIGUSR1, 1);				
						kill_proc(1, SIGUSR1, 1);				
						kill_proc(1, SIGUSR1, 1);				
						//return;
					}
					else
					{
						printk("<1>#      << RESET TO DEFAULT >> %d\n", btick);
						res = RES_DEFAULT;
						kill_proc(1, SIGUSR2, 1);				
						kill_proc(1, SIGUSR2, 1);				
						kill_proc(1, SIGUSR2, 1);				
						//return;
						// Stop GPIO Checking Routine, and wait for user space monitor
						// to trigger Restore To Default Event	
					}
				}
			}		
			else
			{	// Button Press
				if(++btick > HWCTL_RESTDELAY)	// 10 sec
				{
					printk("<1>#      << RESET TO DEFAULT >>\n", btick);
					res = RES_DEFAULT;
					kill_proc(1, SIGUSR2, 1);
					kill_proc(1, SIGUSR2, 1);
					kill_proc(1, SIGUSR2, 1);
					//return;
				}
			}
//sam add +++ 2008.11.14 for hw push button
			// Securtiy Button	
			if (ar7100_gpio_in_val(WLSEC_BUTTON))
			{	// Button Release
			}
			else
			{	// Button Press
				printk("<1>#      << PUSH BUTTON BE PRESSED >>\n", btick);
				kill_proc(1, 30, 1);	//SIGUSR1   30,10,16    Term    User-defined signal 1
					//return;
			}
//sam add --- 2008.11.14
		}
	}
#endif
	/* Finish */
	if( timer_pending(&hwctl_timer) )
		del_timer(&hwctl_timer);
	hwctl_timer.function = hwctl_gpiomonitor;
	hwctl_timer.data = gpio;
 	mod_timer(&hwctl_timer, jiffies + (HWCTL_DELAY * HZ / 1000));
}

static int GPIO_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char buf[10] = "";
	int len = 0;
	struct GPIO_data_d *gpio = (struct GPIO_data_d *)data;
	
	DEBUGP("GPIO_write_proc : got [%s], pin num is %ld\n", gpio->GPIO_NAME, gpio->GPIO_PIN_NUM);
	
	DEBUGP("GPIO_write_proc : get %lu byte.\n", count);
	if(count >= 10)
	{
		len = 9;
	}
	else
	{
		len = count;
	}
	
	DEBUGP("GPIO_write_proc : after check ==> get %d byte.\n", len);
	
	if(copy_from_user(buf, buffer, len))
	{
		DEBUGP("GPIO_write_proc : fail.\n");		
		return -EFAULT;
	}
	
	DEBUGP("GPIO_write_proc : get data...[%s].\n", buf);
	
	if(buf[0] == '0')
	{
// for Security button production test
		if (gpio->GPIO_PIN_NUM==11) {
			start_monitor = 0;
			DEBUGP("GPIO_write_proc : set to OUT LOW. %d, %d\n", gpio->GPIO_PIN_NUM, start_monitor);
		}
		if (gpio->GPIO_PIN_NUM==12)
			button_test = 0;		
		if (gpio->GPIO_PIN_NUM==13)
			stop_clock = 0;		
		LED_OFF(gpio->GPIO_PIN_NUM);
		DEBUGP("GPIO_write_proc : set to OUT LOW.\n");
	}
	else if(buf[0] == '1')
	{
// for Security button production test
		if (gpio->GPIO_PIN_NUM==11) {
			if( timer_pending(&hwctl_timer) )
				del_timer(&hwctl_timer);
			hwctl_timer.function = hwctl_gpiomonitor;
			hwctl_timer.expires  = jiffies + HZ *30;
			hwctl_timer.data = gpio;
		 	mod_timer(&hwctl_timer, jiffies + HZ *30);
			start_monitor = 1;
			DEBUGP("GPIO_write_proc : set to OUT HIGH. %d, %d, hwctl_timer.expires %d\n", gpio->GPIO_PIN_NUM, start_monitor, hwctl_timer.expires);
		}
		if (gpio->GPIO_PIN_NUM==12)
			button_test = 1;
		if (gpio->GPIO_PIN_NUM==13)
			stop_clock = 1;		
		LED_ON(gpio->GPIO_PIN_NUM);
		DEBUGP("GPIO_write_proc : set to OUT HIGH.\n");
	}
	else
	{
		DEBUGP("GPIO_write_proc : Unknown...do nothing.\n");
	}
	return len;
}  
          
static int GPIO_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	struct GPIO_data_d *gpio = (struct GPIO_data_d *)data;
	
	DEBUGP("GPIO_read_proc : got [%s], pin num is %ld\n", gpio->GPIO_NAME, gpio->GPIO_PIN_NUM);
	return len;
}

static struct proc_dir_entry *create_proc_read_write_entry(const char *name,
	mode_t mode, struct proc_dir_entry *base, 
	read_proc_t *read_proc, write_proc_t *write_proc, void * data)
{
	struct proc_dir_entry *res=create_proc_entry(name,mode,base);
	if (res) {
		res->read_proc=read_proc;
		res->write_proc=write_proc;
		res->data=data;
	}
	return res;
}

// --- Gemtek

static int __init	gtk_gpio_init(void)
{
	int i=0;	
  int req;
  unsigned long gpio;
  
	for(i = 0;i < GPIO_TOTAL; i++)
	{
		strcpy(GPIO_data[i].GPIO_NAME, GPIO_LIST_NAME[i]);
		GPIO_data[i].GPIO_PIN_NUM = GPIO_LIST_PIN[i];
		if(create_proc_read_write_entry(GPIO_data[i].GPIO_NAME,0,NULL,GPIO_read_proc,GPIO_write_proc,(void *)&GPIO_data[i]) == NULL)
		{
			DEBUGP("%s : fail\n", GPIO_data[i].GPIO_NAME);
		}
		else
		{
			DEBUGP("%s : success\n", GPIO_data[i].GPIO_NAME);			
		}
		
	}	
  /* configure gpio as outputs */
  ar7100_gpio_config_output (POWER_LED); 
  //ar7100_gpio_config_output (WSC_GLED_GPIO); 
  ar7100_gpio_config_output (WSC_OLED_GPIO); 
  ar7100_gpio_config_output (WATCHDOG_ENABLED); 
  ar7100_gpio_config_output (WATCHDOG_CLK); 

  /* switch off the led */
  LED_OFF(POWER_LED);
  //LED_ON(WSC_GLED_GPIO); //Pull high to extinguish the WPS LED
  LED_ON(WSC_OLED_GPIO); //Pull high to extinguish the WPS LED
  LED_ON(WATCHDOG_CLK);  //Pull high to disable WatchDog
  LED_ON(WATCHDOG_ENABLED);  //Pull high to disable WatchDog
  
	ar7100_gpio_config_input(RESET_BUTTON);
	ar7100_gpio_config_input(WLSEC_BUTTON);
	init_timer(&hwctl_timer);
	hwctl_timer.function = hwctl_gpiomonitor;
	hwctl_timer.data = gpio;
 	mod_timer(&hwctl_timer, jiffies + (HZ / 1000));
 	
 	
	init_timer(&watchdog_timer);
	watchdog_timer.function = enabled_watchdog;
	watchdog_timer.data = gpio;
 	mod_timer(&watchdog_timer, jiffies + (30000 * HZ / 1000));
 	return 0;
}

static void __exit gtk_gpio_exit(void)
{	
	int i=0;
	for(i = 0;i < GPIO_TOTAL; i++)
	{
		strcpy(GPIO_data[i].GPIO_NAME, GPIO_LIST_NAME[i]);
		GPIO_data[i].GPIO_PIN_NUM = GPIO_LIST_PIN[i];
		remove_proc_entry(GPIO_data[i].GPIO_NAME,NULL);
	}
// +++ Gemtek	
	del_timer(&hwctl_timer);
// --- Gemtek
 	return 0;
}

module_init(gtk_gpio_init);
module_exit(gtk_gpio_exit);
