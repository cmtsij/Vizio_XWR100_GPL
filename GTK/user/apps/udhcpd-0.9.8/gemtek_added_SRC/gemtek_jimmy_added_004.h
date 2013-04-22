#ifndef	_Gemtek_Jimmy_Added_004_H_

#define	_Gemtek_Jimmy_Added_004_H_

#include  <bcmnvram.h>

#define	Gemtek_Jimmy_Added_004_Extra_SRC_001		if (client_config.ifindex==0) { \
													read_interface(client_config.interface, &client_config.ifindex,  \
			   										NULL, client_config.arp);  \
												}

//Added by ChungLiang for checking NULL string
#define	Gemtek_Jimmy_Added_004_Extra_SRC_002		if (!string) return 0;

/* 
 * Checking whether it is Router's LAN IP Address
 * Added by PeterChen , Gemtek , 2003-0210 PM 03:24
 */
#define	Gemtek_Jimmy_Added_004_Extra_SRC_003		char	*lan_ipaddr = NULL , BaBaTempSpace [ 17 ]; \
char	IsLanIPAddr ( u_int32_t addr ) \
{ \
	struct in_addr	temp; \
	lan_ipaddr = nvram_safe_get ( "lan_ipaddr" ); \
	if ( lan_ipaddr == NULL ) { \
		if ( ( lan_ipaddr = nvram_safe_get ( "lan_ipaddr" ) ) == NULL ) { \
			; \
		} else { \
			memset ( &BaBaTempSpace , '\0' , 17 ); \
			memcpy ( &BaBaTempSpace , lan_ipaddr , 17 ); \
			lan_ipaddr = BaBaTempSpace; \
			goto	CompareLANIP; \
		} \
	} else { \
CompareLANIP:	temp.s_addr = addr; \
		if ( !strcmp ( lan_ipaddr , inet_ntoa ( temp ) ) ) { \
			add_lease ( blank_chaddr , addr , server_config.conflict_time ); \
			return	1; \
		} \
	} \
	return	0; \
}

/* Checking whether it is Router's LAN IP Address , Added by PeterChen , Gemtek , 2003-0210 PM 03:24 */
#define	Gemtek_Jimmy_Added_004_Extra_SRC_004		if ( IsLanIPAddr ( addr ) == 1 ) { \
													add_lease ( blank_chaddr , addr , server_config.conflict_time ); \
													return	1; \
												}

#define	Empty_IP_Addr	"0.0.0.0"

// adding this mechansim for ignoring the situation that the requested IP is 0.0.0.0
#define	Gemtek_Jimmy_Added_004_Extra_SRC_005		struct in_addr		addr; \
												memset ( &addr , '\0' , sizeof ( struct in_addr ) ); \
												addr.s_addr = yiaddr; \
												LOG ( LOG_INFO , "add_lease ... XXX ... yiaddr = %s#\n" , inet_ntoa ( addr ) ); \
												if ( strncmp ( inet_ntoa ( addr ) , Empty_IP_Addr , strlen ( Empty_IP_Addr ) ) == 0 ) { \
													LOG ( LOG_INFO , "add_lease ... yiaddr = %s ... return NULL\n" , inet_ntoa ( addr ) ); \
													return	NULL; \
												}


#endif	// end of _Gemtek_Jimmy_Added_004_H_