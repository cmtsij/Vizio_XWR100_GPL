/* support for multi-pppoe via dns setting --- japan request */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <syslog.h>

#include <bcmnvram.h>
#include <shutils.h>
#include <code_pattern.h>
#include <cy_conf.h>

#ifdef DEBUG_MPPPOE
#define printf(fmt, args...)	syslog(LOG_DEBUG, fmt, ##args);
#endif

//#define MPPPOE_DNAME	"hinet.net"
//#define MPPPOE_GW	"192.168.100.254"
//#define MPPPOE_IF	"eth1"
/*
   nvram need:  mpppoe_dname = domain name search string
		mpppoe_ifname= multi-pppoe interface
		mpppoe_gateway = multi-pppoe gateway
*/


struct rtrec {
        struct rtrec *prev,*next;
        unsigned char network[4];
        unsigned long ttl;
};

static struct rtrec *rtrec_head=NULL, *rtrec_tail=NULL;
static int rtrec_count=0;
void check_link();
void dump_link();
void remove_ttl();
struct rtrec *find_link();
void add_link();
void del_link();

void route_add_mpppoe ( char *name, struct in_addr *addr4, unsigned long now, unsigned long ttl) 
{
	char *dname = nvram_safe_get("mpppoe_dname");
	unsigned int namelen = strlen(name);
	unsigned int domainlen = strlen(dname);

	cprintf("get name %s, ip=%s \n", name, inet_ntoa(*addr4));
	if ( dname[0] && (strcmp(name + namelen - domainlen, dname) == 0)) {
		unsigned char network[4];

		strncpy(network,(unsigned char *)addr4,4);
		network[3]=0;		// bulid a network address

		cprintf("tallest:=====( Match key word !! name %s, ip=%s )=====\n", name, inet_ntoa(*addr4));

		check_link(network,now,ttl);
                dump_link();
		
		cprintf("11route add -net %s netmask 255.255.255.0 gw %s dev %s\n",
			inet_ntoa(*(struct in_addr *)network),nvram_safe_get("wan_gateway_1"),
			nvram_safe_get("wan_ifname_1"));
                eval ("route","add","-net",inet_ntoa(*(struct in_addr *)network),
			"netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
			"dev",nvram_safe_get("wan_ifname_1") );
	}
}
		

void check_link(unsigned char *network, unsigned long now, unsigned long ttl)
{
        struct rtrec *pt;

	//ttl = ttl + 7200;	//keep routing entry two hours. by tallest
        if ( (pt=find_link(network)) == NULL) { //a new network, create it
                add_link(network,now+ttl);

                cprintf("route add -net %s netmask 255.255.255.0 gw %s dev %s",
                        inet_ntoa(*(struct in_addr *)network),nvram_safe_get("wan_gateway_1"),
                        nvram_safe_get("wan_ifname_1"));
                eval ("route","add","-net",inet_ntoa(*(struct in_addr *)network),
                        "netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
                        "dev",nvram_safe_get("wan_ifname_1") );

        }
#if 0 // by tallest, routing entry deleted issue.
        else {  // a exist network, renew it
                del_link(pt);
                add_link(network,now+ttl > pt->ttl ? now+ttl : pt->ttl);
        }
        remove_ttl(now);
#endif
}



// remove link & route if ttl timeout

void remove_ttl(unsigned long now)
{
        struct rtrec *pt=rtrec_head,*tmp;
#if 1	
//===================  (Do not delete DNS's routing entry )by tallest =======================
	int dnsno = 0;
	char got_dns[3][20], *next;
	struct in_addr dnsip1;

	foreach(got_dns[dnsno], nvram_safe_get("wan_get_dns_1"), next){
		dnsno ++;
	}
	inet_aton(got_dns[0], &dnsip1);
	next =(char *)&dnsip1;
	next[3] = 0;

	while (pt) {
                if (pt->ttl < now && (memcmp(pt->network,(char *)&dnsip1,4))) {
                                                                                                                             
                        cprintf("route del -net %s netmask 255.255.255.0 gw %s dev %s\n",
                                inet_ntoa(*(struct in_addr *)pt->network),nvram_safe_get("wan_gateway_1"),
                                nvram_safe_get("wan_ifname_1"));
                                                                                                                             
                        eval ("route","del","-net",inet_ntoa(*(struct in_addr *)pt->network),
                                "netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
                                "dev",nvram_safe_get("wan_ifname_1") );
                                                                                                                             
                        tmp=pt->next;
                        del_link(pt);
                        pt=tmp;
                } else
                        pt= pt->next;
        }
//=============================================================================================
#else
        while (pt) {
                if (pt->ttl < now) {

               		printf("route del -net %s netmask 255.255.255.0 gw %s dev %s",
                        	inet_ntoa(*(struct in_addr *)pt->network),nvram_safe_get("wan_gateway_1"),
                        	nvram_safe_get("wan_ifname_1"));

                	eval ("route","del","-net",inet_ntoa(*(struct in_addr *)pt->network),
                        	"netmask","255.255.255.0","gw",nvram_safe_get("wan_gateway_1"),
                        	"dev",nvram_safe_get("wan_ifname_1") );

                        tmp=pt->next;
                        del_link(pt);
                        pt=tmp;
                } else
                        pt= pt->next;
        }
#endif
}


struct rtrec *find_link(unsigned char *network)
{
        struct rtrec *pt=rtrec_head;

        while (pt) {
                if ( !memcmp(pt->network,network,4)) {
                        return pt;
                } else
                        pt= pt->next;
        }
        return NULL;
}

void add_link(unsigned char *network, unsigned long timeout)
{

        struct rtrec *new;

        if ( (new=malloc(sizeof(struct rtrec))) == NULL ) {
                printf("malloc error when add_link!!! \n");
                return;
        }
        cprintf("add link... \n");
        strncpy(new->network,network,4);
        new->ttl=timeout;
        if (rtrec_head)
                rtrec_head->prev=new;
        new->next= rtrec_head;
        new->prev= NULL;
        rtrec_head= new;
        if (!rtrec_tail)
                rtrec_tail=new;

        rtrec_count++;
}

void del_link (struct rtrec *del)
{

        if (!del)
                return;

        if (del->prev)
                del->prev->next = del->next;
        else
                rtrec_head=del->next;

        if (del->next)
                del->next->prev = del->prev;
        else
                rtrec_tail=del->prev;

        free(del);

        rtrec_count--;
}

void dump_link ()
{

        struct rtrec *pt;
        printf("-----dump table-----\n");
        for (pt=rtrec_head; pt ; pt=pt->next) {
                printf("%s %lu %s\n",inet_ntoa(*(struct in_addr *)pt->network),
                                        pt->ttl,ctime(&(pt->ttl)) );
        }
        printf("=========%d=========\n", rtrec_count);
}



