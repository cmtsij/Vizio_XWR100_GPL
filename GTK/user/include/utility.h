
#ifndef __GTK_UTILITY_H__
#define __GTK_UTILITY_H__

/*
Defines for LED control
*/
#define RESET_BUTTON      3
#define WLSEC_BUTTON			8
#define WATCHDOG_ENABLED	2					//	Watch Dog enabled PIN
#define WATCHDOG_CLK			7				//	Watch Dog clock PIN
#define WLAN							0
#define RES_REBOOT	1
#define RES_DEFAULT	2
#define ON  1
#define OFF 0
//#define ON  0
//#define OFF 1
#define BLINK  1
#define NORMAL   2
#define POWER_LED		1
#define WSC_OLED_GPIO		5
//#define WSC_GLED_GPIO		4

#define ETHERADDR_LEN	6

#define eval_script(cmd, args...) ({					\
	char *argv[] = { cmd, ## args, NULL };			\
	sysinit_script(argv);		\
})
  
/*
 * Convert Ethernet address string representation to binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @param	e	binary data
 * @return	1 if conversion was successful and 0 otherwise
 */
int GMTK_ether_atoe(const char *a, unsigned char *e);


/*
 * Convert Ethernet address binary data to string representation
 * @param	e	binary data
 * @param	a	string in xx:xx:xx:xx:xx:xx notation
 * @return	a
 */
char *GMTK_ether_etoa(const unsigned char *e, char *a);


/* Get Mac address by search ARP Table
 * @param ip : ip string (192.168.1.1)
 * @param ifn: interface name (eth0, eth1, br0)
 * @param mac: NULL, if there is no matched information in ARP table
 	             otherwise, return the pointer.
 *  return 1: found, 0: not found, others : error	             
 */
int GMTK_GetMacByIP(const char *ip,const char *ifn, char *mac);

/* Get interface number form bridge by search target MAC address
 * @param mac : mac string 
 * @param ifn : interface name (br0)
 * @param no  : the interface port number of bridge
 *  return 1: found, 0: not found, others : error	             
 */
int GMTK_GetBrIfNumByMac(const char *mac,const char *ifn,int *no);

/* Get ip address of the interface name
 * @param ifname : interface name (eth0, br0) 
 * @param ip		 : interface name (br0)
 *  return 0: success, 
 		 others : error	             
 */
int GMTK_GetIPbyIFName(const char *ifname, char *ip);

/* Check if the ip address is private ipaddress
 * 
 * @param ip  : ip string (format, x.x.x.x) 
 *  return 0	: it is not private ip address 
 *	  		 1	: A-class ip address, 
 *				 2	: B-class ip address,
 *				 3	: C-class ip address, 
 *	 others 	: error	             
 */
int GMTK_IsPrivateIP(const char *ip);

/* Check if the interface has been added into bridge
 * @param ifn : interface name (eth0)
 * @param brn : bridge interface name (br0)
 *  return 1: yes, 0: no	             
 */
int GMTK_IsBridgeIF(const char *ifn, const char *brn);


/*
 * Send signal to process whose PID is stored in plaintext in pidfile
 * @param	pidfile : PID file
 * @param sig			: singal number
 * @return	0 on success and errno on failure
 */
int GMTK_Kill_PIDFile(const char *pidfile, const int sig);

/* Get valid ip address range by lan ip address and lan netmask
 * assume lan netmask format is 255.255.255.XXX
 * @param lan_ip 		: (input)	 LAN IP Address
 * @param lan_nm 		: (input)	 LAN Netmask
 * @param start_ip	: (output) Start IP address
 * @param end_ip	 	: (output) End IP address	
 *  return 0: success, 
 *	 others : error	             
 */
int GMTK_GetIPRangebyIPNM(const char *lan_ip, const char *lan_nm, int *start_ip, int *end_ip);

/*
 * Check the input string to see if  there are some special characters which should be escaped. 
 * Output string will store the final string which will contain the escape character if need.
 * Right now, it only check " and ' 
 *
 * @param	in 			: input string
 * @param in_size	: input string length
 * @param out 		: output string
 * @param out_size: output string length	
 * @return 
 *		0 	:	on success and errno on failure
 *		-1	: in_size is bigger than out_size
 *		-2	: the size of out string is too big and it will over write memory space	
 *
 */
int GMTK_convert_character(const char *in, int in_size, char *out, int out_size);


/* Check if the dst and src address is located at the same subnet
 * 
 * @param dst_ip  : destination ip address
 * @param src_ip  : source ip address
 * @param nm_ip 	: netmask	
 *  return 0			: false
 *	  		 1			: true
 */
int GMTK_IsSameSubnet(const char *dst_ip, const char *src_ip, const char *nm_ip);

/*
Control LED with GPIOs.
* @param command : LED commands , now support BLINK , NORMAL
* @param pin     : GPIO pin number 
* @param action  : action for the command , ON,OFF
*/
int led_ctrl( int command , int pin , int action );

int sysinit_script(char *const argv[]);

int run_script();
#endif // __GTK_UTILITY_H__
