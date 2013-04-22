/***************************************************************
**Gemtek +++ 20081124 ---
**IGMP Snooping(IPv4) function & MLD Snooping(IPv6) function
**Small-database-related functions
**
****************************************************************/
#include <linux/spinlock.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/skbuff.h>
#include <linux/ip.h>

#if 0
#define DEBUG printk
#else
#define DEBUG(format, args...)
#endif

#define		GTK_IGMPMLDTblSize		110 //15 // Number of groups
#define		GTK_IGMPMLDMACMax			32 //8  // Number of members of each group

#define		ETH_ADDR_LEN					6

#define		LANIFNAME		"br0"
//#define		WANIFNAME		"eth1"

char wan_ifname[8];
unsigned long int wan_ipaddr;
unsigned char wan_mac[ETH_ADDR_LEN];

struct GTK_MACEntry {
	unsigned char MAC[ETH_ADDR_LEN];
	int occ; // 0 if empty
};

struct GTK_IGMPMLDEntry {
	spinlock_t			Flag;
	unsigned char		GroupId[ETH_ADDR_LEN];
	struct GTK_MACEntry GroupMember[GTK_IGMPMLDMACMax];
	int occ; // if no member, occ = 0
}	GTK_IGMPMLDTbl[GTK_IGMPMLDTblSize];

void InitGTK_IGMPMLDTbl( void )
{
	int	i = 0, j = 0;

	for ( i = 0 ; i < GTK_IGMPMLDTblSize ; i++ ) 
	{
		if ( !spin_is_locked( &(GTK_IGMPMLDTbl[i].Flag) ) ) 
		{
			spin_lock ( &(GTK_IGMPMLDTbl[i].Flag) );
			memset ( GTK_IGMPMLDTbl[i].GroupId , '\0' , ETH_ADDR_LEN );
			for ( j = 0 ; j < GTK_IGMPMLDMACMax ; j++ ) 
			{
				memset ( GTK_IGMPMLDTbl[i].GroupMember[j].MAC , '\0' , ETH_ADDR_LEN );
				GTK_IGMPMLDTbl[i].GroupMember[j].occ = 0;
			}	
			GTK_IGMPMLDTbl[i].occ = 0;
			spin_unlock( &(GTK_IGMPMLDTbl[i].Flag) );
		} 
		else 
		{
			DEBUG ( "%s:Item #%d has been locked\n" , __FUNCTION__ , i );
		}
	}
}

void	showIGMPMLDtable( void )
{
	int i, j, z = 0;
	unsigned char *tmp1, *tmp2;
	
	DEBUG("\nshowIGMPMLDtable: now showing all the data in database...\n");
	DEBUG("***Begin*************************************************\n");
	
	for(i = 0; i < GTK_IGMPMLDTblSize; i++)
	{
		if( GTK_IGMPMLDTbl[i].occ == 1 )
		{
			z++;
			tmp1 = GTK_IGMPMLDTbl[i].GroupId;
			DEBUG("Group index %d: %02X-%02X-%02X-%02X-%02X-%02X\n", i, tmp1[0], tmp1[1], tmp1[2], tmp1[3], tmp1[4], tmp1[5]);
			for(j = 0; j < GTK_IGMPMLDMACMax; j++)
			{
				if( GTK_IGMPMLDTbl[i].GroupMember[j].occ == 1 )
				{
					tmp2 = GTK_IGMPMLDTbl[i].GroupMember[j].MAC;
					DEBUG("->Member index %d: %02X-%02X-%02X-%02X-%02X-%02X\n", j, tmp2[0], tmp2[1], tmp2[2], tmp2[3], tmp2[4], tmp2[5]);
				}
			}
		}
	}
	if(z == 0)
		DEBUG("showIGMPMLDtable: database is totally empty...\n");
	DEBUG("***End***************************************************\n\n");	
}

// return existing Group index or (-1 & insertPoint)
int isGroupexist(char *GroupId, int* insertPoint)
{
	int i;
	
	for(i=0; i < GTK_IGMPMLDTblSize; i++)
	{
		if(GTK_IGMPMLDTbl[i].occ == 0)
		{
			if(*insertPoint == -1)
				*insertPoint = i;
			continue;
		}
		if(!memcmp(GroupId, GTK_IGMPMLDTbl[i].GroupId, ETH_ADDR_LEN))
		{
			return i;
		}
	}
	return -1;
}

int isMemberexist(int search, char *SrcMAC, int* count) 
{
	struct GTK_MACEntry *pCurrent;
	int i;
	
	pCurrent = GTK_IGMPMLDTbl[search].GroupMember;
	
	for(i = 0; i < GTK_IGMPMLDMACMax; i++)
	{
		if((*(pCurrent + i)).occ == 0)
		{
			continue;
		}
		
		(*count) = (*count) + 1; // count occupied slots
		
		if(memcmp((*(pCurrent + i)).MAC, SrcMAC, ETH_ADDR_LEN) == 0)
		{
			return i;
		}
	}
	return -1;
}

int joinAction(char* GroupId, char* SrcMAC)
{
	int insertPoint = -1, count = 0;
	int search;
	unsigned char Filter1[ ETH_ADDR_LEN ] = { 0x01, 0x00, 0x5e, 0x7f, 0xff, 0xfa };
	unsigned char Filter2[ 3 ] = { 0x33, 0x33, 0xff };
	
	/* This group is belong to the local UPnP, we don't want it to do packet clone. */
	if(!memcmp( Filter1, GroupId, ETH_ADDR_LEN ))
	{
		DEBUG("joinAction: multicast for local UPnP, do nothing...\n");
		return	0;
	}
	else if(!memcmp( Filter2, GroupId, 3 ))
	{
		DEBUG("joinAction: IPv6 solicited-node multicast address detected, do nothing...\n");
		return	0;
	}
	
	search = isGroupexist(GroupId, &insertPoint);
	
	if(search == -1 && insertPoint == -1)
	{
		DEBUG("joinAction: GroupId not found, database is full...\n");
	}
	else if(search == -1 && insertPoint != -1)
	{
		DEBUG("joinAction: GroupId not found, insert point is %d...\n", insertPoint);
		insertGroup(GroupId, SrcMAC, insertPoint);
	}
	else if(search != -1)
	{
		DEBUG("joinAction: GroupId found, index is %d...\n", search);
		if( isMemberexist(search, SrcMAC, &count) == -1 )
		{
			if(count == GTK_IGMPMLDMACMax)
			{
				DEBUG("joinAction: no space for this insertion...\n");
			}
			else
				insertMember(GroupId, SrcMAC, search);
		}
		else
			DEBUG("joinAction: Member found...\n");	
	}
	showIGMPMLDtable();
	return 0;
}

int leaveAction(char* GroupId, char* SrcMAC)
{
	int insertPoint = -1, count = 0;
	int search;
	int msearch;
	
	search = isGroupexist(GroupId, &insertPoint);
	
	if(search != -1)
	{
		DEBUG("leaveAction: GroupId found, index is %d...\n", search);
		msearch = isMemberexist(search, SrcMAC, &count);
		if(msearch == -1)
			DEBUG("leaveAction: Member not found...\n");
		else
		{
			removeMember(search, msearch);
			if(count == 1)
			{
				// if there is only one occupied slot so far, it is ok to check if the group is empty
				removeGroup(search);
			}
		}
	}
	else
	{
		DEBUG("leaveAction: GroupId not found...\n");
	}
	showIGMPMLDtable();
	return 0;
}

int insertGroup(char* GroupId, char* SrcMAC, int insertPoint)
{
	if ( !spin_is_locked ( &(GTK_IGMPMLDTbl[insertPoint].Flag) ) ) 
	{
		spin_lock ( &(GTK_IGMPMLDTbl[insertPoint].Flag) );
		memcpy ( GTK_IGMPMLDTbl[insertPoint].GroupId , GroupId , ETH_ADDR_LEN );
		memcpy ( GTK_IGMPMLDTbl[insertPoint].GroupMember[0].MAC, SrcMAC , ETH_ADDR_LEN );
		GTK_IGMPMLDTbl[insertPoint].GroupMember[0].occ = 1;
		GTK_IGMPMLDTbl[insertPoint].occ = 1;
		spin_unlock ( &(GTK_IGMPMLDTbl[insertPoint].Flag) );
	} 
	else 
	{
		DEBUG ( "%s:Item #%d has been locked\n" , __FUNCTION__ , insertPoint );
	}
	return 0;
}

int insertMember(char* GroupId, char* SrcMAC, int search)
{
	struct GTK_MACEntry *pCurrent;
	int i;
	
	pCurrent = GTK_IGMPMLDTbl[search].GroupMember;
	
	for(i = 0; i < GTK_IGMPMLDMACMax; i++)
	{
		if ((*(pCurrent + i)).occ == 0)
		{
			if ( !spin_is_locked ( &(GTK_IGMPMLDTbl[search].Flag) ) ) 
			{
				spin_lock ( &(GTK_IGMPMLDTbl[search].Flag) );
				memcpy ( (*(pCurrent + i)).MAC, SrcMAC , ETH_ADDR_LEN );
				(*(pCurrent + i)).occ = 1;
				spin_unlock ( &(GTK_IGMPMLDTbl[search].Flag) );
				break;
			} 
			else
			{
				DEBUG ( "%s:Item #%d has been locked\n" , __FUNCTION__ , search );
			}	
		}
	}
	return 0;
}

int removeMember(int search, int msearch)
{
	struct GTK_MACEntry *pCurrent;
		
	pCurrent = GTK_IGMPMLDTbl[search].GroupMember;
	
	if ( !spin_is_locked ( &(GTK_IGMPMLDTbl[search].Flag) ) ) 
	{
		spin_lock ( &(GTK_IGMPMLDTbl[search].Flag) );
		memset ( (*(pCurrent + msearch)).MAC , '\0' , ETH_ADDR_LEN );
		(*(pCurrent + msearch)).occ = 0;
		spin_unlock ( &(GTK_IGMPMLDTbl[search].Flag) );
	} 
	else 
	{
		DEBUG ( "%s:Item #%d has been locked\n" , __FUNCTION__ , search );
	}	
	
	return 0;
}

int removeGroup(int search)
{
	int i, occ = 0;
	struct GTK_MACEntry *pCurrent;
		
	pCurrent = GTK_IGMPMLDTbl[search].GroupMember;
	
	for(i = 0; i < GTK_IGMPMLDMACMax; i++)
	{
		occ += (*(pCurrent + i)).occ;
	}
	if(occ == 0)
	{
		if ( !spin_is_locked ( &(GTK_IGMPMLDTbl[search].Flag) ) ) 
		{
			spin_lock ( &(GTK_IGMPMLDTbl[search].Flag) );
			GTK_IGMPMLDTbl[search].occ = 0;
			memset ( GTK_IGMPMLDTbl[i].GroupId , '\0' , ETH_ADDR_LEN );
			DEBUG("removeGroup: group index %d removed...\n", search);
			spin_unlock ( &(GTK_IGMPMLDTbl[search].Flag) );
		} 
		else 
		{
			DEBUG ( "%s:Item #%d has been locked\n" , __FUNCTION__ , search );
		}	
	}
	
	return 0;
}

int IgmpForward(struct sk_buff *skb)
{
	//DEBUG("%s-%d: saddr=%08x daddr=%08x\n" , __FUNCTION__ , __LINE__ , skb->nh.iph->saddr , skb->nh.iph->daddr);
	if( skb->nh.iph->saddr == 0 )
		return 0;
	
	skb->pkt_type = PACKET_OUTGOING;
	skb->dev = dev_get_by_name ( wan_ifname );
	memcpy( eth_hdr(skb)->h_source, wan_mac , ETH_ADDR_LEN);
  skb->nh.iph->saddr = 	wan_ipaddr;
	skb->nh.iph->check = ip_fast_csum((unsigned char *)skb->nh.iph, skb->nh.iph->ihl);
	if( NULL == strstr( wan_ifname , "ppp") )
		skb_push( skb, ETH_HLEN );
	dev_queue_xmit( skb );
	//DEBUG("Multi2Unicast:  unicast cloned packet for index %d done...\n", i);
	
	return 1;	
}

int Multi2Unicast(struct sk_buff *skb, int search)
{
	struct sk_buff *pskb;
	int i;
	struct GTK_MACEntry *pCurrent;
	
	unsigned char *pDstMacAddr = eth_hdr(skb)->h_dest;
		
	pCurrent = GTK_IGMPMLDTbl[search].GroupMember;
	
	for(i = 0; i < GTK_IGMPMLDMACMax; i++)
	{
		if( (*(pCurrent + i)).occ == 1 )
		{
			pskb = skb_clone(skb, GFP_ATOMIC);
			if(pskb == NULL)
			{
				DEBUG("Multi2Unicast: failed to clone packet for index %d...\n", i);
				continue;
			}
			memcpy( eth_hdr(pskb)->h_dest, (*(pCurrent + i)).MAC, ETH_ADDR_LEN);
			pskb->pkt_type = PACKET_OUTGOING;
			pskb->dev = dev_get_by_name ( LANIFNAME );
			skb_push( pskb, ETH_HLEN );
			dev_queue_xmit( pskb );
			//DEBUG("Multi2Unicast:  unicast cloned packet for index %d done...\n", i);
		}
	}
	kfree_skb(skb);
	
	return 0;	
}

//*******************End of igmp_mld_db.c****************************************************
