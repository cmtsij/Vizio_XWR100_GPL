/* gemtek_jimmy_added_003.h
 *
 * Jimmy_Ma <Jimmy_Ma@gemtek.com.tw> 
 *
 * Added for e2_etoa to change the ETHERNET MAC Address to birnary. 20071116
 * 
 * Copyrightc 2008 Gemtek Technology Co., Ltd.  All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef	_Gemtek_Jimmy_Added_003_H_

#define	_Gemtek_Jimmy_Added_003_H_

#include <bcmnvram.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include "../dhcpd.h"

/*****************************************************************************
 * lock a file.
 *
 * This function blocks until the lock is given to this process.
 *
 * @param fp an open file pointer
 * @param mode one of LOCK_SH or LOCK_EX (see flock manpage).
 *
 ****************************************************************************/
static void cache_lock( FILE * fp, int mode ) {
	int fd = -1;
 	fd = fileno(fp);

	//debug("cache_lock(%d)\n", fd);

	if( fd < 0 ) {
		//debug("cache_lock(): can not get the descriptor for a file");
		return;
	}

	flock( fd , mode );
}

/*****************************************************************************
 * unlock a file.
 *
 * This function releases a lock for this file.
 *
 * @param fp an open file pointer
 *
 ****************************************************************************/
static void cache_unlock( FILE * fp ) {
	int fd = -1;

 	fd = fileno(fp);
	//debug("cache_unlock(%d)\n", fd );

	if( fd < 0 ) {
		//debug("cache_unlock(): can not get the descriptor for a file");
		return;
	}

	/** flush data to the file */
	fflush(fp);

	flock( fd , LOCK_UN);
}

/*    added by Jimmy Ma 20030924 */

#define ETHER_ADDR_LEN	6

char *e2_etoa ( const unsigned char *n )
{
	static char	buf [ ETHER_ADDR_LEN * 3 ];
	char		*c = buf;
	int		i = 0;

	if ( n == NULL ) { return NULL; }

	memset ( buf , '\0' , ETHER_ADDR_LEN * 3 );

	for ( i = 0 ; i < ETHER_ADDR_LEN ; i++ ) {
		if ( i ) {
			*c++ = ':';
		}

		c += sprintf ( c , "%02X" , n[i] & 0xff );
	}

	return	buf;
}


void write_hosts(void)
{
	FILE *fp;
	unsigned int i;
	char buf[255];
	
	char	MACStr[19];
	char    IPStr[19];
	char	DomainStr[255];
	char    HostStr[255];
	struct	in_addr	addr;
	
	time_t curr = sys_time();//time(0);
	unsigned long lease_time;
	
	
	
	
	strcpy(DomainStr, nvram_safe_get("wan_domain"));
	if (strlen(DomainStr) == 0)
		strcpy(DomainStr, "domain.com");
		
	strcpy(HostStr, nvram_safe_get("wan_hostname"));	
	if (strlen(HostStr) == 0)
		strcpy(HostStr, "WL");		
		
//	strcpy(MACStr, nvram_safe_get("et0macaddr"));
	strcpy(MACStr, nvram_safe_get("lan_mac"));
	strcpy(IPStr, nvram_safe_get("lan_ipaddr"));	
			
	
	
	
	if (!(fp = fopen("/tmp/hosts", "w"))) {
		LOG(LOG_ERR, "Unable to open %s for writing", "/tmp/hosts");
		return;
	}
	
	
	cache_lock(fp, LOCK_SH);
	
	// sprintf(buf, "%s %s.%s %s#%s#", IPStr, HostStr, DomainStr, HostStr, MACStr);	
	sprintf(buf, "%s %s.%s %s#%s", IPStr, HostStr, DomainStr, HostStr, MACStr);	
	fputs(buf, fp);
	fputs("\n", fp);	
	
	for (i = 0; i < server_config.max_leases; i++) {
		if (leases[i].yiaddr != 0) {
		
			// MAC address			
			strcpy(MACStr, e2_etoa (leases[i].chaddr));
			// IP address
			addr.s_addr = leases[i].yiaddr;
			strcpy(IPStr, inet_ntoa(addr));
			
			
			// sprintf(buf, "%s %s.%s %s#%s#", IPStr, leases[i].hostname, DomainStr, leases[i].hostname, MACStr);
			sprintf(buf, "%s %s.%s %s#%s", IPStr, leases[i].hostname, DomainStr, leases[i].hostname, MACStr);
					
			fputs(buf, fp);
			fputs("\n", fp);
		}
	}
	
	cache_unlock(fp);
	fclose(fp);

}

#define	Jimmy_Added_003_Extra_SRC_001	int	fd;

#define	Jimmy_Added_003_Extra_SRC_002	fd = fileno(fp);

#define	Jimmy_Added_003_Extra_SRC_003	flock(fd , LOCK_EX);

#define	Jimmy_Added_003_Extra_SRC_004	flock(fd , LOCK_UN);

// We don't want that the end users see any static IP host infos, so we added the following extra SRCs
// The mechansim was done by PeterChen, Gemtek	2003-1231 PM 02:14
#define	Jimmy_Added_003_Extra_SRC_005	extern unsigned char blank_chaddr[]; \
										if ( memcmp ( leases[i].chaddr , blank_chaddr , 16 ) == 0 ) { \
											continue; \
										}

// 1*60*60*24*365*(2000-1970)=946080000
// The DHCP client gets lease from DHCP server ( Before NTP gets the Date/Time from NTP server ),
// then it causes some time difference issue, and these SRCs is done for this kind of issue.
// The mechansim was done by PeterChen, Gemtek	2003-1231 PM 03:47

//time(0) + server_config.offer_time; 
#define	Jimmy_Added_003_Extra_SRC_006	if ( ( leases[i].expires < 946080000 ) && ( time(0) > 946080000 ) ) { \
											leases[i].expires = sys_time() + server_config.offer_time; \
										}

#endif	// end of _Gemtek_Jimmy_Added_003_H_