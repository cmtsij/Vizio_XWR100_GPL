/*
 * This is a module which is used for logging packets.
 */
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/spinlock.h>
#include <net/icmp.h>
#include <net/udp.h>
#include <net/tcp.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#include <net/route.h>
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

#include <linux/proc_fs.h>

/*	For putting processes to sleep and waking them up	*/
#include <linux/sched.h>
#include <linux/wrapper.h>

#define CHECK_TIMER				60000			/*	60 second  */  

/* Use lock to serialize, so printks don't overlap */
static spinlock_t trigger_lock = SPIN_LOCK_UNLOCKED;
static int timer_flag = 0;
static unsigned long expired_time = 0;
static struct timer_list	trigger_timer;

DECLARE_WAIT_QUEUE_HEAD(OnDemandWaitQ);

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

static int read_proc_DialOnDemand(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	interruptible_sleep_on(&OnDemandWaitQ);
	return len;
}

static int write_proc_DialOnDemand(struct file *file, const char *buffer, unsigned long count, void *data)
{
	int len = 0;
	char buf[16];
		
	if(count >= 15)
		len = 15;
	else
		len = count;

	if(copy_from_user(buf, buffer, len))
	{
		printk(KERN_EMERG "write_proc_DialOnDemand : fail.\n");		
		return -EFAULT;
	}
	spin_lock_bh(&trigger_lock);
	expired_time = 0;	
	spin_unlock_bh(&trigger_lock);
	printk(KERN_EMERG "write_proc_DialOnDemand = %s\n", buf);
	return len;
}  

static void trigger_monitor(unsigned long d)
{
	if (expired_time!=0 && expired_time <= jiffies + 1000)
	{
		// printk("<2>ipt_trigger : Timeout, expired_time = %ld, jiffies = %ld\n", expired_time, jiffies);
		/* 
			FixME : I should use kernel API 'call_usermodehelper' to ask user-space program 
							to set nvram parameter
		*/
		spin_lock_bh(&trigger_lock);
		expired_time = 0;	
		spin_unlock_bh(&trigger_lock);
		nvram_set("wan_release", "1");
		
	}
	trigger_timer.expires  = jiffies + (CHECK_TIMER * HZ / 1000);
 	add_timer(&trigger_timer);	
// 	printk("<2>ipt_trigger : Register trigger_timer at %ld\n", jiffies);
}

static unsigned int
ipt_trigger_target(struct sk_buff **pskb,
	       unsigned int hooknum,
	       const struct net_device *in,
	       const struct net_device *out,
	       const void *targinfo,
	       void *userinfo)
{
	unsigned int timer = 0;
	const struct ipt_trigger_info *triggerinfo = targinfo;
	timer = triggerinfo->timer;
	
//	printk("<2>ipt_trigger : On Demand Timer = %ld\n", timer);
	spin_lock_bh(&trigger_lock);
	expired_time = jiffies + (timer * CHECK_TIMER * HZ / 1000);
	spin_unlock_bh(&trigger_lock);
	wake_up_interruptible(&OnDemandWaitQ);
//	printk("<2>ipt_trigger : expired_time = %ld, register time = %ld\n", expired_time, jiffies);
	return IPT_CONTINUE;
}

static int ipt_trigger_checkentry(const char *tablename,
			      const struct ipt_entry *e,
			      void *targinfo,
			      unsigned int targinfosize,
			      unsigned int hook_mask)
{
	const struct ipt_trigger_info *triggerinfo = targinfo;

	if (targinfosize != IPT_ALIGN(sizeof(struct ipt_trigger_info))) {
		return 0;
	}

	if (triggerinfo->timer <= 0 || triggerinfo->timer > 9999 ) {
		return 0;
	}

	if (triggerinfo->process_name[sizeof(triggerinfo->process_name)-1] != '\0') {
		return 0;
	}
	
	return 1;
}

/*static struct ipt_target ipt_trigger_reg
= { { NULL, NULL }, "TRIGGER", ipt_trigger_target, ipt_trigger_checkentry, NULL, 
    THIS_MODULE };*/

static struct ipt_target ipt_process_reg = {
	.name		= "TRIGGER",
	.target		= ipt_trigger_target,
	.checkentry	= ipt_trigger_checkentry,
	.me		= THIS_MODULE,
};

static int __init init(void)
{
	if (ipt_register_target(&ipt_trigger_reg))
		return -EINVAL;
		
	if(create_proc_read_write_entry("DialOnDemand",0,NULL, read_proc_DialOnDemand, write_proc_DialOnDemand, NULL) == NULL)
		printk("<2>ipt_trigger : register proc fail\n");
	else
		printk("<2>ipt_trigger : register proc success\n");
		
	memset(&trigger_timer, 0, sizeof(trigger_timer));
	trigger_timer.function = trigger_monitor;
	trigger_timer.expires  = jiffies + (CHECK_TIMER * HZ / 1000);
 	add_timer(&trigger_timer);
	return 0;
}

static void __exit fini(void)
{
	ipt_unregister_target(&ipt_trigger_reg);
	del_timer(&trigger_timer);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
