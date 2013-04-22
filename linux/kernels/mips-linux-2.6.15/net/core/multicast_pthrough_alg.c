#include <linux/netdevice.h>
#include <linux/proc_fs.h>

#include <linux/skbuff.h>
#include <linux/ip.h>

extern struct net_device	*dev_get_by_name ( const char *name );
extern void	dev_set_promiscuity ( struct net_device *dev , int inc );
extern int		dev_queue_xmit ( struct sk_buff *skb );
int				multicast_pt_enable = 0; /* 0 : disabled , 1 enabled */
unsigned char	multicast_alg_landev [ IFNAMSIZ ] , multicast_alg_wandev [ IFNAMSIZ ];
static struct	proc_dir_entry		*multicast_bridging = NULL;

#define SIZE 100

__u32 group_address[SIZE];

int is_subscribed(struct sk_buff *skb) {
	int i;

	for(i = 0; i < SIZE; i ++) {
		if(group_address[i] == skb->nh.iph->daddr) {
			return 1;
		}
	}
	return 0;
}

int igmp_already_exist(__u32 addr) {
	int i;
			
	for(i = 0; i < SIZE; i ++) {
		if(group_address[i] == addr) {
			return 1;
		}
	}
	return 0;
}

void igmp_add(__u32 addr) {
	int i;

	for(i = 0; i < SIZE; i ++) {
		if(group_address[i] == 0) {
			group_address[i] = addr;
			break;
		}
	}	
}

void igmp_remove(__u32 addr) {
	int i;

	for(i = 0; i < SIZE; i ++) {
		if(group_address[i] == addr) {
			group_address[i] = 0;
			break;
		}
	}
}

/*	return 1 -> we have processed this pkt.
 	return 0 -> we don't process this pkt.	*/
int		multicast_bridging_alg ( struct sk_buff *skb )
{
	unsigned short		proto = 0x0000;
	unsigned char		*smac = NULL , *dmac = NULL;
	unsigned char			*dest = NULL;

	if ( 0 == multicast_pt_enable ) {
		return	0;
	}

	proto = eth_hdr(skb)->h_proto;
	smac = eth_hdr(skb)->h_source;
	dmac = eth_hdr(skb)->h_dest;

	skb->nh.raw = skb->data;


	if( strcmp(skb->dev->name, multicast_alg_landev) == 0 && proto == 0x0800 && skb->nh.iph->protocol == 0x02) {  // IGMP packet
		skb->h.raw = skb->data + skb->nh.iph->ihl * 4;
		if(skb->h.igmph->type == 0x16) {  // Report message
			if(igmp_already_exist(skb->h.igmph->group) == 0) {
				igmp_add(skb->h.igmph->group);
			}
		} else if(skb->h.igmph->type == 0x17) {  // Leave message
			igmp_remove(skb->h.igmph->group);
		} else if(skb->h.igmph->type == 0x22) {  // IGMPv3 Report message
			__u32 *igmpv3_group_address = skb->h.raw + 12;
			__u8 *record_type = skb->h.raw + 8;
		
			if(*record_type == 0x03) { // Change to include mode  -- to leave
				igmp_remove(*igmpv3_group_address);				
			} else if(*record_type == 0x04) { // Change to exclude mode
				if(igmp_already_exist(*igmpv3_group_address) == 0) {
					igmp_add(*igmpv3_group_address);
				}			
			}
		}
	}

	if ( !strcmp ( skb->dev->name , multicast_alg_wandev ) ) {
		if	( NULL != ( dest = eth_hdr(skb)->h_dest ) ) {
			if ( ( 0x01 == dest[0] ) ) {
				if( is_subscribed(skb) ) {
					skb->dev = dev_get_by_name ( multicast_alg_landev );
					skb_push ( skb , ETH_HLEN );
					dev_queue_xmit ( skb );
					return	1;
				}
			}
		}
	}
	return	0;
}

int		proc_multicast_read ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	int		ret = 0;

	if ( 1 == multicast_pt_enable ) {
		ret = sprintf ( page , "Multicast bridging is for %s <-> %s\n" , multicast_alg_landev , multicast_alg_wandev );
	} else {
		ret = sprintf ( page , "Multicast bridging is not enabled\n" );
	}

	return	ret;
}

#define isCHAR(x) ((x >= 'a') && (x <= 'z')) ? 1:((x >= '0') && (x <= '9')) ? 1:((x >= 'A') && (x <= 'Z')) ? 1:0

int		proc_multicast_write ( struct file *file , const char *buffer , unsigned long count , void *data )
{
	unsigned char		*pt = NULL;
	struct net_device	*dev = NULL;

	if ( 1 == multicast_pt_enable ) {
		multicast_pt_enable = 0;
		if ( ( dev = dev_get_by_name ( multicast_alg_landev ) ) ) {
			dev_set_promiscuity ( dev , -1 );
		}
		if ( ( dev = dev_get_by_name ( multicast_alg_wandev ) ) ) {
			dev_set_promiscuity ( dev , -1 );
		}
	}

	memset ( multicast_alg_landev , '\0' , sizeof ( multicast_alg_landev ) );
	memset ( multicast_alg_wandev , '\0' , sizeof ( multicast_alg_wandev ) );

	for ( pt = multicast_alg_landev ; *buffer && ( *buffer != ',' ) ; buffer++ ) {
		if ( ( *buffer != ' ' ) && isCHAR ( *buffer ) ) {
			*pt = *buffer;
			pt++;
		}
	}

	if ( !( *buffer ) ) {
		goto	ppw_failed;
	}

	for ( pt = multicast_alg_wandev , buffer++ ; *buffer ; buffer++ ) {
		if ( ( *buffer != ' ' ) && isCHAR ( *buffer ) ) {
			*pt = *buffer;
			pt++;
		}
	}

	if ( !( dev = dev_get_by_name ( multicast_alg_landev ) ) ) {
		goto	ppw_failed;
	} else {
		dev_set_promiscuity ( dev , 1 );
	}

	if ( !( dev = dev_get_by_name ( multicast_alg_wandev ) ) ) {
		goto	ppw_failed;
	} else {
		dev_set_promiscuity ( dev , 1 );
	}

	multicast_pt_enable = 1;
	printk ( "Multicast bridging is for %s <-> %s\n" , multicast_alg_landev , multicast_alg_wandev );
	return	count;

ppw_failed:
	multicast_pt_enable = 0;
	memset ( multicast_alg_landev , '\0' , sizeof ( multicast_alg_landev ) );
	memset ( multicast_alg_wandev , '\0' , sizeof ( multicast_alg_wandev ) );
	return	count;
}

int		multicast_bridging_system_configuration ( void )
{
	multicast_pt_enable = 0;
	memset ( multicast_alg_landev , '\0' , IFNAMSIZ );
	memset ( multicast_alg_wandev , '\0' , IFNAMSIZ );

	memset(group_address, '\0', sizeof(group_address));

	
	if ( NULL == multicast_bridging ) {
		multicast_bridging = create_proc_entry ( "multicast_bridging" , 0644 , NULL );
		if ( NULL == multicast_bridging ) {
			printk ( "Multicast bridging : create_proc_entry ... failed\n" );
		} else {
			multicast_bridging->read_proc = proc_multicast_read;
			multicast_bridging->write_proc = proc_multicast_write;
			return	0;
		}
	} else {
		printk ( "Multicast bridging : proc_entry named multicast_bridging has been created one moment ago.\n" );
	}

	return	-1;
}
