
#ifndef _rc_h_
#define _rc_h_

#include <netinet/in.h>

#ifdef	DEBUG
#define my_printf(str, args...) printf(str, ## args)
#else
#define	my_printf(str, args...)
#endif

#define eval_script(cmd, args...) ({					\
	char *argv[] = { cmd, ## args, NULL };			\
	sysinit_script(argv);		\
})

struct sta_info
{
	char mac[18];
	char mode[2];
	int rx_bytes;
	int tx_bytes;
	int rx_packets;
	int tx_packets;	
};

#define IFUP (IFF_UP | IFF_RUNNING | IFF_BROADCAST | IFF_MULTICAST)

#define sin_addr(s) (((struct sockaddr_in *)(s))->sin_addr)

/* ppp scripts */
extern int ipup_main(int argc, char **argv);
extern int ipdown_main(int argc, char **argv);
extern int ppp_ifunit(char *ifname);

/* http functions */
extern int http_get(const char *server, char *buf, size_t count, off_t offset);
extern int http_post(const char *server, char *buf, size_t count);
extern int http_stats(const char *url);

/* init */
extern int console_init(void);
extern pid_t run_shell(int timeout, int nowait);
extern void signal_init(void);
extern void fatal_signal(int sig);

/* interface */
extern int ifconfig(char *ifname, int flags, char *addr, char *netmask);
extern int route_add(char *name, int metric, char *dst, char *gateway, char *genmask);
extern int route_del(char *name, int metric, char *dst, char *gateway, char *genmask);
extern void config_loopback(void);
extern int start_vlan(void);
extern int stop_vlan(void);

/* network */
extern void start_lan(void);
extern void stop_lan(void);
extern void lan_up(char *ifname);
extern void lan_down(char *ifname);
//extern void start_wan(void);
extern void stop_wan(void);
extern void wan_up(char *ifname);
extern void wan_down(char *ifname);
extern int hotplug_net(void);
extern int hotplug_usb(void);
extern int wan_ifunit(char *ifname);
extern int wan_primary_ifunit(void);

/* services */
extern int psmon_main(int argc, char **argv);
extern int start_dhcpd(void);
extern int stop_dhcpd(void);
extern int start_dns(void);
extern int stop_dns(void);
extern int start_ntpc(void);
extern int stop_ntpc(void);
extern int start_nas(char *type);
extern int stop_nas(void);
extern int start_services(void);
extern int stop_services(void);
extern int start_httpd_lan(void);
extern int start_dhcpd(void);
extern int start_igmp(void);
extern int start_tftpd(void);
#ifdef SES
extern int start_ses(void);
#elif WPS
extern int start_lltd(void);
extern int start_wsc(void);
#endif
extern int start_hostapd(void);
extern int start_dns(void);
extern int start_wan_connect(void);
extern int start_conn_log(void);

/*psmon , wan_connect*/
extern int doCheck;
extern int doWanCheck;
extern int wan_started;
extern int wan_disconnected;
extern int wan_status;	
extern unsigned long int time_stamp, psmon_stamp;
extern int gpio_monitor(char action[2], char *path);

extern int pptp_first_wan_connect;	//added by oder

/* RC States */
enum {
	REBOOT,
	RESTORE_DEFAULT,
	RESTART,
	STOP,
	START,
	TIMER,
	IDLE,
};

/*	Gemtek */
void restore_factory();

#define BRI_IFNAME "br0"
#define LAN_IFNAME "eth0"
#define WAN_IFNAME "eth1"
#define WL_IFNAME "ath0"
#define WL1_IFNAME "ath1"
#define WL2_IFNAME "ath2"
#define LAN_MAC_NAME "et0macaddr"
#define WL_MAC_NAME "wl0_macaddr"
#define WL1_MAC_NAME "wl1_macaddr"
#define WL2_MAC_NAME "wl2_macaddr"
#define ETHER_ADDR_LEN 6

#endif /* _rc_h_ */
