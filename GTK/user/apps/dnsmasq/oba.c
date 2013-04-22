#include "dnsmasq.h"
#include <shutils.h>
#include <utils.h>
#include <bcmnvram.h>
#include <outbreak.h>

#if 0
#define dprintf(fmt, args...) cprintf("%s: " fmt, __FUNCTION__, ## args)
#else
#define dprintf(fmt, args...)
#endif

int
write_oba_dns(char *name, char *ip)
{
	FILE *fp;

	/* Write name and IP pair to file */ 
	if((fp = fopen(OBA_DNS_FILE, "a"))) {
		cprintf("Write \"%s %s\" to %s\n", name, ip, OBA_DNS_FILE);
		fprintf(fp, "%s	%s\n", name, ip);
		fclose(fp);
	}
	else {
		dprintf("Cann't open %s\n", OBA_DNS_FILE);
		return -1;
	}
	return 0;
}

int
insert_oba_firewall(char *query_name, struct all_addr *addr)
{
	char ip[INET_ADDRSTRLEN];
	char *except_ip = nvram_safe_get("symc_except_ip");
	char dnames[254], name[254], *next, *next1;
	char ports[254], port[10];
	char cmd[254];
	int ret;

	inet_ntop(AF_INET, addr, ip, INET_ADDRSTRLEN);
	dprintf("query_name[%s] ip[%s]\n", query_name, ip);

	if(!nvram_match("symc_outbreak", "1") ||
	   nvram_match("symc_except_ip", ""))	return 0;

	/* Check name and IP whether exist */
	ret = find_dns_ip_name(OBA_DNS_FILE, ip, query_name);

	if(ret) {
		dprintf("The name and ip is already exist\n");
		return 0;
	}

	/* Scan symc_except_ip table */
	foreach(dnames, except_ip, next) {
		/* Format: name:port1,port2,port3,.... */
		sscanf(dnames, "%[^:]:%s", name, ports);
		if(regmatch(name, query_name)) 
		{
			write_oba_dns(query_name, ip);
			_foreach(port, ports, next1, ",", ',') {
				memset(cmd,0,254);
				snprintf(cmd, sizeof(cmd), 
					"iptables -I oba -p tcp -m tcp --dport %s -d %s -j ACCEPT", port, ip);
				cprintf("cmd=[%s]\n", cmd);
				system(cmd);
				
				if (80 ==atoi(port) )
				{
					memset(cmd,0,254);
					snprintf(cmd, sizeof(cmd), 
						"iptables -t nat -I PREROUTING  -p tcp -m tcp --dport %s -d %s -j ACCEPT", port, ip);
					cprintf("cmd=[%s]\n", cmd);
					system(cmd);
				}
				
			}
			return 0;
		}
	}
	return 0;
}
