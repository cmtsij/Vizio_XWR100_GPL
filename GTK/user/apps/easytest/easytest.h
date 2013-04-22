#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <linux/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include <nvram.h>
#include <utility.h>
#include <rc_action.h>

#if 1
#define dbg_printf(fmt, args...) printf("(%s->%s(%d)) " fmt "\n", __FILE__,__FUNCTION__,__LINE__,##args)
#else
#define dbg_printf(fmt, args...)
#endif


#define MULTICAST_PORT	"14677"
#define MULTICAST_IP	"234.2.2.7"

#define MAX_BUFFER_SIZE 1024

typedef struct SERVER_MAC_INFO {
  unsigned char lan_mac[6];
  unsigned char wan_mac[6];
  unsigned char g_mac[6];
  unsigned char a_mac[6];	
}	mac_info_t;

typedef struct SERVER_SSID_CHANNEL_INFO {
	unsigned char g_SSID[50];	// 0x00 --> N/A
	unsigned char a_SSID[50];	// 0x00 --> N/A
	unsigned char g_Channel;	// 0x00 --> auto or N/A   
	unsigned char a_Channel;	// 0x00 --> auto or N/A
	unsigned char g_SSID_2[50]; // 0x00 --> Ignore
	unsigned char a_SSID_2[50]; // 0x00 --> Ignore
	unsigned char g_SSID_3[50]; // 0x00 --> Ignore
	unsigned char a_SSID_3[50]; // 0x00 --> Ignore
	unsigned char reserve[100];   // default by zero(0x00) ( don't care )    
}	ssid_info_t;

typedef	struct SERVER_COUNTRY_CODE_INFO
{
	unsigned char G_Band[6];	//Null-terminated string
	unsigned char A_Band[6];	//Null-terminated string
}	country_code_info_t;

typedef struct _EXTENDED_UNIT_INFO
{
	unsigned char VID[30];
	unsigned char PID[30];
	unsigned char Others[100];
}	extended_unit_info_t;

typedef struct WlanSecurityKey_Info
{  
	unsigned char WEP[50];
	unsigned char WPA[50];
	unsigned char WPA2[50];
	unsigned char reserve[100];   // default by zero(0x00) ( don't care )
}	wlan_sec_key_info_t;

#define BD_GET_FIRMWARE_VER_REQ			"BackdoorPacketRetrieveInformation_Req"
#define BD_GET_FIRMWARE_VER_ACK			"BackdoorPacketRetrieveInformation_Ack"

#define BD_GET_MACS_REQ					"BackdoorPacketRetrieveMACs_Req"
#define	BD_GET_MACS_ACK					"BackdoorPacketRetrieveMACs_Ack"

#define BD_SET_MACS_REQ					"BackdoorPacketLoadMACs_Req"
#define BD_SET_MACS_ACK					"BackdoorPacketLoadMACs_Ack"

#define BD_RESTORE_DEFAULTS_REQ			"BackdoorPacketRestoreToDefault_Req"
#define BD_RESTORE_DEFAULTS_ACK			"BackdoorPacketRestoreToDefault_Ack"

#define BD_GET_SN_REQ           		"BackdoorPacketRetrieveSerialNum_Req"
#define BD_GET_SN_ACK           		"BackdoorPacketRetrieveSerialNum_Ack"

#define BD_Load_SN_REQ           		"BackdoorPacketLoadSerialNum_Req"
#define BD_Load_SN_ACK           		"BackdoorPacketLoadSerialNum_Ack"

#define BD_SET_BUTTON_LED_REQ			"BackdoorPacketSetToButtonLedTest_Req"
#define BD_SET_BUTTON_LED_ACK			"BackdoorPacketSetToButtonLedTest_Ack"

#define	BD_SET_BAND_11G_REQ				"BackdoorPacketSetBandto11G_Req"
#define	BD_SET_BAND_11G_ACK				"BackdoorPacketSetBandto11G_Ack"

#define	BD_SET_BAND_11A_REQ				"BackdoorPacketSetBandto11A_Req"
#define	BD_SET_BAND_11A_ACK				"BackdoorPacketSetBandto11A_Ack"

#define BD_SET_COUNTRY_CODE_REQ			"BackdoorPacketLoadCountryCode_Req"
#define BD_SET_COUNTRY_CODE_ACK			"BackdoorPacketLoadCountryCode_Ack"

#define BD_GET_COUNTRY_CODE_REQ			"BackdoorPacketGetCountryCode_Req"
#define BD_GET_COUNTRY_CODE_ACK			"BackdoorPacketGetCountryCode_ACK"

#define BD_SET_LANGUAGE_CODE_REQ		"BackdoorPacketLoadLanguageCode_Req"
#define BD_SET_LANGUAGE_CODE_ACK		"BackdoorPacketLoadLanguageCode_Ack"

#define BD_GET_LANGUAGE_CODE_REQ		"BackdoorPacketGetLanguageCode_Req"
#define BD_GET_LANGUAGE_CODE_ACK		"BackdoorPacketGetLanguageCode_Ack"

#define	BD_SET_NATION_CODE_REQ			"BackdoorPacketLoadNationCode_Req"
#define	BD_SET_NATION_CODE_ACK			"BackdoorPacketLoadNationCode_Ack"

#define	BD_GET_NATION_CODE_REQ			"BackdoorPacketGetNationCode_Req"
#define	BD_GET_NATION_CODE_ACK			"BackdoorPacketGetNationCode_Ack"

#define BD_BACK_SHIPPING_MODE_REQ		"BackdoorPacketBackToShippingMode_Req"
#define BD_BACK_SHIPPING_MODE_ACK		"BackdoorPacketBackToShippingMode_Ack"

#define BD_BACK_TESTING_MODE_REQ		"BackdoorPacketBackToTestingMode_Req"
#define BD_BACK_TESTING_MODE_ACK		"BackdoorPacketBackToTestingMode_Ack"

#define BD_GET_SSID_CHANNEL_REQ			"BackdoorPacketRetrieveSSIDChannel_Req"
#define BD_GET_SSID_CHANNEL_ACK			"BackdoorPacketRetrieveSSIDChannel_Ack"

#define BD_BACK_SHIPMODE_NOBOOT_REQ		"BackdoorPacketBackToShippingModeWithoutReboot_Req"
#define BD_BACK_SHIPMODE_NOBOOT_ACK		"BackdoorPacketBackToShippingModeWithoutReboot_Ack"

#define BD_BACK_TESTMODE_NOBOOT_REQ		"BackdoorPacketBackToTestingModeWithoutReboot_Req"
#define BD_BACK_TESTMODE_NOBOOT_ACK		"BackdoorPacketBackToTestingModeWithoutReboot_Ack"

#define BD_GET_FWMODE_REQ				"BackdoorPacketGetFWMode_Req"
#define BD_GET_FWMODE_ACK				"BackdoorPacketGetFWMode_Ack"

#define BD_DISABLE_WLAN_SECURITY_REQ	"BackdoorPacketDisableWlanSecurity_Req"
#define BD_DISABLE_WLAN_SECURITY_ACK	"BackdoorPacketDisableWlanSecurity_Ack"

#define BD_SET_SSID_CHANNEL_REQ			"BackdoorPacketSetSSIDChannel_Req"
#define BD_SET_SSID_CHANNEL_ACK			"BackdoorPacketSetSSIDChannel_Ack"

#define	BD_GET_EXTENDED_UNIT_INFO_REQ	"BackdoorPacketRetrieveExtendedUnitInfo_Req"
#define	BD_GET_EXTENDED_UNIT_INFO_ACK	"BackdoorPacketRetrieveExtendedUnitInfo_Ack"

#define	BD_GET_MODEL_NAME_REQ			"BackdoorPacketRetrieveModelName_Req"
#define	BD_GET_MODEL_NAME_ACK			"BackdoorPacketRetrieveModelName_Ack"

#define	BD_SET_MODEL_NAME_REQ			"BackdoorPacketLoadModelName_Req"
#define	BD_SET_MODEL_NAME_ACK			"BackdoorPacketLoadModelName_Ack"

#define BD_DISABLE_WLAN_REQ				"BackdoorPacketDisableWlanFunction_Req"
#define BD_DISABLE_WLAN_ACK				"BackdoorPacketDisableWlanFunction_Ack"

#define	BD_REMOVE_REQ					"BackdoorPacketRemove_Req"
#define	BD_REMOVE_ACK					"BackdoorPacketRemove_Ack"

#define	BD_REFRESH_TAMPER_PROOF_REQ		"BackdoorPacketRefreshTamperProof_Req"
#define	BD_REFRESH_TAMPER_PROOF_ACK		"BackdoorPacketRefreshTamperProof_Ack"

#define	BD_CHECK__TAMPER_PROOF_REQ		"BackdoorPacketCheckTamperProof_Req"
#define	BD_CHECK__TAMPER_PROOF_ACK		"BackdoorPacketCheckTamperProof_Ack"

#define BD_SET_WLAN_CHANNEL_REQ	"BackdoorPacketSetWlanChannel_Req"
#define BD_SET_WLAN_CHANNEL_ACK	"BackdoorPacketSetWlanChannel_Ack"

#define BD_RESTORE_WLAN_CHANNEL_REQ	"BackdoorPacketRestoreWlanChannel_Req"
#define BD_RESTORE_WLAN_CHANNEL_ACK	"BackdoorPacketRestoreWlanChannel_Ack"

#define	BD_USB_TEST_REQ					"BackdoorPacketUSBTest_Req"
#define	BD_USB_TEST_ACK					"BackdoorPacketUSBTest_Ack"

#define	BD_SET_SEC_KEY_REQ					"BackdoorPacketSetWlanSecurityKey_Req"
#define	BD_SET_SEC_KEY_ACK					"BackdoorPacketSetWlanSecurityKey_Ack"

#define	BD_GET_SEC_KEY_REQ					"BackdoorPacketGetWlanSecurityKey_Req"
#define	BD_GET_SEC_KEY_ACK					"BackdoorPacketGetWlanSecurityKey_Ack"


int mcast_send(int* s_socket, unsigned char* send_buffer, int send_length);
int parse_packets(int* mcast_socket, char* buf, int length);
int macaddr_to_6byte(char* mac_str, unsigned char* byte_buf);
