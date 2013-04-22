#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/ioport.h>
#include <linux/fcntl.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/system.h>
#include <asm/delay.h>
#include <linux/notifier.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/smp_lock.h>
#include <linux/wait.h>
#include <linux/irq.h>
#include "ar7100.h"
#include "../../include/utility.h"

#define	Keep_1_20_second		( HZ/20 )   // keep 1/20 second
#define	Keep_1_10_second		( HZ/10 )   // keep 1/10 second
#define	Keep_1_4_second		( HZ/4 )   // keep 1/4 second
#define	Keep_half_second		( HZ/2 )   // keep 1/2 second
#define	Keep_1_second		( 1 * HZ )  // keep 1 second
#define	Keep_3_second		( 3 * HZ )  // keep 1 second

#define		file_name_size		128
#define	led_ctrl_wsc_folder_name		"wsc_led_ctrl"

static struct proc_dir_entry	*led_ctrl_wsc_dir = NULL;
static struct proc_dir_entry	*entry = NULL;
static struct timer_list			wps_led_blinking_timer;
unsigned char	wps_led_blinking_flag = 0;
int	wps_led_error_status_counter = 0;
int	wps_led_overlap_status_counter = 0;
unsigned char	wps_prcoess_success_led_flag = 0; // 0: fail , 1: success
unsigned int	wps_blinking_latency = Keep_half_second;
unsigned int	wps_status_blinking_type=0;  // 0: ON , 1 : In Progress ,2: Error , 4: Success
//sam add +++ 2008.12.16
unsigned char	wps_configed_flag = 0; // 0: unconfiged , 1: config
static int wps_configed ( char *, char ** , off_t  , int  , int * , void * );
//sam add --- 2008.12.16
static int wsc_led_on ( char *, char ** , off_t  , int  , int * , void * );
static int wsc_led_off ( char *, char ** , off_t  , int  , int * , void * );
static int wsc_led_inprogress ( char *, char ** , off_t  , int  , int * , void * );
static int wsc_led_error ( char *, char ** , off_t  , int  , int * , void * );
static int wsc_led_overlap ( char *, char ** , off_t  , int  , int * , void * );
static int wsc_led_success ( char *, char ** , off_t  , int  , int * , void * );

struct GPIO_Fun {
	unsigned char		file_name [ file_name_size ];
	mode_t			mode;
	void   				(*file_handler);
};

struct GPIO_Fun WPS_GPIOTbl [] = {
	/* { file_name , mode , file_handler } */
	{ "ON_wsc_led" , 0444 , wsc_led_on } ,
	{ "STOP_wsc_led" , 0444 , wsc_led_off } ,
	{ "WPS_LED_InProgress" , 0444 , wsc_led_inprogress } ,
	{ "WPS_LED_Error" , 0444 , wsc_led_error } ,
	{ "WPS_LED_Overlap" , 0444 , wsc_led_overlap } ,
	{ "WPS_LED_Success" , 0444 , wsc_led_success } ,
//sam add +++ 2008.12.16
	{ "WPS_CONFIGED" , 0444 , wps_configed } ,
//sam add --- 2008.12.16
	{ "" , 0000 , NULL }
};

//sam add +++ 2008.12.16
static int wps_configed ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_configed_flag=1;
	printk ( "%s->%s(%d),wps_configed_flag=%d\n", __FILE__, __FUNCTION__, __LINE__, wps_configed_flag);
	return	0;
}
//sam add --- 2008.12.16

/* LED on : Pull high the specific BIT of GPIO */
void	led_off ( unsigned int GPIO_LED )
{
	ar7100_gpio_out_val(GPIO_LED, ON);

	return;
}

/* LED off : Pull low the specific BIT of GPIO */
void	led_on ( unsigned int GPIO_LED )
{
	ar7100_gpio_out_val(GPIO_LED, OFF);

	return;
}

void	wps_led_blinking_timeout ( unsigned long NULLPTR )
{
/*
	//led_off ( WSC_OLED_GPIO );
	if ( 0 == wps_led_blinking_flag )
	{
//		led_on ( WSC_GLED_GPIO );
		wps_led_blinking_flag = 1;
	}
	else if ( 1 == wps_led_blinking_flag )
	{
//		led_off ( WSC_GLED_GPIO );
		wps_led_blinking_flag = 0;
	}
	else
	{
//		led_on ( WSC_GLED_GPIO );
		wps_led_blinking_flag = 0;
	}
*/
	
	switch(wps_status_blinking_type)
	{
		case 0:
			if(wps_led_blinking_flag==0)  // LED ON now
			{
				wps_blinking_latency=Keep_1_second*3;
				wps_led_blinking_flag=1;
			}
			else
			{
				wps_led_blinking_flag=1;
			}
			break;
		case 1:
//			led_off ( WSC_OLED_GPIO );
			if ( 0 == wps_led_blinking_flag )
			{
				led_on ( WSC_OLED_GPIO );
				wps_led_blinking_flag = 1;
			}
			else if ( 1 == wps_led_blinking_flag )
			{
				led_off ( WSC_OLED_GPIO );
				wps_led_blinking_flag = 0;
			}
			else;
			if(wps_led_blinking_flag==0)  // LED ON now
			{
				wps_blinking_latency=Keep_1_10_second*2;
			}
			else
			{
				wps_blinking_latency=Keep_1_10_second;
			}
			break;
		case 2:
				wps_led_error_status_counter=0;
				wps_led_blinking_flag=0;
				del_timer ( &wps_led_blinking_timer );
				led_off ( WSC_OLED_GPIO );
//sam modify +++ 2008.11.12 for adding WPS LED control
//orig				led_off ( WSC_GLED_GPIO );
//orig				led_on ( WSC_OLED_GPIO );
//sam add +++ 2008.12.16
//orig	led_off ( WSC_GLED_GPIO );
//orig	led_off ( WSC_OLED_GPIO );
/*
				if(!wps_configed_flag)
				{
//				led_off ( WSC_GLED_GPIO );
				///led_off ( WSC_OLED_GPIO );
					led_off ( WSC_OLED_GPIO );
				}
				else
				{
//					led_on ( WSC_GLED_GPIO );
					///led_off ( WSC_OLED_GPIO );
					led_on ( WSC_OLED_GPIO );
				}
*/
//sam add --- 2008.12.16
//sam modify --- 2008.11.12
				return;
			//}
			break;
		case 3:
			led_off ( WSC_OLED_GPIO );
			if(wps_led_overlap_status_counter <=8)
			{
				wps_blinking_latency=Keep_1_10_second;
				wps_led_overlap_status_counter++;
			}
			else
			{
				wps_blinking_latency=Keep_1_10_second*6;
				wps_led_overlap_status_counter=0;
			}
			break;
		case 4:
				wps_led_blinking_flag=0;
				del_timer ( &wps_led_blinking_timer );
				led_off ( WSC_OLED_GPIO );
//sam modify +++ 2008.11.12 for adding WPS LED control
//orig				led_on ( WSC_GLED_GPIO );
//orig				led_off ( WSC_OLED_GPIO );
//sam add +++ 2008.12.16
//orig	led_off ( WSC_GLED_GPIO );
//orig	led_off ( WSC_OLED_GPIO );
/*
				if(!wps_configed_flag)
				{
//				led_off ( WSC_GLED_GPIO );
				led_off ( WSC_OLED_GPIO );
				}
				else
				{
//					led_on ( WSC_GLED_GPIO );
					led_off ( WSC_OLED_GPIO );
				}
*/
//sam add --- 2008.12.16
//sam modify --- 2008.11.12
				return;
			break;
		default:
			wps_blinking_latency=Keep_1_second;
			break;
	}

	mod_timer ( &wps_led_blinking_timer , jiffies + wps_blinking_latency );

	return;
}

void	Set_wps_led_Timer ( unsigned int blinking_latency )
{
	del_timer ( &wps_led_blinking_timer );

	/* Initializing the timer */
	wps_led_blinking_timer.function = wps_led_blinking_timeout;
	wps_led_blinking_timer.data = NULL;
	wps_blinking_latency = blinking_latency;
	wps_led_blinking_timer.expires = jiffies + wps_blinking_latency;

	add_timer ( &wps_led_blinking_timer );

	return	0;
}

static int wsc_led_on ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_status_blinking_type=0;
//	led_on ( WSC_GLED_GPIO );
	///led_on ( WSC_OLED_GPIO );
	led_on ( WSC_OLED_GPIO );
	return	0;
}

static int wsc_led_inprogress ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_led_blinking_flag=0;
	wps_status_blinking_type=1;
//	led_on ( WSC_GLED_GPIO );
	///led_off ( WSC_OLED_GPIO );
	led_on ( WSC_OLED_GPIO );
	Set_wps_led_Timer ( Keep_1_10_second );
	return	0;
}

static int wsc_led_error ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_led_blinking_flag=0;
	wps_status_blinking_type=2;
//	led_off ( WSC_GLED_GPIO );
	///led_on ( WSC_OLED_GPIO );
	led_off( WSC_OLED_GPIO );
	Set_wps_led_Timer ( Keep_1_10_second*300 );
	return	0;
}

static int wsc_led_overlap ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_led_blinking_flag=1;
	wps_status_blinking_type=3;
	Set_wps_led_Timer ( Keep_1_10_second );
	wps_led_blinking_flag=1;
	return	0;
}

static int wsc_led_success ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_led_blinking_flag=0;
	wps_status_blinking_type=4;
//	led_on ( WSC_GLED_GPIO );
	///led_off ( WSC_OLED_GPIO );
	led_on ( WSC_OLED_GPIO );
	Set_wps_led_Timer ( Keep_1_second*300 );
	return	0;
}

static int wsc_led_off ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	wps_led_blinking_flag=0;
	del_timer ( &wps_led_blinking_timer );
//	led_off ( WSC_GLED_GPIO );
	///led_off ( WSC_OLED_GPIO );
	led_on ( WSC_OLED_GPIO );
	return	0;
}

static int __init	led_ctrl_wsc_init ( void )
{
	/* Register /proc/led_ctrl_wsc/ */
	led_ctrl_wsc_dir = proc_mkdir ( led_ctrl_wsc_folder_name , NULL );
	if ( NULL == led_ctrl_wsc_dir )
	{
		remove_proc_entry ( led_ctrl_wsc_folder_name , NULL );
		printk ( "%s - proc_mkdir ... failed\n" , __FUNCTION__ );
		return	-12;
	}
	else if ( NULL != led_ctrl_wsc_dir )
	{
		unsigned char		i = 0;

		while ( i < 255 )
		{
			if ( NULL == WPS_GPIOTbl[i].file_handler )
			{
				break;
			}
			else
			{
				entry = create_proc_read_entry ( WPS_GPIOTbl[i].file_name , WPS_GPIOTbl[i].mode ,
													  led_ctrl_wsc_dir , WPS_GPIOTbl[i].file_handler , NULL );
				if ( NULL == entry )
				{
					remove_proc_entry ( led_ctrl_wsc_folder_name , NULL );
					printk ( "%s - create_proc_read_entry ( %s ) ... failed\n" , __FUNCTION__ , WPS_GPIOTbl[i].file_name );
					return	-12;
				}
			}
			i++;
		}	/* end of while ( i < 255 ) */
	}	/* end of ( NULL != led_ctrl_wsc_dir ) */
	
		
  /* configure gpio as outputs */
//  ar7100_gpio_config_output (WSC_GLED_GPIO); 
  ar7100_gpio_config_output (WSC_OLED_GPIO); 

  /* switch off the led */
//  led_off(WSC_GLED_GPIO);
  ///led_off(WSC_OLED_GPIO);
  led_on(WSC_OLED_GPIO);

	init_timer ( &wps_led_blinking_timer );

	printk ( "Initialziing the WSC LED control module ... successful\n");
	return	0;
}

static void __exit	led_ctrl_wsc_exit ( void )
{
	unsigned char		i = 0;

	/* Remove all entries in /proc/ releated with LED control that created by us */
	while ( i < 255 )
	{
		if ( NULL == WPS_GPIOTbl[i].file_handler )
		{
			break;
		} else
		{
			/* remove all the entires named as "WPS_GPIOTbl[i].file_name" */
			remove_proc_entry ( WPS_GPIOTbl[i].file_name , NULL );
		}
		i++;
	}	/* end of while ( i < 255 ) */

	/* remove the folder named as "led_ctrl_wsc_folder_name" */
	remove_proc_entry ( led_ctrl_wsc_folder_name , NULL );

	printk ( "remove the WSC LED control module ... successful\n");
	return;
}

module_init ( led_ctrl_wsc_init );
module_exit ( led_ctrl_wsc_exit );

MODULE_AUTHOR ( "Gemtek Communications, Inc." );
MODULE_DESCRIPTION ( "Support for WPS LED Ctrl" );

#ifdef		MODULE_LICENSE
MODULE_LICENSE ( "Gemtek-SWD" );
#endif
