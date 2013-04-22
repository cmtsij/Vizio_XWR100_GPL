#ifndef	_Dump_DHCP_Lease_Table_H_

#define	_Dump_DHCP_Lease_Table_H_

#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


struct lease_t {
	unsigned char chaddr[16];
	u_int32_t yiaddr;
	u_int32_t expires;
	char hostname[64];
};

void		dump_leasefile ( void );
int		hex2int ( char , char );
char		VerifyUsedIPAdd ( char * );

//	write the nvram value to lease table
void		dump_leasefile ( void )
{
	char		*dhcp_dumplease = NULL;
	char		lease_buf [ 128 ] , *next = NULL ,*bufp = NULL;
	char		*hostname = NULL , *mac = NULL , *ip = NULL , *tmp = NULL , *expires = NULL;
	int		i = 0 ;
	//char		VerifyUsedIPAddFlag = 0;
	FILE		*fp = NULL;
	struct in_addr		addr;
	struct lease_t		lease;
	
	char *lan_ipaddr 					= nvram_safe_get("lan_ipaddr");
	char lan_ipaddr_prefix[32];
	int lan_ipaddr_prefix_len;
	char *lastdot;
	char target_ip[32];
	
	memset(&lan_ipaddr_prefix, 0, sizeof(lan_ipaddr_prefix));
	
	lastdot = strrchr(lan_ipaddr, '.');
	lan_ipaddr_prefix_len = lastdot - lan_ipaddr;
	
	strncpy(lan_ipaddr_prefix, lan_ipaddr, lan_ipaddr_prefix_len);
	
//	if ( strncmp ( nvram_get ( "lan_proto" ) , "dhcp" , 4 ) != 0 ) {
//		nvram_unset ( "dhcp_leasedump" );
//		return	NULL;
//	}

	if ( ( dhcp_dumplease = nvram_get ( "dhcp_leasedump" ) ) == NULL ) {
		return	;
	}

	if ( strlen ( dhcp_dumplease ) <= 0 ) {
		return	;
	}

	if ( ( fp = fopen ( "/tmp/udhcpd.leases" , "a" ) ) == NULL ) {
//		printf ( "dump_leasefile ... open file ... fail\n" );
		return	;
	}

	foreach ( lease_buf , dhcp_dumplease , next ) {
		bufp = lease_buf ;
		hostname = strsep ( &bufp , "|" );
		mac = strsep ( &bufp , "|" );
		ip = strsep ( &bufp , "|" );
		expires = strsep ( &bufp , "|" );
//		printf ( "dump_leasefile ... %s %s %s\n" , hostname , mac , ip );
		strcpy ( lease.hostname , hostname );
		
#ifdef	VerifyUsedIPFuncEnableFlag
		VerifyUsedIPAddFlag = VerifyUsedIPAdd ( ip );	//	return 0	->	The used IP address is invalid.
		// printf ( "********** 1. VerifyUsedIPAddFlag = %d **********\n" , VerifyUsedIPAddFlag );
		if ( VerifyUsedIPAddFlag == 0 ) {		
			continue;
		}
#endif

		//	Convert IP prefix 
		memset(target_ip, 0, sizeof(target_ip));
		sprintf(target_ip, "%s%s", lan_ipaddr_prefix, strrchr(ip, '.'));
					
		//
		// printf ( "********** 2. VerifyUsedIPAddFlag = %d **********\n" , VerifyUsedIPAddFlag );
		for ( i = 0 ; i < 6 ; i++ ) {
			tmp = strsep ( &mac , ":" );
			lease.chaddr[i] = hex2int ( tmp[0] , tmp[1] );
		}
	
		inet_aton ( target_ip , &addr );
		lease.yiaddr = addr.s_addr;
		
		// lease.expires = atol ( nvram_get ( "DhcpReleaseTime" ) ) ;
		//lease.expires = 0;
		if (expires==NULL)
			lease.expires = htonl (86400);			
		else
			lease.expires = htonl ( atol(expires));
		
//		printf ( " %02x , %s" , lease.chaddr[0] , lease.hostname );
		fwrite ( lease.chaddr , 16 , 1 , fp );
		fwrite ( &lease.yiaddr , 4 , 1 , fp );
		fwrite ( &lease.expires , 4 , 1 , fp );
		fwrite ( lease.hostname , 64 , 1 , fp );
		
	}

	fclose ( fp );

	nvram_unset ( "dhcp_leasedump" );

	return	;
}

int	hex2int ( char hi , char lo )
{
	int	a = 0 , temp1 = 0 , temp2 = 0;

	if ( !isxdigit ( hi ) ) {
		hi = '0';
	}

	if ( !isxdigit ( lo ) ) {
		lo = hi;
		hi = '0';
	}

	if ( isdigit ( hi ) ) {
		temp1 = hi - '0';
	} else {
		temp1=( (int) tolower ( hi ) - 'a' ) + 10;
	}

	if ( isdigit ( lo ) ) {
		temp2 = lo - '0';
	} else {
		temp2 = ( (int) tolower ( lo ) - 'a' ) + 10;
	}

	a = temp1 * 16 + temp2;

	return	a;
}

#ifdef	VerifyUsedIPFuncEnableFlag
//	return 1	->	The used IP address is valid.
//	return 0	->	The used IP address is invalid.
char		VerifyUsedIPAdd ( char *Used_IP )
{
	char		*lan_ipaddr = NULL , *lan_netmask = NULL , TempBuff [ 32 ];
	int		Used_IP_1 = 0 , Used_IP_2 = 0 , Used_IP_3 = 0 , Used_IP_4 = 0;
	int		LAN_IP_1 = 0 , LAN_IP_2 = 0 , LAN_IP_3 = 0 , LAN_IP_4 = 0;
	int		MASK_1 = 0 , MASK_2 = 0 , MASK_3 = 0 , MASK_4 = 0;
	int		i = 0;

	// printf ( "********** Entering VerifyUsedIPAdd **********\n" );
	if ( Used_IP == NULL ) {
		return	0;	// The used IP address is invalid.
	} else if ( strlen ( Used_IP ) > strlen ( "xxx.xxx.xxx.xxx" ) ) {
		return	0;	// The used IP address is invalid.
	} else if ( strlen ( Used_IP ) < strlen ( "x.x.x.x" ) ) {
		return	0;	// The used IP address is invalid.
	} else {
		memset ( TempBuff , '\0' , 32 );
		strcpy ( TempBuff , Used_IP );
		Used_IP = TempBuff;
		for ( i = 0 ; i < 32 ; i++ ) {
			if ( *( TempBuff + i ) == '.' ) {
				*( TempBuff + i ) = ' ';
			}
		}
		sscanf ( Used_IP , "%d %d %d %d" , &Used_IP_1 , &Used_IP_2 , &Used_IP_3 , &Used_IP_4 );
	}
	// printf ( "********** Used_IP = %d.%d.%d.%d **********\n" , Used_IP_1 , Used_IP_2 , Used_IP_3 , Used_IP_4 );

	lan_ipaddr = nvram_get ( "lan_ipaddr" );
	if ( lan_ipaddr == NULL ) {
		return	0;	// The used IP address is invalid.
	} else if ( strlen ( lan_ipaddr ) > strlen ( "xxx.xxx.xxx.xxx" ) ) {
		return	0;	// The used IP address is invalid.
	} else if ( strlen ( lan_ipaddr ) < strlen ( "x.x.x.x" ) ) {
		return	0;	// The used IP address is invalid.
	} else {
		memset ( TempBuff , '\0' , 32 );
		strcpy ( TempBuff , lan_ipaddr );
		lan_ipaddr = TempBuff;
		for ( i = 0 ; i < 32 ; i++ ) {
			if ( *( TempBuff + i ) == '.' ) {
				*( TempBuff + i ) = ' ';
			}
		}
		sscanf ( lan_ipaddr , "%d %d %d %d" , &LAN_IP_1 , &LAN_IP_2 , &LAN_IP_3 , &LAN_IP_4 );
	}
	// printf ( "********** lan_ipaddr = %d.%d.%d.%d **********\n" , LAN_IP_1 , LAN_IP_2 , LAN_IP_3 , LAN_IP_4 );

	lan_netmask = nvram_get ( "lan_netmask" );
	if ( lan_netmask == NULL ) {
		return	0;	// The used IP address is invalid.
	} else if ( strlen ( lan_netmask ) > strlen ( "xxx.xxx.xxx.xxx" ) ) {
		return	0;	// The used IP address is invalid.
	} else if ( strlen ( lan_netmask ) < strlen ( "x.x.x.x" ) ) {
		return	0;	// The used IP address is invalid.
	} else {
		memset ( TempBuff , '\0' , 32 );
		strcpy ( TempBuff , lan_netmask );
		lan_netmask = TempBuff;
		for ( i = 0 ; i < 32 ; i++ ) {
			if ( *( TempBuff + i ) == '.' ) {
				*( TempBuff + i ) = ' ';
			}
		}
		sscanf ( lan_netmask , "%d %d %d %d" , &MASK_1 , &MASK_2 , &MASK_3 , &MASK_4 );
	}
	// printf ( "********** lan_netmask = %d.%d.%d.%d **********\n" , MASK_1 , MASK_2 , MASK_3 , MASK_4 );

	if ( ( MASK_1 == 0 ) && ( MASK_2 == 0 ) && ( MASK_3 == 0 ) && ( MASK_4 == 0 ) ) {
		// printf ( "++++++++++ 1 ++++++++++\n" );
		return	0;	// The used IP address is invalid.
	} else if ( ( MASK_1 == 255 ) && ( MASK_2 == 0 ) && ( MASK_3 == 0 ) && ( MASK_4 == 0 ) ) {
		if ( Used_IP_1 == LAN_IP_1 ) {
			// printf ( "++++++++++ 2 ++++++++++\n" );
			return	1;	// The used IP address is valid.
		} else {
			// printf ( "++++++++++ 3 ++++++++++\n" );
			return	0;	// The used IP address is invalid.
		}
	} else if ( ( MASK_1 == 255 ) && ( MASK_2 == 255 ) && ( MASK_3 == 0 ) && ( MASK_4 == 0 ) ) {
		if ( ( Used_IP_1 == LAN_IP_1 ) && ( Used_IP_2 == LAN_IP_2 ) ) {
			// printf ( "++++++++++ 4 ++++++++++\n" );
			return	1;	// The used IP address is valid.
		} else {
			// printf ( "++++++++++ 5 ++++++++++\n" );
			return	0;	// The used IP address is invalid.
		}
	} else if ( ( MASK_1 == 255 ) && ( MASK_2 == 255 ) && ( MASK_3 == 255 ) && ( MASK_4 == 0 ) ) {
		if ( ( Used_IP_1 == LAN_IP_1 ) && ( Used_IP_2 == LAN_IP_2 ) && ( Used_IP_3 == LAN_IP_3 ) ) {
			// printf ( "++++++++++ 6 ++++++++++\n" );
			return	1;	// The used IP address is valid.
		} else {
			// printf ( "++++++++++ 7 ++++++++++\n" );
			return	0;	// The used IP address is invalid.
		}
	} else {
		// printf ( "++++++++++ 8 ++++++++++\n" );
		return	0;	// The used IP address is invalid.
	}
}
#endif
/*************************************************************************************/

#define		Dump_DHCP_Lease_Table_H_Extra_SRCs_001		dump_leasefile();

#endif		// end of _Dump_DHCP_Lease_Table_H_
