#ifndef	_Gemtek_Jimmy_Added_002_H_

#define	_Gemtek_Jimmy_Added_002_H_

#define	GemtekAddedSRC

#include <bcmnvram.h>
#include "EventLog.h"

#define	SomeVariableDefinition_001	char 	buf[255];		\
								char	MACStr[19];	\
								char    IPStr[19];	\
								char	DomainStr[255];	\
								struct	in_addr	addr;

/* added by Jimmy Ma 20030925 */
//+++ GTK December 5, 2007.
//Typically the domain name assigned by the WAN DHCP server is used, but if the domain name is manually set on the web UI, it has a higher priority.
/*
#define	SomeExtraCodes_001	strcpy(DomainStr, nvram_safe_get("wan_domain")); \
							if (strlen(DomainStr) == 0) \
								strcpy(DomainStr, "WL");
*/
#define	SomeExtraCodes_001	strcpy(DomainStr, nvram_safe_get("wan_domain"));				\
				if ( strlen(DomainStr) == 0 && strlen(nvram_safe_get("wan_domain_default")) )	\
					strcpy(DomainStr, nvram_safe_get("wan_domain_default"));		\
				else										\
					strcpy(DomainStr, "WL");
//--- GTK

/* added by Jimmy Ma 20030925 */
#define	SomeExtraCodes_002	strcpy(MACStr, e2_etoa (lease->chaddr)); \	
							addr.s_addr = lease->yiaddr; \
							strcpy(IPStr, inet_ntoa(addr)); \
							sprintf(buf, "%s%s %s.%s %s %s", GettingTimeStamp(), IPStr, lease->hostname, DomainStr, lease->hostname, MACStr); \
							write_eventlog(0, buf);

// added by Jimmy Ma 20030924
#define	SomeExtraCodes_003	
//#define	SomeExtraCodes_003	write_hosts();

// added by PeterChen, 2003-1205 PM 02:10
#define	SomeExtraCodes_004	remove(server_config.pidfile);

// added by PeterChen, 2003-1205 PM 02:10
#define	SomeExtraCodes_005	sendOffer(&packet);

// added by PeterChen, 2003-1205 PM 02:13
#define	SomeExtraCodes_006	else if ( server_id_align != server_config.server ) { \
								DEBUG ( LOG_INFO , "server_id_align != server_config.server\n" ); \
							} else if ( !requested ) { \
								DEBUG ( LOG_INFO , "!requested\n" ); \
								sendNAK ( &packet ); \
							} else if ( requested_align != lease->yiaddr ) { \
								DEBUG ( LOG_INFO , "requested_align != lease->yiaddr\n" ); \
								sendNAK ( &packet ); \
							} else { \
								DEBUG ( LOG_INFO , "SELECTING State ... Unkonwn situation\n" ); \
								sendNAK ( &packet ); \
							}

// added by PeterChen, 2003-1205 PM 02:32
#define	SomeExtraCodes_007	if ( server_id_align != server_config.server ) { \
								DEBUG ( LOG_INFO , "SELECTING State ... server_id_align != server_config.server\n" ); \
							} else if ( server_id_align == server_config.server ) { \
								DEBUG ( LOG_INFO , "SELECTING State ... server_id_align == server_config.server\n" ); \
								if ( ( ntohl ( requested_align ) < ntohl ( server_config.start ) ) || \
					     				( ntohl ( requested_align ) > ntohl ( server_config.end ) ) ) { \
									DEBUG ( LOG_INFO , "The requested_align is in the wrong domain" ); \
									sendNAK ( &packet ); \
								} else if ( requested ) { \
									if ( !add_lease ( packet.chaddr , requested_align , server_config.offer_time ) ) { \
										DEBUG ( LOG_INFO , "lease pool is full -- OFFER abandoned" ); \
										sendNAK ( &packet ); \
									} else { \
										DEBUG ( LOG_INFO , "lease pool is not full" ); \
										lease = find_lease_by_chaddr ( packet.chaddr ); \
										if ( lease ) { \
											DEBUG ( LOG_INFO , "No lease ... adding lease ... successful" ); \
											sendACK ( &packet , lease->yiaddr ); \
											if ( hostname ) { \
												bytes = hostname[-1]; \
												if (bytes >= (int) sizeof(lease->hostname)) \
												bytes = sizeof(lease->hostname) - 1; \
												strncpy(lease->hostname, hostname, bytes); \
												lease->hostname[bytes] = '\0'; \
											} else {  \
												lease->hostname[0] = '\0'; \
											} \
										} else { \
											DEBUG ( LOG_INFO , "No lease ... adding lease ... failed" ); \
											sendNAK ( &packet ); \
										} \
									} \
								} else { \
									sendNAK ( &packet ); \
								} \
							}

// added by PeterChen, 2003-1205 PM 02:35
#define	SomeExtraCodes_008	DEBUG ( LOG_INFO , "INIT-REBOOT State ...\n" );


// added by PeterChen, 2003-1205 PM 02:37
#define	SomeExtraCodes_009	if ( ( ntohl ( requested_align ) < ntohl ( server_config.start ) ) || \
							     ( ntohl ( requested_align ) > ntohl ( server_config.end ) ) ) { \
								DEBUG ( LOG_INFO , "The requested_align is in the wrong domain" ); \
								sendNAK ( &packet ); \
							} else if ( !add_lease ( packet.chaddr , requested_align , server_config.offer_time ) ) { \
									DEBUG ( LOG_INFO , "lease pool is full -- OFFER abandoned" ); \
									sendNAK ( &packet ); \
							} else { \
									DEBUG ( LOG_INFO , "lease pool is not full" ); \
									lease = find_lease_by_chaddr ( packet.chaddr ); \
									if ( lease ) { \
										DEBUG ( LOG_INFO , "No lease ... adding lease ... successful" ); \
										sendACK ( &packet , requested_align ); \
										if ( hostname ) { \
											bytes = hostname[-1]; \
											if (bytes >= (int) sizeof(lease->hostname)) \
											bytes = sizeof(lease->hostname) - 1; \
											strncpy(lease->hostname, hostname, bytes); \
											lease->hostname[bytes] = '\0'; \
										} else { \
											lease->hostname[0] = '\0'; \
										} \
									} else { \
										DEBUG ( LOG_INFO , "No lease ... adding lease ... failed" ); \
										sendNAK ( &packet ); \
									} \
							}

// added by PeterChen, 2003-1205 PM 02:40
#define	SomeExtraCodes_010	DEBUG ( LOG_INFO , "RENEWING or REBINDING State ...\n" );

// added by PeterChen, 2003-1205 PM 02:40
#define	SomeExtraCodes_011	if ( ( ntohl ( packet.ciaddr ) < ntohl ( server_config.start ) ) || \
							     ( ntohl ( packet.ciaddr ) > ntohl ( server_config.end ) ) ) { \
								DEBUG ( LOG_INFO , "The requested_align is in the wrong domain" ); \
								sendNAK ( &packet ); \
							} else if ( !add_lease ( packet.chaddr , packet.ciaddr , server_config.offer_time ) ) { \
									DEBUG ( LOG_INFO , "lease pool is full -- OFFER abandoned" ); \
									sendNAK(&packet); \
							} else { \
									DEBUG ( LOG_INFO , "lease pool is not full" ); \
									lease = find_lease_by_chaddr ( packet.chaddr ); \
									if ( lease ) { \
										DEBUG ( LOG_INFO , "No lease ... adding lease ... successful" ); \
										sendACK ( &packet , packet.ciaddr ); \
										if ( hostname ) { \
											bytes = hostname[-1]; \
											if (bytes >= (int) sizeof(lease->hostname)) \
											bytes = sizeof(lease->hostname) - 1; \
											strncpy(lease->hostname, hostname, bytes); \
											lease->hostname[bytes] = '\0'; \
										} else { \
											lease->hostname[0] = '\0'; \
										} \
									} else { \
										DEBUG ( LOG_INFO , "No lease ... adding lease ... failed" ); \
										sendNAK ( &packet ); \
									} \
							}

#endif	// end of _Gemtek_Jimmy_Added_002_H_