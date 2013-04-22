/* 
 * leases.c -- tools to manage DHCP leases 
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 */

#include <time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "debug.h"
#include "dhcpd.h"
#include "files.h"
#include "options.h"
#include "leases.h"
#include "arpping.h"
#include "./gemtek_added_SRC/gemtek_jimmy_added_004.h"

unsigned char blank_chaddr[] = {[0 ... 15] = 0};

extern int vizio_device;
/* clear every lease out that chaddr OR yiaddr matches and is nonzero */
void clear_lease(u_int8_t *chaddr, u_int32_t yiaddr)
{
	unsigned int i, j;
	char buf[128];
	struct in_addr clear_ip;
	
	for (j = 0; j < 16 && !chaddr[j]; j++);
	
	for (i = 0; i < server_config.max_leases; i++)
		if ((j != 16 && !memcmp(leases[i].chaddr, chaddr, 16)) ||
		    (yiaddr && leases[i].yiaddr == yiaddr)) {
			memset(&(leases[i]), 0, sizeof(struct dhcpOfferedAddr));
		}
	
	clear_ip.s_addr = yiaddr;
	sprintf(buf, "/bin/iptables -t filter -D LAN_FLOOD -i br0 -o vlan2 -s %s -p udp -j ExecutingAllUDPFlood", inet_ntoa(clear_ip));
	system(buf);
	sprintf(buf, "/bin/iptables -t filter -D LAN_FLOOD -i br0 -o vlan2 -s %s -p tcp --tcp-flags SYN SYN -j ExecutingAllUDPFlood", inet_ntoa(clear_ip));
	system(buf);
}


/* add a lease into the table, clearing out any old ones */
struct dhcpOfferedAddr *add_lease(u_int8_t *chaddr, u_int32_t yiaddr, unsigned long lease)
{
	char buf[128];
	struct in_addr client_ip;
	struct dhcpOfferedAddr *oldest;

	Gemtek_Jimmy_Added_004_Extra_SRC_005
	/* clean out any old ones */
	clear_lease(chaddr, yiaddr);
		
	oldest = oldest_expired_lease();
	if (oldest) {
		memcpy(oldest->chaddr, chaddr, 16);
		oldest->yiaddr = yiaddr;
		oldest->expires = sys_time() + lease;//time(0) + lease;
	}
	client_ip.s_addr = yiaddr;
	fprintf(stderr, "lease is %ld yiaddr is %s\n", lease, inet_ntoa(client_ip));
	sprintf(buf, "/bin/iptables -t filter -D LAN_FLOOD -i br0 -s %s -p udp -j ExecutingAllUDPFlood", inet_ntoa(client_ip));
	system(buf);
	sprintf(buf, "/bin/iptables -t filter -D LAN_FLOOD -i br0 -s %s -p tcp --dport ! 80 --tcp-flags SYN SYN -j ExecutingAllUDPFlood", inet_ntoa(client_ip));
	system(buf);
	sprintf(buf, "/bin/iptables -t filter -A LAN_FLOOD -i br0 -s %s -p udp -j ExecutingAllUDPFlood", inet_ntoa(client_ip));
	system(buf);
	sprintf(buf, "/bin/iptables -t filter -A LAN_FLOOD -i br0 -s %s -p tcp --dport ! 80 --tcp-flags SYN SYN -j ExecutingAllUDPFlood", inet_ntoa(client_ip));
	system(buf);

	if(vizio_device == 1) {
		sprintf(buf, "/bin/vizio_qos add_device vizio_device %s &\n", inet_ntoa(client_ip));
		system(buf);		
		vizio_device = 0;
	}
	else {
		sprintf(buf, "/bin/vizio_qos add_device %02X:%02X:%02X:%02X:%02X:%02X %s &\n", chaddr[0], chaddr[1], chaddr[2], chaddr[3], chaddr[4], chaddr[5], inet_ntoa(client_ip));
		system(buf);
	}
	return oldest;
}


/* true if a lease has expired */
int lease_expired(struct dhcpOfferedAddr *lease)
{
	return (lease->expires < (unsigned long) sys_time());//(unsigned long) time(0));
}	


/* Find the oldest expired lease, NULL if there are no expired leases */
struct dhcpOfferedAddr *oldest_expired_lease(void)
{
	struct dhcpOfferedAddr *oldest = NULL;
	unsigned long oldest_lease = sys_time();//time(0);
	unsigned int i;

	
	for (i = 0; i < server_config.max_leases; i++)
		if (oldest_lease > leases[i].expires) {
			oldest_lease = leases[i].expires;
			oldest = &(leases[i]);
		}
	return oldest;
		
}


/* Find the first lease that matches yiaddr, NULL is no match */
struct dhcpOfferedAddr *find_lease_by_yiaddr(u_int32_t yiaddr)
{
	unsigned int i;

	for (i = 0; i < server_config.max_leases; i++)
		if (leases[i].yiaddr == yiaddr) return &(leases[i]);
	
	return NULL;
}

Gemtek_Jimmy_Added_004_Extra_SRC_003

/* find an assignable address, it check_expired is true, we check all the expired leases as well.
 * Maybe this should try expired leases by age... */
u_int32_t find_address(int check_expired) 
{
	u_int32_t addr, ret;
	struct dhcpOfferedAddr *lease = NULL;		

	addr = ntohl(server_config.start); /* addr is in host order here */
	for (;addr <= ntohl(server_config.end); addr++) {

		/* ie, 192.168.55.0 */
		if (!(addr & 0xFF)) continue;

		/* ie, 192.168.55.255 */
		if ((addr & 0xFF) == 0xFF) continue;

		/* Same as Lan ip ? */
		if( IsLanIPAddr ( addr ) == 1)
			continue;
		
		/* lease is not taken */
		ret = htonl(addr);
		if ((!(lease = find_lease_by_yiaddr(ret)) ||

		     /* or it expired and we are checking for expired leases */
		     (check_expired  && lease_expired(lease))) &&

		     /* and it isn't on the network */
	    	     !check_ip(ret)) {
			return ret;
			break;
		}
	}
	return 0;
}



/* check is an IP is taken, if it is, add it to the lease table */
int check_ip(u_int32_t addr)
{
	struct in_addr temp;
	
	Gemtek_Jimmy_Added_004_Extra_SRC_004	
	if (arpping(addr, server_config.server, server_config.arp, server_config.interface) == 0) {
		temp.s_addr = addr;
	 	LOG(LOG_INFO, "%s belongs to someone, reserving it for %ld seconds", 
	 		inet_ntoa(temp), server_config.conflict_time);
		add_lease(blank_chaddr, addr, server_config.conflict_time);
		return 1;
	} else return 0;
}

///* Find the first lease that matches chaddr, NULL if no match */
//struct dhcpOfferedAddr *find_lease_by_chaddr(u_int8_t *chaddr)
//{
//	unsigned int i;
//
//	for (i = 0; i < server_config.max_leases; i++)
//		//if (!memcmp(leases[i].chaddr, chaddr, 16)) return &(leases[i]);
//		if (!memcmp(leases[i].chaddr, chaddr, 6)) return &(leases[i]);
//	
//	return NULL;
//}

/* Find the first lease that matches chaddr, NULL if no match */
struct dhcpOfferedAddr *find_lease_by_chaddr(u_int8_t *chaddr)
{
	unsigned int i;

	for (i = 0; i < server_config.max_leases; i++)
	{
		//if (!memcmp(leases[i].chaddr, chaddr, 16)) return &(leases[i]);
		if (!memcmp(leases[i].chaddr, chaddr, 6))
		{
			if ( IsLanIPAddr ( leases[i].yiaddr ) == 1 ) 
			{ 
				fprintf(stderr, "find_lease_by_chaddr : This is LAN IP...\n");
				return NULL;
			}
			else
			{
				//fprintf(stderr, "find_lease_by_chaddr : This is not LAN IP...\n");
				return &(leases[i]);
			}
		}
	}
	
	return NULL;
}

int update_lease_by_new_ip()
{
	unsigned int i , svr_ip , svr_mask , newyiaddr ;	
	struct in_addr mask;
	
	svr_ip = server_config.server;
	inet_aton( nvram_safe_get("lan_netmask"),&mask ); 
	svr_mask = mask.s_addr;
	svr_ip = svr_ip & svr_mask;	

	DEBUG(LOG_INFO, "svr_ip = 0x%08x" , svr_ip);
	fprintf(stderr, "svr_ip = 0x%08x" , svr_ip);

	for (i = 0; i < server_config.max_leases; i++)
	{	
		   if( leases[i].yiaddr != 0 )
		   {
		   	   newyiaddr = leases[i].yiaddr & ~svr_mask;
		   	   leases[i].yiaddr = svr_ip | newyiaddr;
						DEBUG(LOG_INFO, "leases[i].yiaddr = 0x%08x" , leases[i].yiaddr);
		   }
	}	
	return NULL;
}