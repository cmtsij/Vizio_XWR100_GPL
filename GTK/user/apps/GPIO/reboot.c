/*
 *   kernel reboot interface for user application
 *
 *			- TJ Lin, 2007, Oct, 30
 */


#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

//#include <typedefs.h>
//#include <sbutils.h>

// +++ Gemtek
#include <linux/poll.h>

#include <linux/proc_fs.h>

#define RESET_TIME	10
/* GLOBAL */

extern void ar7100_restart(char *);
static struct	timer_list timer_reboot;


static void reboot(void)
{
			 del_timer(&timer_reboot);
	     ar7100_restart(NULL);
}

static int ar7100_reboot_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	//int len = 0;
	ulong	k=0;
	init_timer(&timer_reboot);
	timer_reboot.function = reboot;
	timer_reboot.data = k;
 	mod_timer(&timer_reboot, jiffies + (jiffies + RESET_TIME*HZ));
	
	//len = sprintf(buf,"wait %d seconds before reboot" , RESET_TIME);
	
	return sprintf(buf,"wait %d seconds before reboot\n" , RESET_TIME);
}

static int __init reboot_init(void)
{
	
	create_proc_read_entry("reboot",0,NULL,ar7100_reboot_proc,NULL);

	return 0;
}


static void reboot_cleanup(void)
{
	del_timer(&timer_reboot);
}


module_init(reboot_init);
module_exit(reboot_cleanup);


