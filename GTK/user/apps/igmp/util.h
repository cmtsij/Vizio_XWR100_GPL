
#define EQUAL(s1,s2)	(strcmp((s1),(s2)) == 0)
extern int log_level;

/*
 * Log support
 */
#ifdef DEBUG
#define LOG(x) debug x
#else
#define LOG(x)
#endif
//#define LOG_INFO	1
#define	LOG_DETAIL	2
//#define LOG_DEBUG	3

#if 0 
#define dbg_printf(fmt,args...) printf(fmt,##args)
#else
#define dbg_printf(fmt,args...) ;
#endif


typedef struct _interface_list_t {
    char ifl_name[IFNAMSIZ];
    struct sockaddr ifl_addr;
    struct _interface_list_t *ifl_next;
    short ifl_index; /* lo=1, eth0=2,usb=3,br0=4,nas=5 and etc*/
} interface_list_t;

unsigned short in_cksum(unsigned short *addr, int len);
interface_list_t* get_interface_list(short af, short flags, short unflags);
void free_interface_list(interface_list_t *ifl);
short get_interface_flags(char *ifname);
short set_interface_flags(char *ifname, short flags);
int get_interface_mtu(char *ifname);
int mrouter_onoff(int sockfd, int onoff);
