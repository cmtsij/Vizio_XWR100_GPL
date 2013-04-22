/*
 * This is a module which is used for trigger user applications.
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
#include <linux/netfilter_ipv4/ipt_PROCESS.h>

#include <linux/proc_fs.h>

/*	For putting processes to sleep and waking them up	*/
#include <linux/sched.h>
static char procfs_buffer[24];
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

static int read_proc_CallProcess(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	int ret;
	
	//printk(KERN_INFO "procfile_read (/proc/CallProcess) called\n");
	
	if (off > 0) {
		/* we have finished to read, return 0 */
		ret  = 0;
	} else {
		/* fill the buffer, return the buffer size */
		memcpy(buf, procfs_buffer, strlen(procfs_buffer));
		ret = strlen(procfs_buffer);
		strcpy(procfs_buffer, "none");
	}

	return ret;
}

static int write_proc_CallProcess(struct file *file, const char *buffer, unsigned long count, void *data)
{
	return 0;
}  

static unsigned int
ipt_process_target(struct sk_buff **pskb,
	       unsigned int hooknum,
	       const struct net_device *in,
	       const struct net_device *out,
	       const void *targinfo,
	       void *userinfo)
{
	//printLog("Marmot enter ipt_process_target\n");
	const struct ipt_process_info *processinfo = targinfo;
	//procfs_buffer = processinfo->path;
	strcpy(procfs_buffer, processinfo->path);
	//	nvram_set("trigger_path", processinfo->path);
	//printLog("Marmot processinfo->path is %s\n", processinfo->path);
	//argv [0] = processinfo->path;
	//argv [1] = "&";
	//argv [2] = 0;
	
	//envp [0] = "HOME=/";
	//envp [1] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
	
	//value = call_usermodehelper (argv [0], argv, envp);
	
	//if (value != 0) {
	//	printk("Marmot FAIL\n");
	//	return 0;
	//}
	return IPT_CONTINUE;
}

static int ipt_process_checkentry(const char *tablename,
			      const struct ipt_entry *e,
			      void *targinfo,
			      unsigned int targinfosize,
			      unsigned int hook_mask)
{
	
	//printLog("Marmot enter ipt_process_checkentry\n");
	return 1;
}

static struct ipt_target ipt_process_reg = {
	.name		= "PROCESS",
	.target		= ipt_process_target,
	.checkentry	= ipt_process_checkentry,
	.me		= THIS_MODULE,
};

static int __init init(void)
{
	if (ipt_register_target(&ipt_process_reg))
		return -EINVAL;
		
		
	if(create_proc_read_write_entry("CallProcess",0,NULL, read_proc_CallProcess, write_proc_CallProcess, NULL) == NULL)
		printk("<2>ipt_trigger : register proc fail\n");
	else
		printk("<2>ipt_trigger : register proc success\n");
		
	return 0;
}

static void __exit fini(void)
{
	remove_proc_entry ( "CallProcess" , NULL );
	ipt_unregister_target(&ipt_process_reg);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
