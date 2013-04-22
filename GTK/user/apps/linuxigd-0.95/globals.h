#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include <net/if.h>

struct GLOBALS {
  char extInterfaceName[IFNAMSIZ]; 	// The name of the external interface, picked up from the
                                   	// command line
  char intInterfaceName[IFNAMSIZ]; 	// The name of the internal interface, picked from command line

  // All vars below are read from /etc/upnpd.conf in main.c
  int debug;  											// 1 - print debug messages to syslog
               											// 0 - no debug messages
  char iptables[50];  							// The full name and path of the iptables executable, used in pmlist.c
  char upstreamBitrate[12];  				// The upstream bitrate reported by the daemon
  char downstreamBitrate[12]; 			// The downstream bitrate reported by the daemon
  char forwardChainName[20];  			// The name of the iptables chain to put FORWARD rules in
  char preroutingChainName[20];			// The name of the chain to put PREROUTING rules in
  int  forwardRules;     						// 1 - forward rules are inserted
                          					// 0 - no forward rules inserted
  long int duration;    						// 0 - no duration
                          					// >0 - duration in seconds
                         						// <0 - expiration time 
  char descDocName[20];
  char xmlPath[50];

// +++ Gemtek, Add flag for user configuration
  char device_uuid[48];  						// The service uuid, it will be generated automatically based on MAC address
	int configureAllowed;							// allow user to configure
	int disableAllowed;								// allow user to disable wan
	
// --- Gemtek, Add flag for user configuration  
  
};

typedef struct GLOBALS* globals_p;
extern struct GLOBALS g_vars;

#define CONF_FILE 													"/var/linuxigd/upnpd.conf"
#define MAX_CONFIG_LINE 										256
#define IPTABLES_DEFAULT_FORWARD_CHAIN 			"FORWARD"
#define IPTABLES_DEFAULT_PREROUTING_CHAIN 	"PREROUTING"
#define DEFAULT_DURATION 										0
#define DEFAULT_UPSTREAM_BITRATE 						"0"
#define DEFAULT_DOWNSTREAM_BITRATE 					"0"
#define DESC_DOC_DEFAULT 										"gatedesc.xml"
#define XML_PATH_DEFAULT 										"/var/linuxigd"

#endif // _GLOBALS_H_
