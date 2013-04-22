/***************************************************************
**Gemtek +++ 20081124 ---
**IGMP Snooping(IPv4) function & MLD Snooping(IPv6) function
**References from:
**
**
****************************************************************/
#include <linux/ip.h>

#if 0
#define DEBUG printk
#else
#define DEBUG(format, args...)
#endif

int igmp_snoop_enabled = 0;
int mld_snoop_enabled = 0;

static struct	proc_dir_entry		*igmp_snoop = NULL;
static struct	proc_dir_entry		*mld_snoop = NULL;

#define		IGMP_PROTOCOL_DESCRIPTOR		0x02
#define 	IPV6_NEXT_HEADER_ICMPV6			0x3a

#define		MODE_IS_INCLUDE							1
#define		MODE_IS_EXCLUDE							2
#define		CHANGE_TO_INCLUDE_MODE			3
#define		CHANGE_TO_EXCLUDE_MODE			4
#define		ALLOW_NEW_SOURCES						5
#define		BLOCK_OLD_SOURCES						6

#define		IGMP_V1_MEMBERSHIP_REPORT		0x12
#define 	IGMP_V2_MEMBERSHIP_REPORT		0x16
#define 	IGMP_LEAVE_GROUP						0x17
#define 	IGMP_V3_MEMBERSHIP_REPORT		0x22

#define 	MLD_V1_LISTENER_REPORT			131
#define 	MLD_V1_LISTENER_DONE				132
#define 	MLD_V1_ROUTER_ADVERTISEMENT	134
#define 	MLD_V1_ROUTER_SOLICITATION	135
#define 	MLD_V2_LISTERNER_REPORT			143

// defined as sequence in IPv6 header
#define		IPV6_NEXT_HEADER_HOP_BY_HOP				0x00	// 0
#define 	IPV6_NEXT_HEADER_DESTINATION			0x3c	// 60
#define 	IPV6_NEXT_HEADER_ROUTING					0x2b	// 43
#define 	IPV6_NEXT_HEADER_FRAGMENT					0x2c	// 44
#define 	IPV6_NEXT_HEADER_AUTHENTICATION		0x33  // 51
#define 	IPV6_NEXT_HEADER_ENCAPSULATION		0x32  // 50, RFC-2406
#define 	IPV6_NEXT_HEADER_NONE							0x3b	// 59

#define		ETH_ADDR_LEN					6
#define		LANIFNAME		"br0"

#define   true  1 
#define   false 0

typedef   unsigned char     bool;
typedef		bool							BOOLEAN;
typedef		unsigned char*		PUCHAR;
typedef		unsigned int			UINT16;
typedef		unsigned char			UCHAR;
typedef		unsigned char			UINT8;
typedef		unsigned int			UINT32;
typedef		unsigned short		USHORT;

#define		IPV6_HDR_LEN			40
#define 	IPV6_ADDR_LEN 		16

typedef struct _rt_ipv6_ext_hdr_
{
	UCHAR	nextProto; // Indicate the protocol type of next extension header.
	UCHAR	extHdrLen; // optional field for msg length of this extension header which didn't include the first "nextProto" field.
	UCHAR	octets[1]; // hook to extend header message body.
}RT_IPV6_EXT_HDR, *PRT_IPV6_EXT_HDR;

/* IPv6 Address related structures */
typedef struct rt_ipv6_addr_
{
	union
	{
		UCHAR		ipv6Addr8[16];
		USHORT	ipv6Addr16[8];
		UINT32	ipv6Addr32[4];
	}addr;
#define ipv6_addr				addr.ipv6Addr8
#define ipv6_addr16			addr.ipv6Addr16
#define ipv6_addr32			addr.ipv6Addr32
}RT_IPV6_ADDR, *PRT_IPV6_ADDR;

/*IPv6 Header related structures */
typedef struct _rt_ipv6_hdr_
{
	UINT32 				ver:4,
								trafficClass:8,
        		  	flowLabel:20;
	USHORT 				payload_len;
	UCHAR  				nextHdr;
	UCHAR  				hopLimit;
	RT_IPV6_ADDR  srcAddr;
	RT_IPV6_ADDR	dstAddr;
}RT_IPV6_HDR, *PRT_IPV6_HDR;

/* ICMPv6 related structures */
typedef struct _rt_ipv6_icmpv6_hdr_
{
	UCHAR		type;
	UCHAR		code;
	USHORT	chksum;
	UCHAR		octets[1]; //hook to extend header message body.
}RT_ICMPV6_HDR, *PRT_ICMPV6_HDR;

static inline void mcastIP2MAC(PUCHAR pIpAddr, PUCHAR *ppMacAddr, UINT16 ProtoType);
static inline BOOLEAN IPv6ExtHdrHandle(RT_IPV6_EXT_HDR	*pExtHdr, UCHAR	*pProto, UINT32 *pOffset);
static inline BOOLEAN IsSupportedMldMsg(UINT8 MsgType) ;
void InitGTK_IGMPMLDTbl( void );
void	showIGMPMLDtable( void );

extern char wan_ifname[8];
extern unsigned long int wan_ipaddr;
extern unsigned char wan_mac[6];

static BOOLEAN isIgmpMacAddr(PUCHAR pMacAddr)
{
	if((pMacAddr[0] == 0x01)
		&& (pMacAddr[1] == 0x00)
		&& (pMacAddr[2] == 0x5e))
		return true;
	return false;
}

int parseIgmpPkt(struct sk_buff *skb) // 0 when failed
{
	PUCHAR pIpHeader, pIGMPHeader;
	UCHAR IgmpProtocol, IgmpPktType;
		
	PUCHAR pDstMacAddr = eth_hdr(skb)->h_dest;
	PUCHAR pSrcMacAddr = eth_hdr(skb)->h_source;
	UINT16 IpProtocol = ntohs(eth_hdr(skb)->h_proto);

	UINT16 numOfGroup;
	UCHAR GroupType;
	PUCHAR pGroup;
	UCHAR AuxDataLen;
	UINT16 numOfSources;
	PUCHAR pGroupIpAddr;
	UCHAR GroupMacAddr[6];
	PUCHAR pGroupMacAddr = (PUCHAR)&GroupMacAddr;
	int i, IpHeaderLen;

	if(!isIgmpMacAddr(pDstMacAddr))
	{
		return 0;
	}
	
	if(IpProtocol == ETH_P_IP)
	{
		pIpHeader = (PUCHAR)(skb->nh.iph);
		
		if(pIpHeader == NULL)
			return 0;
		
		IgmpProtocol = (UCHAR)*(pIpHeader + 9);
		
		if(IgmpProtocol == IGMP_PROTOCOL_DESCRIPTOR)
		{
				//pIGMPHeader = (PUCHAR)(skb->h.igmph);
				IpHeaderLen = (*pIpHeader & 0x0f) * 4;
				pIGMPHeader = pIpHeader + IpHeaderLen;
				IgmpPktType = (UCHAR)*pIGMPHeader;
				//DEBUG("IgmpPktType: %02x\n", IgmpPktType);

				switch(IgmpPktType)
				{
					case IGMP_V1_MEMBERSHIP_REPORT:
					case IGMP_V2_MEMBERSHIP_REPORT:
						pGroupIpAddr = (PUCHAR)(pIGMPHeader + 4);
						mcastIP2MAC(pGroupIpAddr, (PUCHAR *)&pGroupMacAddr, ETH_P_IP);
						joinAction(GroupMacAddr, pSrcMacAddr);
						break;
					case IGMP_LEAVE_GROUP:
						pGroupIpAddr = (PUCHAR)(pIGMPHeader + 4);
						mcastIP2MAC(pGroupIpAddr, (PUCHAR *)&pGroupMacAddr, ETH_P_IP);
						leaveAction(GroupMacAddr, pSrcMacAddr);
						break;					
					case IGMP_V3_MEMBERSHIP_REPORT:
						numOfGroup = ntohs(*((UINT16 *)(pIGMPHeader + 6)));
						pGroup = (PUCHAR)(pIGMPHeader + 8);
						for (i=0; i < numOfGroup; i++)
						{
							GroupType = (UCHAR)(*pGroup);
							AuxDataLen = (UCHAR)(*(pGroup + 1));
							numOfSources = ntohs(*((UINT16 *)(pGroup + 2)));
							pGroupIpAddr = (PUCHAR)(pGroup + 4);
							mcastIP2MAC(pGroupIpAddr, (PUCHAR *)&pGroupMacAddr, ETH_P_IP);
							//DEBUG("IGMP Group=%02x:%02x:%02x:%02x:%02x:%02x\n",
							//	GroupMacAddr[0], GroupMacAddr[1], GroupMacAddr[2], GroupMacAddr[3], GroupMacAddr[4], GroupMacAddr[5]);

							do
							{
								if((GroupType == MODE_IS_EXCLUDE) || (GroupType == CHANGE_TO_EXCLUDE_MODE) || (GroupType == ALLOW_NEW_SOURCES))
								{
									//DEBUG("parseIgmpPkt: multicast from %s...\n", skb->dev->name);
									joinAction(GroupMacAddr, pSrcMacAddr);
									break;
								}

								if((GroupType == MODE_IS_INCLUDE) || (GroupType == BLOCK_OLD_SOURCES))
								{
									leaveAction(GroupMacAddr, pSrcMacAddr);
									break;
								}

								if((GroupType == CHANGE_TO_INCLUDE_MODE))
								{
									if(numOfSources == 0)
									{
										leaveAction(GroupMacAddr, pSrcMacAddr);
									}
									else
									{
										joinAction(GroupMacAddr, pSrcMacAddr);
									}
									break;
								}
							} while(false);
							pGroup += (8 + (numOfSources * 4) + AuxDataLen);
						}
						break;			
					default:
						DEBUG("IGMP: unknown IgmpPktType...\n");
						break;
				}
				return 1;
		}
	}

	return 0;
}

static inline BOOLEAN isMldMacAddr(PUCHAR pMacAddr)
{
	return ((pMacAddr[0] == 0x33) && (pMacAddr[1] == 0x33)) ? true : false;
}

int parseMldPkt(struct sk_buff *skb)
{
	PUCHAR pIpHeader;
	
	UINT8 MldType;
	PUCHAR pMldHeader;
	
	PUCHAR pGroupIpAddr;
	UCHAR GroupMacAddr[6];
	PUCHAR pGroupMacAddr = (PUCHAR)&GroupMacAddr;
	UINT16 numOfGroup;
	UCHAR GroupType;
	PUCHAR pGroup;
	UCHAR AuxDataLen;
	UINT16 numOfSources;
	int i;
	
	int result = 0;
	UINT16 IpProtocol = ntohs(eth_hdr(skb)->h_proto);
	PUCHAR pDstMacAddr = eth_hdr(skb)->h_dest;
	PUCHAR pSrcMacAddr = eth_hdr(skb)->h_source;

	if(!isMldMacAddr(pDstMacAddr))
		return 0;
	
	if(IpProtocol != ETH_P_IPV6)
		return 0;

	do
	{
		pIpHeader = (PUCHAR)(skb->nh.ipv6h);
		
		if(pIpHeader == NULL)
		{
			return 0;
		}
		PRT_IPV6_HDR pIpv6Hdr = (PRT_IPV6_HDR)(pIpHeader);
		UINT8 nextProtocol = pIpv6Hdr->nextHdr;
		UINT32 offset = IPV6_HDR_LEN;

		while(nextProtocol != IPV6_NEXT_HEADER_ICMPV6)
		{
			if(IPv6ExtHdrHandle((RT_IPV6_EXT_HDR *)(pIpHeader + offset), &nextProtocol, &offset) == false)
				break;
		}
		
		if(nextProtocol == IPV6_NEXT_HEADER_ICMPV6)
		{
			PRT_ICMPV6_HDR pICMPv6Hdr = (PRT_ICMPV6_HDR)(pIpHeader + offset);
			if (IsSupportedMldMsg(pICMPv6Hdr->type) == true)
			{
				//DEBUG("parseMld: this is a supported ICMPv6 packet\n");
				MldType = pICMPv6Hdr->type;
				pMldHeader = (PUCHAR)pICMPv6Hdr;
				
				switch(MldType)
				{
					case MLD_V1_LISTENER_REPORT:
						// skip Type(1 Byte), code(1 Byte), checksum(2 Bytes), Maximum Rsp Delay(2 Bytes), Reserve(2 Bytes).
						pGroupIpAddr = (PUCHAR)(pMldHeader + 8);
						mcastIP2MAC(pGroupIpAddr, (PUCHAR *)&pGroupMacAddr, ETH_P_IPV6);
						joinAction(GroupMacAddr, pSrcMacAddr);
						break;

					case MLD_V1_LISTENER_DONE:
						// skip Type(1 Byte), code(1 Byte), checksum(2 Bytes), Maximum Rsp Delay(2 Bytes), Reserve(2 Bytes).
						pGroupIpAddr = (PUCHAR)(pMldHeader + 8);
						mcastIP2MAC(pGroupIpAddr, (PUCHAR *)&pGroupMacAddr, ETH_P_IPV6);
						leaveAction(GroupMacAddr, pSrcMacAddr);
						break;

					case MLD_V2_LISTERNER_REPORT: // IGMP version 3 membership report.
						numOfGroup = ntohs(*((UINT16 *)(pMldHeader + 6)));
						pGroup = (PUCHAR)(pMldHeader + 8);
						for (i=0; i < numOfGroup; i++)
						{
							GroupType = (UCHAR)(*pGroup);
							AuxDataLen = (UCHAR)(*(pGroup + 1));
							numOfSources = ntohs(*((UINT16 *)(pGroup + 2)));
							pGroupIpAddr = (PUCHAR)(pGroup + 4);
							mcastIP2MAC(pGroupIpAddr, (PUCHAR *)&pGroupMacAddr, ETH_P_IPV6);
							
							do
							{
								if((GroupType == MODE_IS_EXCLUDE) || (GroupType == CHANGE_TO_EXCLUDE_MODE) || (GroupType == ALLOW_NEW_SOURCES))
								{
									joinAction(GroupMacAddr, pSrcMacAddr);
									break;
								}

								if((GroupType == MODE_IS_INCLUDE) || (GroupType == BLOCK_OLD_SOURCES))
								{
									leaveAction(GroupMacAddr, pSrcMacAddr);
									break;
								}

								if((GroupType == CHANGE_TO_INCLUDE_MODE))
								{
									if(numOfSources == 0)
										leaveAction(GroupMacAddr, pSrcMacAddr);
									else
										joinAction(GroupMacAddr, pSrcMacAddr);
									break;
								}
							} while(false);
							// skip 4 Bytes (Record Type, Aux Data Len, Number of Sources) + a IPv6 address.
							pGroup += (4 + IPV6_ADDR_LEN + (numOfSources * 16) + AuxDataLen);
						}
						break;

					default:
						break;
				}
				
				result = 1;
			}
		}
	}while(false);

	return result;
}

int IGMPMLD_LAN_Processor(struct sk_buff *skb)
{
	int result;
	
	// please refer to igmp_mld_switch(): here at least one snooping is enabled
	if(igmp_snoop_enabled == 1)
	{
		if ( (result = parseIgmpPkt(skb)) == 1 )
		{
			return IgmpForward(skb);
		}		
	}
	return 0;
}

static inline void mcastIP2MAC(PUCHAR pIpAddr, PUCHAR *ppMacAddr, UINT16 ProtoType)
{
	if (pIpAddr == NULL)
		return;

	if (ppMacAddr == NULL || *ppMacAddr == NULL)
		return;

	switch (ProtoType)
	{
		case ETH_P_IPV6:
			*(*ppMacAddr) = 0x33;
			*(*ppMacAddr + 1) = 0x33;
			*(*ppMacAddr + 2) = pIpAddr[12];
			*(*ppMacAddr + 3) = pIpAddr[13];
			*(*ppMacAddr + 4) = pIpAddr[14];
			*(*ppMacAddr + 5) = pIpAddr[15];
			break;

		case ETH_P_IP:
		default:
			*(*ppMacAddr) = 0x01;
			*(*ppMacAddr + 1) = 0x00;
			*(*ppMacAddr + 2) = 0x5e;
			*(*ppMacAddr + 3) = pIpAddr[1] & 0x7f; // RL uses this, and it's not a problem
			*(*ppMacAddr + 4) = pIpAddr[2];
			*(*ppMacAddr + 5) = pIpAddr[3];
			break;
	}
	DEBUG("pIpAddr[1]=%02x,pIpAddr[2]=%02x,pIpAddr[3]=%02x\n",
	       pIpAddr[1],pIpAddr[2],pIpAddr[3]);
	return;
}

static inline BOOLEAN IPv6ExtHdrHandle(RT_IPV6_EXT_HDR	*pExtHdr, UCHAR	*pProto, UINT32 *pOffset)
{
	UCHAR nextProto = 0xff;
	UINT32 extLen = 0;
	BOOLEAN status = true;

	//DEBUG("%s(): parsing the Extension Header with Protocol(0x%x):\n", __FUNCTION__, *pProto);
	switch (*pProto)
	{
		case IPV6_NEXT_HEADER_HOP_BY_HOP:
			// IPv6ExtHopByHopHandle();
			nextProto = pExtHdr->nextProto;
			extLen = (pExtHdr->extHdrLen + 1) * 8;
			break;
			
		case IPV6_NEXT_HEADER_DESTINATION:
			// IPv6ExtDestHandle();
			nextProto = pExtHdr->nextProto;
			extLen = (pExtHdr->extHdrLen + 1) * 8;
			break;
			
		case IPV6_NEXT_HEADER_ROUTING:
			// IPv6ExtRoutingHandle();
			nextProto = pExtHdr->nextProto;
			extLen = (pExtHdr->extHdrLen + 1) * 8;
			break;
			
		case IPV6_NEXT_HEADER_FRAGMENT:
			// IPv6ExtFragmentHandle();
			nextProto = pExtHdr->nextProto;
			extLen = 8; // The Fragment header length is fixed to 8 bytes.
			break;
			
		case IPV6_NEXT_HEADER_AUTHENTICATION:
		//   IPV6_NEXT_HEADER_ENCAPSULATION:
			/*
				TODO: Not support. For encryption issue.
			*/
			nextProto = 0xFF;
			status = false;
			break;

		default:
			nextProto = 0xFF;
			status = false;			
			break;
	}

	*pProto = nextProto;
	*pOffset += extLen;
	//DEBUG("%s(): nextProto = 0x%x!, offset=0x%x!\n", __FUNCTION__, nextProto, offset);
	
	return status;
	
}

static inline BOOLEAN IsSupportedMldMsg(UINT8 MsgType) 
{
	BOOLEAN result = false;
	switch(MsgType)
	{
		case MLD_V1_LISTENER_REPORT:
		case MLD_V1_LISTENER_DONE:
		case MLD_V2_LISTERNER_REPORT:
			result = true;
			break;
		default:
			result = false;
			break;
	}

	return result;
}

// IF statement is designed to avoid unnecessary processing
int parseWANPkt(struct sk_buff *skb)	
{
	PUCHAR pDstMacAddr = eth_hdr(skb)->h_dest;
	int insertPoint = 0; // don't care
	int search;
	unsigned char tmpmac[6];

	// if the function is disabled, we don't go further
	if( (igmp_snoop_enabled == 1 && isIgmpMacAddr(pDstMacAddr))
			|| (mld_snoop_enabled == 1 && isMldMacAddr(pDstMacAddr))
			|| strstr( skb->dev->name , "ppp") )
	{
	
		if( strncmp( skb->dev->name , "ppp" , 3) == 0 )
		{
				if( skb->nh.iph->daddr < 0xE0000000 || skb->nh.iph->daddr > 0xEFFFFFFF )
						return 0;//is a unicast
				pDstMacAddr = tmpmac;
				mcastIP2MAC( (unsigned char*)&skb->nh.iph->daddr , (PUCHAR *)&pDstMacAddr , ETH_P_IP );
		}				
		search = isGroupexist(pDstMacAddr, &insertPoint);
		if(search == -1)
		{
			// Drop normal multicast packet, let ICMPv6 pass
			// When the packet here is for IGMP, just drop it
			if(mld_snoop_enabled == 0 || isICMPv6Pkt(skb) == 0)
			{
				//DEBUG("parseWANPkt: this packet should be dropped, dropping now...\n");
				return -1;
			}
		}
		else
		{
			// Need to clone packet and unicast to all
			//DEBUG("parseWANPkt: found at index: %d, need to unicast it...\n", search);
			//Multi2Unicast(skb, search);
			McastForward(skb);
			return 1;
		}
	}
	
	return 0;
}

int isICMPv6Pkt(struct sk_buff *skb)
{
	UINT16 IpProtocol = ntohs(eth_hdr(skb)->h_proto);
	PUCHAR pIpHeader;
	
	if(IpProtocol != ETH_P_IPV6)
		return 0;
	
	do
	{
		pIpHeader = (PUCHAR)(skb->nh.ipv6h);
		
		if(pIpHeader == NULL)
		{
			return 0;
		}
		
		PRT_IPV6_HDR pIpv6Hdr = (PRT_IPV6_HDR)(pIpHeader);
		UINT8 nextProtocol = pIpv6Hdr->nextHdr;
		UINT32 offset = IPV6_HDR_LEN;

		while(nextProtocol != IPV6_NEXT_HEADER_ICMPV6)
		{
			if(IPv6ExtHdrHandle((RT_IPV6_EXT_HDR *)(pIpHeader + offset), &nextProtocol, &offset) == false)
				break;
		}
		
		if(nextProtocol == IPV6_NEXT_HEADER_ICMPV6)
		{
			return 1;
		}
		
	}while(false);
	
	return 0;
}

BOOLEAN igmp_mld_switch( void )
{
	if(igmp_snoop_enabled == 1 || mld_snoop_enabled == 1)
		return true;
	else	
		return false;
}

int	proc_igmp_read( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	int ret = 0;

	if ( igmp_snoop_enabled == 1 ) 
	{
		showIGMPMLDtable();
		//ret = sprintf( page , "igmp_snoop is enabled...\n" );
	} 
	else 
	{
		ret = sprintf ( page , "igmp_snoop is disabled...\n" );
	}

	return	ret;
}

int	proc_igmp_write( struct file *file , const char *buffer , unsigned long count , void *data )
{
	// echo "1"> ; echo "1" > ; echo 1 > ; echo '1'>; echo '1' > ; /proc/igmp_snoop
	char tmpbuf[128],*start_p,*end_p,*enable,*p_ip;
	unsigned char ip_num=0;
	int i;
	
	memset( tmpbuf , 0 , 128 );		
	if( *buffer == NULL )
	{	
		if(igmp_snoop_enabled != 0)
		{
			igmp_snoop_enabled = 0;
			if(mld_snoop_enabled == 0)
				InitGTK_IGMPMLDTbl(); // reset
		}
		//showIGMPMLDtable();
		DEBUG("igmp_snoop: disabled\n");
	}
	else
	{
	  DEBUG("%s-%d: buffer = %s\n" , __FUNCTION__ , __LINE__ , buffer );
	  strcpy( tmpbuf , buffer );
	  start_p = tmpbuf ;
	  enable = strsep( &start_p , " ");
		if(*enable == '1')
		{
			strcpy( wan_ifname , "eth1" ); 
			wan_ipaddr = 0;
			memset( wan_mac , 0 , 6); 
			igmp_snoop_enabled = 1;
			showIGMPMLDtable();
			DEBUG("igmp_snoop: enabled\n");
			if( start_p != NULL )
			{
					end_p = strsep( &start_p , " ");
					strcpy( wan_ifname , end_p ); 
			}
			if( start_p != NULL )
			{
					end_p = strsep( &start_p , " ");
					for( i = 0 ; i < 4 ; i++ )
					{
							ip_num = (unsigned char)simple_strtoul( end_p , &p_ip , 10);
							wan_ipaddr = (wan_ipaddr << 8) | ip_num;
							end_p = p_ip+1;
					}		 
			}
			if( start_p != NULL )
			{
					end_p = strsep( &start_p , " ");
					for( i = 0 ; i < 6 ; i++ )
					{
							wan_mac[i] = (unsigned char)simple_strtoul( end_p , &p_ip , 16);
							end_p = p_ip+1;
					}		 
			}
			DEBUG("%s-%d\n"
			       "wan_ifname=%s\n"
						 "wan_ipaddr=%08x\n"
						 "wan_mac : %02X:%02X:%02X:%02X:%02X:%02X\n",
						 __FUNCTION__ , __LINE__ , wan_ifname , wan_ipaddr , 
						 wan_mac[0],wan_mac[1],wan_mac[2],wan_mac[3],wan_mac[4],wan_mac[5]);
			
		}
		else
		{
			if(igmp_snoop_enabled != 0)
			{
				igmp_snoop_enabled = 0;
				if(mld_snoop_enabled == 0)
					InitGTK_IGMPMLDTbl(); // reset
			}
			//showIGMPMLDtable();
			DEBUG("igmp_snoop: disabled\n");
		}
	}
	
	return	count;
}

int	proc_mld_read( char *page , char **start , off_t off , int count , int *eof , void *data )
{
	int ret = 0;

	if ( mld_snoop_enabled == 1 ) 
	{
		showIGMPMLDtable();
		//ret = sprintf( page , "mld_snoop is enabled...\n" );
	} 
	else 
	{
		ret = sprintf ( page , "mld_snoop is disabled...\n" );
	}

	return	ret;
}

int	proc_mld_write( struct file *file , const char *buffer , unsigned long count , void *data )
{
	// echo "1"> ; echo "1" > ; echo 1 > ; echo '1'>; echo '1' > ; /proc/mld_snoop
	
	if( *buffer == NULL )
	{	
		if(mld_snoop_enabled != 0)
		{
			mld_snoop_enabled = 0;
			if(igmp_snoop_enabled == 0)
				InitGTK_IGMPMLDTbl(); // reset
		}
		//showIGMPMLDtable();
		DEBUG("mld_snoop: disabled\n");
	}
	else
	{
		if(*buffer == '1')
		{
			mld_snoop_enabled = 1;
			showIGMPMLDtable();
			DEBUG("mld_snoop: enabled\n");
		}
		else
		{
			if(mld_snoop_enabled != 0)
			{
				mld_snoop_enabled = 0;
				if(igmp_snoop_enabled == 0)
					InitGTK_IGMPMLDTbl(); // reset
			}
			//showIGMPMLDtable();
			DEBUG("mld_snoop: disabled\n");
		}
	}
	
	return	count;
}

int igmp_snoop_system_configuration( void )
{
	igmp_snoop_enabled = 0;
	
	if ( igmp_snoop == NULL ) 
	{
		igmp_snoop = create_proc_entry( "igmp_snoop" , 0644 , NULL );
		if ( igmp_snoop == NULL ) 
		{
			DEBUG ( "igmp_snoop : create_proc_entry ... failed\n" );
		} 
		else 
		{
			igmp_snoop->read_proc = proc_igmp_read;
			igmp_snoop->write_proc = proc_igmp_write;
			return	0;
		}
	} 
	else 
	{
		DEBUG ( "igmp_snoop : proc_entry named igmp_snoop has been created one moment ago.\n" );
	}

	return	-1;
	
}

int mld_snoop_system_configuration( void )
{
	mld_snoop_enabled = 0;
	
	if ( mld_snoop == NULL ) 
	{
		mld_snoop = create_proc_entry( "mld_snoop" , 0644 , NULL );
		if ( mld_snoop == NULL ) 
		{
			DEBUG ( "mld_snoop : create_proc_entry ... failed\n" );
		} 
		else 
		{
			mld_snoop->read_proc = proc_mld_read;
			mld_snoop->write_proc = proc_mld_write;
			return	0;
		}
	} 
	else 
	{
		DEBUG ( "mld_snoop : proc_entry named mld_snoop has been created one moment ago.\n" );
	}

	return	-1;
	
}

int McastForward(struct sk_buff *skb)
{
	char devname_tmp[8];
	struct sk_buff *pskb;
	unsigned char tmpmac[6];
	unsigned char* pDstMacAddr ;
	unsigned char new_header[14];
	
	strcpy( devname_tmp , skb->dev->name );

	if( memcmp( devname_tmp , "ppp" , 3)!=0 )
	{
			skb->pkt_type = PACKET_OUTGOING;
			skb->dev = dev_get_by_name ( LANIFNAME );
			skb_push( skb, ETH_HLEN );
			dev_queue_xmit( skb );
	}		
	else
	{
				
			pskb = skb_clone(skb, GFP_ATOMIC);
			if(pskb == NULL)
			{
				DEBUG("Mcast PPP forward fail...\n");
				return 0;
			}
			pDstMacAddr = tmpmac;
			mcastIP2MAC( (unsigned char*)&pskb->nh.iph->daddr , (unsigned char**)&pDstMacAddr , ETH_P_IP );
			pskb->pkt_type = PACKET_OUTGOING;
			pskb->dev = dev_get_by_name ( LANIFNAME );
			//memcpy( new_header , pskb->data , 14 );//copy original data to buffer;
			skb_push( pskb, ETH_HLEN+8 );//data point to ip header , so move forward 22 bytes 
			memcpy( pskb->data, pDstMacAddr, ETH_ADDR_LEN);
			memcpy( new_header , pskb->data , 14 );
			skb_pull( pskb, 8 );
			memcpy( pskb->data, new_header, ETH_HLEN);
			memset( pskb->data+12, 0x08, 1);
			memset( pskb->data+13, 0x00, 1);
			pskb->protocol = ETH_P_IP ;
			pskb->csum = skb_checksum( pskb , 0 , pskb->len , 0);			
			dev_queue_xmit( pskb );	 
			kfree_skb(skb); 
	}				
	//DEBUG("Multi2Unicast:  unicast cloned packet for index %d done...\n", i);
	
	return 0;	
}

//*******************End of igmp_mld_snoop.h****************************************************
