
#ifndef __RC_ACTION_H__
#define __RC_ACTION_H__

#include <nvram.h>
#include <sys/types.h>
#include <signal.h>

enum {
	GMTK_SYSTEM_RESTART = 1001,
	GMTK_SYSTEM_STOP,
	GMTK_SYSTEM_START,
	GMTK_SYSTEM_REBOOT,
	GMTK_SYSTEM_UPGRADE_REBOOT,	
	GMTK_SYSTEM_DEFAULT,
	GMTK_RESTART_WIRELESS,
	GMTK_RESTART_WIRELESS_WPS,
	GMTK_RESTART_IPTABLE,
	GMTK_RESTART_DDNS,
	GMTK_RESTART_IPTABLE_TPROXY,
	GMTK_RESTART_CONNLOG,
	GMTK_RESTART_ADVROUTE,
	GMTK_RESTART_SAMBA,
	GMTK_RESTART_PROFTPD,
	GMTK_RESTART_STORAGE_MEDIA,
	GMTK_RESTART_USB,
	GMTK_RESTART_ADMIN,
	GMTK_RESTART_DHCP = 1019,
	GMTK_RESTART_QOS,
	GMTK_START_TIMER,
	GMTK_STOP_TIMER,
	GMTK_START_MON,
	GMTK_STOP_MON,
	GMTK_RESTART_ROUTING,
//Tony 2008/1/8 ====================================================================	
	GMTK_RESTART_TIMEZONE,
	GMTK_RESTART_ROUTER_SETTINGS,
	GMTK_REBOOT_HNAP,
//Tony 2008/1/8 end====================================================================	
	GMTK_SYSTEM_RESTART_LAN,
	GMTK_SYSTEM_RESTART_SETUP,
	GMTK_SYSTEM_RESTART_WL_MAC_FILTER,
	GMTK_RESTART_EDIT_INTERNET,
};

#define RC_ACTION					"rc_action"

#define RC_ACTION_TRIGGER()	kill(1, SIGHUP)
#define RC_ACTION_CLEAN()		nvram_set(RC_ACTION, "0")
#define RC_ACTION_GET() 		atoi(nvram_safe_get(RC_ACTION))
#define RC_ACTION_SET(x) 	{\
	char rc_action[16];\
	sprintf(rc_action, "%d", x);\
	nvram_set(RC_ACTION, rc_action);\
}

#endif // __RC_ACTION_H__
