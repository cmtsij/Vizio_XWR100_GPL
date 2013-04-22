#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/init.h>
#include "ag7100.h"
#include "athrs26_phy.h"

#ifdef _USE_ATHRS26
#define IS_LINK_ALIVE athrs26_phy_is_link_alive
#else
#define IS_LINK_ALIVE athrs16_phy_is_link_alive
#endif

/* Gemtek Add WAN Port Detect */
#define linkstate_file0	"link_state0"
#define linkstate_file1	"link_state1"
#define linkstate_file2	"link_state2"
#define linkstate_file3	"link_state3"
#define linkstate_file4	"link_state4"
int Linkstate=0; // 0:link down , 1 :link up
#define UINT32 u32
#define PORT_BASE_OFFSET  0x0108
#define PRIORITY_CTL_OFFSET  0x0110
#define PRIORITY_CTL_E_OFFSET         0x0100
#define PRIORITY_CTL_PORT_PRI_EN_BOFFSET 19
struct QoS_data_d {
	char QoS_proc_NAME[32];
	int QoS_port_NUM;
}QoS_data[4];

static int port0_link_detect_function(char *buf, char **start, off_t off,
				 int count, int *eof, void *data)
{
	if (IS_LINK_ALIVE(0)) {
		Linkstate = 1;
	} else {
		Linkstate = 0;
	}
	return sprintf(buf, "%d", Linkstate);
}
static int port1_link_detect_function(char *buf, char **start, off_t off,
				 int count, int *eof, void *data)
{
	if (IS_LINK_ALIVE(1)) {
		Linkstate = 1;
	} else {
		Linkstate = 0;
	}
	return sprintf(buf, "%d", Linkstate);
}
static int port2_link_detect_function(char *buf, char **start, off_t off,
				 int count, int *eof, void *data)
{
	if (IS_LINK_ALIVE(2)) {
		Linkstate = 1;
	} else {
		Linkstate = 0;
	}
	sprintf(buf, "%d", Linkstate);
	return sprintf(buf, "%d", Linkstate);
}
static int port3_link_detect_function(char *buf, char **start, off_t off,
				 int count, int *eof, void *data)
{
	if (IS_LINK_ALIVE(3)) {
		Linkstate = 1;
	} else {
		Linkstate = 0;
	}
	return sprintf(buf, "%d", Linkstate);
}
static int port4_link_detect_function(char *buf, char **start, off_t off,
				 int count, int *eof, void *data)
{
	if (IS_LINK_ALIVE(4)) {
		Linkstate = 1;
	} else {
		Linkstate = 0;
	}
	return sprintf(buf, "%d", Linkstate);
}
static int switch_reset_function(char *buf, char **start, off_t off,
				 int count, int *eof, void *data)
{
	athrs26_phy_setup(0);
	athrs26_phy_setup(1);
	athrs26_phy_setup(2);
	athrs26_phy_setup(3);
	return sprintf(buf, "Switch resetting...done\n");
}
int QoS_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	
	char buf[10] = "\0";
	int len = 0;
	struct QoS_data_d *qos = (struct QoS_data_d *)data;
	UINT32 reg_addr;
	UINT32 pre_reg_data, post_reg_date;
	long qos_level=0;
	len = count;	
	sscanf(buffer,"%ld",&qos_level);	//QoS Level :1~4 and 0 is disable QoS
	if(qos_level<0 || qos_level >4)
	{
		printk("%s->%s(%d),Error QoS Level : %ld\n", __FILE__, __FUNCTION__, __LINE__, qos_level);
		return len;
	}
	if(qos_level == 0)
	{
		reg_addr=PRIORITY_CTL_OFFSET + PRIORITY_CTL_E_OFFSET * (qos->QoS_port_NUM + 2);
		pre_reg_data = ar7100_reg_rd(reg_addr);
		post_reg_date = pre_reg_data&0x0;	//use 0x0 to reset bit[0:31]
		printk("Address:0x%08X,pre_reg_data => before : 0x%08X,after : 0x%08X\n", reg_addr, pre_reg_data, post_reg_date );
		ar7100_reg_wr(reg_addr, post_reg_date);		
	}
	else
	{
		qos_level--;	//because QoS level starts from 0
		/*
			Because of ING_PORT_PRI is at 0x0108 bit[28:29] and PORT_PRI_SEL is at 0x0110 bit[1:0].
			We want to set QoS level at those bits and use "&" to reset and use "|" to set it.
		*/
		//PRIORITY_CTL_OFFSET:0x0110
		reg_addr=PRIORITY_CTL_OFFSET + PRIORITY_CTL_E_OFFSET * (qos->QoS_port_NUM + 2);
		pre_reg_data = ar7100_reg_rd(reg_addr);
		post_reg_date = ((pre_reg_data&0x0)|qos_level|(1UL <<PRIORITY_CTL_PORT_PRI_EN_BOFFSET));	//use 0x0 to reset bit[0:31] then set bits[0:1] and bit[19]
		printk("Address:0x%08X,pre_reg_data => before : 0x%08X,after : 0x%08X\n", reg_addr, pre_reg_data, post_reg_date );
		ar7100_reg_wr(reg_addr, post_reg_date);
		//PORT_BASE_OFFSET:0x0108
		reg_addr=PORT_BASE_OFFSET + PRIORITY_CTL_E_OFFSET * (qos->QoS_port_NUM + 2);	//lan port is start from 2 to 6
		pre_reg_data = ar7100_reg_rd(reg_addr);
		post_reg_date = (pre_reg_data&0xCFFFFFFF)|(qos_level<<28);
		printk("Address:0x%08X,pre_reg_data => before : 0x%08X,after : 0x%08X\n", reg_addr, pre_reg_data, post_reg_date );	
		ar7100_reg_wr(reg_addr, post_reg_date);			
	}
	return len;
}  
          
int QoS_read_proc(char *buf, char **start, off_t off, int count, int *eof, void *data)
{
	struct QoS_data_d *qos = (struct QoS_data_d *)data;
	UINT32 reg_addr=PRIORITY_CTL_OFFSET + PRIORITY_CTL_E_OFFSET * (qos->QoS_port_NUM + 2);;
	UINT32 reg_data;
	reg_data = ar7100_reg_rd(reg_addr);
	if(reg_data & (1UL <<PRIORITY_CTL_PORT_PRI_EN_BOFFSET))
		printk("PORT_PRI_EN is enable\n");
	else
		printk("PORT_PRI_EN is disable!!\n");
	printk("reg_data = Address:0x%08X => 0x%08X and QoS level is %d\n", reg_addr, reg_data, (int)(reg_data&0x3));
	sprintf(buf,"%d", (int)(reg_data&0x3));
	return 2;
}

static struct proc_dir_entry *gemtek_entry = NULL;
static struct proc_dir_entry *link_status = NULL;
static struct proc_dir_entry *switch_reset = NULL;

void ag7100_proc_fs_init(void)
{	
  if (gemtek_entry != NULL) {
      printk ("Already have a proc entry for /proc/gemtek!\n");
      return -ENOENT;
  }
  gemtek_entry = proc_mkdir("gemtek", NULL);
  if (!gemtek_entry)
  	return -ENOENT;

  link_status = create_proc_entry (linkstate_file0, 0644, gemtek_entry);
  if (!link_status)
  	return -ENOENT;
  link_status->write_proc = NULL;
  link_status->read_proc = port0_link_detect_function;
  
  link_status = create_proc_entry (linkstate_file1, 0644, gemtek_entry);
  if (!link_status)
  	return -ENOENT;
  link_status->write_proc = NULL;
  link_status->read_proc = port1_link_detect_function;
  
  link_status = create_proc_entry (linkstate_file2, 0644, gemtek_entry);
  if (!link_status)
  	return -ENOENT;
  link_status->write_proc = NULL;
  link_status->read_proc = port2_link_detect_function;
  
  link_status = create_proc_entry (linkstate_file3, 0644, gemtek_entry);
  if (!link_status)
  	return -ENOENT;
  link_status->write_proc = NULL;
  link_status->read_proc = port3_link_detect_function;
  
  link_status = create_proc_entry (linkstate_file4, 0644, gemtek_entry);
  if (!link_status)
  	return -ENOENT;
  link_status->write_proc = NULL;
  link_status->read_proc = port4_link_detect_function;

  switch_reset = create_proc_entry ("switch_reset", 0644, gemtek_entry);
  if (!switch_reset)
  	return -ENOENT;
  switch_reset->write_proc = NULL;
  switch_reset->read_proc = switch_reset_function;
  
}

void ag7100_proc_fs_exit (void)
{
	
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

void ag7100_qos_proc_fs_init (void)
{
	int i=0;	
	for(i = 0;i < 4; i++)
	{
		sprintf(QoS_data[i].QoS_proc_NAME,"port%d_qos_state", i);
		QoS_data[i].QoS_port_NUM = i;
		if(create_proc_read_write_entry(QoS_data[i].QoS_proc_NAME,0,gemtek_entry,QoS_read_proc,QoS_write_proc,(void *)&QoS_data[i]) == NULL)
		{
			printk("%s->%s(%d) : fail\n", __FILE__, __FUNCTION__, __LINE__ );
		}
		else
		{
			printk("%s->%s(%d) : fail\n", __FILE__, __FUNCTION__, __LINE__ );
		}
	}		
}

void ag7100_qos_proc_exit (void)
{
	int i=0;	
	for(i = 0;i < 4; i++)
	{
		sprintf(QoS_data[i].QoS_proc_NAME,"port%d_qos_state", i);
		remove_proc_entry(QoS_data[i].QoS_proc_NAME,NULL);
	}
}

