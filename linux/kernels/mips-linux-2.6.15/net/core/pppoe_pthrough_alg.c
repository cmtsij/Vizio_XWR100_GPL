#include	<linux/netdevice.h>
#include	<linux/proc_fs.h>

extern struct net_device		*dev_get_by_name ( const char *name );
extern void	dev_set_promiscuity ( struct net_device *dev , int inc );
extern int		dev_queue_xmit ( struct sk_buff *skb );
int	pppoe_pt_enable = 0;
unsigned char		pppoe_pt_landev [ IFNAMSIZ ] , pppoe_pt_wandev [ IFNAMSIZ ];
#define	ETH_TYPE_PPPOE_DISCOVERY		0x8863
#define	ETH_TYPE_PPPOE_SESSION			0x8864
#define	PTABLE_SIZE		16
static int	pthrough_idx = 0;
static unsigned char	pthrough_table [ PTABLE_SIZE ] [ ETH_ALEN ];
#define isCHAR(x) ((x >= 'a') && (x <= 'z')) ? 1:((x >= '0') && (x <= '9')) ? 1:((x >= 'A') && (x <= 'Z')) ? 1:0
static struct	proc_dir_entry		*pppoe = NULL;

/*	return 0 : This Pkt is not processed by PPPoE_Passthru_ALG.
	return 1 : This Pkt has been processed ( forwarding to the certain i/f ) by PPPoE_Passthru_ALG. */
int		pppoe_pthrough_alg ( struct sk_buff *skb )
{
	unsigned short		proto = 0;
	unsigned char		*smac = NULL , *dmac = NULL;
	int					i = 0;

	if ( 0 == pppoe_pt_enable ) {
		return	0;
	}

	proto = eth_hdr(skb)->h_proto;
	smac = eth_hdr(skb)->h_source;
	dmac = eth_hdr(skb)->h_dest;

	if (	( ETH_TYPE_PPPOE_SESSION == proto ) ||
		( ETH_TYPE_PPPOE_DISCOVERY == proto ) ) {
		if ( !strcmp ( skb->dev->name , pppoe_pt_landev ) ) {
			for ( i = 0 ; i < pthrough_idx ; i++ ) {
				if ( !memcmp ( pthrough_table [ i ] , smac , ETH_ALEN ) ) {
					break;
				}
			}

			if ( i == pthrough_idx ) {
				memcpy ( pthrough_table [ i ] , smac , ETH_ALEN );
				pthrough_idx++;
				if ( pthrough_idx >= PTABLE_SIZE ) {
					printk ( "PPPoE pthrough table is full.\n" );
					pthrough_idx--;
				}
			}

			skb->dev = dev_get_by_name ( pppoe_pt_wandev );
			skb_push ( skb , ETH_HLEN );
			dev_queue_xmit ( skb );
			return	1;
		}

		if ( !strcmp ( skb->dev->name , pppoe_pt_wandev ) ) {
			for ( i = 0 ; i < pthrough_idx ; i++ ) {
				if ( !memcmp ( pthrough_table [ i ] , dmac , ETH_ALEN ) ) {
					skb->dev = dev_get_by_name ( pppoe_pt_landev );
					skb_push ( skb , ETH_HLEN );
					dev_queue_xmit ( skb );
					return	1;
				}
			}
		}
	}

	return 0;
}

int		proc_pppoe_read ( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	int		ret = 0;

	if ( 1 == pppoe_pt_enable ) {
		ret = sprintf ( page , "PPPoE passthru is for %s <-> %s\n" , pppoe_pt_landev , pppoe_pt_wandev );
	} else {
		ret = sprintf ( page , "PPPoE passthru is not enabled\n" );
	}

	return	ret;
}

int		proc_pppoe_write ( struct file *file , const char *buffer , unsigned long count , void *data )
{
	char		*pt = NULL;
	struct net_device		*dev = NULL;

	if ( 1 == pppoe_pt_enable ) {
		pppoe_pt_enable = 0;
		if ( ( dev = dev_get_by_name ( pppoe_pt_landev ) ) ) {
			dev_set_promiscuity ( dev , -1 );
		}
		if ( ( dev = dev_get_by_name ( pppoe_pt_wandev ) ) ) {
			dev_set_promiscuity ( dev , -1 );
		}
	}

	/* we expect that buffer contain format of "landev_name,wandev_name" */
	memset ( pppoe_pt_landev , '\0' , sizeof ( pppoe_pt_landev ) );
	memset ( pppoe_pt_wandev , '\0' , sizeof ( pppoe_pt_wandev ) );

	for ( pt = pppoe_pt_landev ; ( *buffer && ( *buffer != ',' ) ) ; buffer++ ) {
		if ( ( *buffer != ' ' ) && isCHAR ( *buffer ) ) {
			*pt = *buffer;
			pt++;
		}
	}

	if ( !( *buffer ) ) {
		goto	ppw_failed;
	}

	for ( pt = pppoe_pt_wandev , buffer++ ; *buffer ; buffer++ ) {
		if ( ( *buffer != ' ' ) && isCHAR ( *buffer ) ) {
			*pt = *buffer;
			pt++;
		}
	}

	if ( !( dev = dev_get_by_name ( pppoe_pt_landev ) ) ) {
		printk ( "PPPoE Passthru : no such landev ( %s )\n" , pppoe_pt_landev );
		goto	ppw_failed;
	} else {
		dev_set_promiscuity ( dev , 1 );
	}

	if ( !( dev = dev_get_by_name ( pppoe_pt_wandev ) ) ) {
		printk ( "PPPoE Passthru : no such wandev ( %s )\n" , pppoe_pt_wandev );
		goto	ppw_failed;
	} else {
		dev_set_promiscuity ( dev , 1 );
	}

	pppoe_pt_enable = 1;
	printk ( "GTK : starting up PPPoE_Passthru ( %s <-> %s ) ... successfully\n" , pppoe_pt_landev , pppoe_pt_wandev );
	return	count;

ppw_failed:
	pppoe_pt_enable = 0;
	memset ( pppoe_pt_landev , '\0' , sizeof ( pppoe_pt_landev ) );
	memset ( pppoe_pt_wandev , '\0' , sizeof ( pppoe_pt_wandev ) );
	return	count;
}

int		pppoe_pthrough_system_configuration ( void )
{
	/* reset all buffers */
	memset ( pthrough_table , '\0' , ( PTABLE_SIZE * ETH_ALEN ) );
	memset ( pppoe_pt_landev , '\0' , IFNAMSIZ );
	memset ( pppoe_pt_wandev , '\0' , IFNAMSIZ );
	pthrough_idx = 0;

	if ( NULL == pppoe ) {
		if ( NULL == ( pppoe = create_proc_entry ( "pppoe" , 0644 , NULL ) ) ) {
			printk ( "PPPoE Passthru : create_proc_entry ... failed\n" );
		} else {
			pppoe->read_proc = proc_pppoe_read;
			pppoe->write_proc = proc_pppoe_write;
			return	0;
		}
	} else {
		printk ( "PPPoE Passthru : proc_entry named pppoe has been created one moment ago ... failed\n" );
	}

	return	-1;
}

#if		0
void		pthrough_rmove_proc_entry ( void )
{
	remove_proc_entry ( "pppoe" , NULL );
}
#endif