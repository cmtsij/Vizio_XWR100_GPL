#include <linux/netdevice.h>
#include <linux/proc_fs.h>

extern struct net_device	*dev_get_by_name ( const char *name );
extern void	dev_set_promiscuity ( struct net_device *dev , int inc );
extern int		dev_queue_xmit ( struct sk_buff *skb );
int				ipv6_pt_enable = 0; /* 0 : disabled , 1 enabled */
unsigned char	ipv6_alg_landev [ IFNAMSIZ ] , ipv6_alg_wandev [ IFNAMSIZ ];
#ifdef		_Little_Endian_System_
#define		ETH_TYPE_IPV6		0xdd86
#else
#define		ETH_TYPE_IPV6		0x86dd
#endif		/* end of _Little_Endian_System_ */
static struct	proc_dir_entry		*ipv6_bridging = NULL;

/*	For RaLink system, the wireless driver can't handle IPv6 Multicast packet,
	so this patch is for it. Not a good solution, and need RaLink to fix it.*/
// #define	_FixWirelessCanNotHandleIPv6MultiCast_

#ifdef		_FixWirelessCanNotHandleIPv6MultiCast_
struct net_device		*lan_ethernet_dev = NULL;
#define		_EthernetLANIFName_			"eth0"
unsigned char		PatchFor_WirelessCanNotHandleIPv6MultiCast ( struct sk_buff *skb )
{
	extern unsigned int	_IPv6_Multicast_Passthru_Flag_;
	unsigned char			*dest = NULL;

	if ( NULL != skb ) {
		if	( NULL != ( dest = eth_hdr(skb)->h_dest ) ) {
			if ( ( 0x33 == dest[0] ) && ( 0x33 == dest[1] ) ) {	/* for IPv6 Multicast */
				if ( 0 == _IPv6_Multicast_Passthru_Flag_ ) {	/* wireless i/f is not allowed to pass IPv6 Multicast */
					if ( NULL == lan_ethernet_dev ) {
						lan_ethernet_dev = dev_get_by_name ( _EthernetLANIFName_ );
					}
					skb->dev = lan_ethernet_dev;
					if ( NULL != skb->dev ) {
						skb_push ( skb , ETH_HLEN );
						dev_queue_xmit ( skb );
						return	1;
					}	/* end of ( NULL != ( skb->dev = dev_get_by_name ( _EthernetLANIFName_ ) ) ) */
				}	/* end of ( 0 == _IPv6_Multicast_Passthru_Flag_ ) */
			}	/* end of ( ( 0x33 == dest[0] ) && ( 0x33 == dest[1] ) ) */
		}	/* end of ( NULL != dest ) */
	}	/* end of ( NULL != skb ) */

	return	0;
}
#endif		/* end of _FixWirelessCanNotHandleIPv6MultiCast_ */

/*	return 1 -> we have processed this pkt.
 	return 0 -> we don't process this pkt.	*/
int		ipv6_bridging_alg ( struct sk_buff *skb )
{
	unsigned short		proto = 0x0000;
	unsigned char		*smac = NULL , *dmac = NULL;

	if ( 0 == ipv6_pt_enable ) {
		return	0;
	}

	proto = eth_hdr(skb)->h_proto;
	smac = eth_hdr(skb)->h_source;
	dmac = eth_hdr(skb)->h_dest;

	if ( ETH_TYPE_IPV6 == proto ) {
		if ( !strcmp ( skb->dev->name , ipv6_alg_landev ) ) {
			skb->dev = dev_get_by_name ( ipv6_alg_wandev );
			skb_push ( skb , ETH_HLEN );
			dev_queue_xmit ( skb );
			return	1;
		}

		if ( !strcmp ( skb->dev->name , ipv6_alg_wandev ) ) {
#ifdef		_FixWirelessCanNotHandleIPv6MultiCast_
			if ( 1 == PatchFor_WirelessCanNotHandleIPv6MultiCast ( skb ) ) { return 1; }
#endif		/* end of _FixWirelessCanNotHandleIPv6MultiCast_ */
			skb->dev = dev_get_by_name ( ipv6_alg_landev );
			skb_push ( skb , ETH_HLEN );
			dev_queue_xmit ( skb );
			return	1;
		}
	}

	return	0;
}

int		proc_ipv6_read ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	int		ret = 0;

	if ( 1 == ipv6_pt_enable ) {
		ret = sprintf ( page , "IPv6 bridging is for %s <-> %s\n" , ipv6_alg_landev , ipv6_alg_wandev );
	} else {
		ret = sprintf ( page , "IPv6 bridging is not enabled\n" );
	}

	return	ret;
}

#define isCHAR(x) ((x >= 'a') && (x <= 'z')) ? 1:((x >= '0') && (x <= '9')) ? 1:((x >= 'A') && (x <= 'Z')) ? 1:0

int		proc_ipv6_write ( struct file *file , const char *buffer , unsigned long count , void *data )
{
	unsigned char		*pt = NULL;
	struct net_device	*dev = NULL;

	if ( 1 == ipv6_pt_enable ) {
		ipv6_pt_enable = 0;
		if ( ( dev = dev_get_by_name ( ipv6_alg_landev ) ) ) {
			dev_set_promiscuity ( dev , -1 );
		}
		if ( ( dev = dev_get_by_name ( ipv6_alg_wandev ) ) ) {
			dev_set_promiscuity ( dev , -1 );
		}
	}

	memset ( ipv6_alg_landev , '\0' , sizeof ( ipv6_alg_landev ) );
	memset ( ipv6_alg_wandev , '\0' , sizeof ( ipv6_alg_landev ) );

	for ( pt = ipv6_alg_landev ; *buffer && ( *buffer != ',' ) ; buffer++ ) {
		if ( ( *buffer != ' ' ) && isCHAR ( *buffer ) ) {
			*pt = *buffer;
			pt++;
		}
	}

	if ( !( *buffer ) ) {
		goto	ppw_failed;
	}

	for ( pt = ipv6_alg_wandev , buffer++ ; *buffer ; buffer++ ) {
		if ( ( *buffer != ' ' ) && isCHAR ( *buffer ) ) {
			*pt = *buffer;
			pt++;
		}
	}

	if ( !( dev = dev_get_by_name ( ipv6_alg_landev ) ) ) {
		goto	ppw_failed;
	} else {
		dev_set_promiscuity ( dev , 1 );
	}

	if ( !( dev = dev_get_by_name ( ipv6_alg_wandev ) ) ) {
		goto	ppw_failed;
	} else {
		dev_set_promiscuity ( dev , 1 );
	}

#ifdef		_FixWirelessCanNotHandleIPv6MultiCast_
	lan_ethernet_dev = NULL;
#endif		/* end of _FixWirelessCanNotHandleIPv6MultiCast_ */

	ipv6_pt_enable = 1;
	printk ( "IPv6 bridging is for %s <-> %s\n" , ipv6_alg_landev , ipv6_alg_wandev );
	return	count;

ppw_failed:
	ipv6_pt_enable = 0;
#ifdef		_FixWirelessCanNotHandleIPv6MultiCast_
	lan_ethernet_dev = NULL;
#endif		/* end of _FixWirelessCanNotHandleIPv6MultiCast_ */
	memset ( ipv6_alg_landev , '\0' , sizeof ( ipv6_alg_landev ) );
	memset ( ipv6_alg_wandev , '\0' , sizeof ( ipv6_alg_landev ) );
	return	count;
}

int		ipv6_bridging_system_configuration ( void )
{
	ipv6_pt_enable = 0;
	memset ( ipv6_alg_landev , '\0' , IFNAMSIZ );
	memset ( ipv6_alg_wandev , '\0' , IFNAMSIZ );

	if ( NULL == ipv6_bridging ) {
		ipv6_bridging = create_proc_entry ( "ipv6_bridging" , 0644 , NULL );
		if ( NULL == ipv6_bridging ) {
			printk ( "IPv6 bridging : create_proc_entry ... failed\n" );
		} else {
			ipv6_bridging->read_proc = proc_ipv6_read;
			ipv6_bridging->write_proc = proc_ipv6_write;
			return	0;
		}
	} else {
		printk ( "IPv6 bridging : proc_entry named ipv6_bridging has been created one moment ago.\n" );
	}

	return	-1;
}
