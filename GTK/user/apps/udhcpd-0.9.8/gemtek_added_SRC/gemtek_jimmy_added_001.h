#ifndef	_Gemtek_Jimmy_Added_001_H_

#define	_Gemtek_Jimmy_Added_001_H_

// added by Jimmy Ma 2003.4.29
#define SIOCSETCIFENABLE	35323
#define SIOCSETCIPOK		35324

#define	CPU_ID_4702		"BCM4710"
#define	CPU_ID_4712		"BCM3302"

#include <stdio.h>
#include <bcmnvram.h>
#include <stdlib.h>
#include <shutils.h>

char		*get_wan_if_name ( void );
char		WhichPlatform ( void );
char		LED_Control_For_4712_Blinking ( void );
char		LED_Control_For_4712_WAN_INPUT_MODE ( void );
void		File_Read ( char *PathName );

static int
LED_wan_ip_ok(int ok)
{
	int s, mydata;
	struct ifreq ifr;

	/* Open socket to kernel */
	if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		return 0;

//	strncpy(ifr.ifr_name, "eth1", IFNAMSIZ);
	strncpy ( ifr.ifr_name , get_wan_if_name () , IFNAMSIZ );
	mydata = ok;
	ifr.ifr_data = (void *)&mydata;

	if (ioctl(s, SIOCSETCIPOK, &ifr) < 0) 
	{
		close(s);
		return 0;
	}

	close(s);
	return 1;
}

#define	WANIFNameSize	64
#define	WANIFVarName	"wanif"
#define	DefaultWANIFName	"eth1"
char		WANIFName [ WANIFNameSize ];
char		*get_wan_if_name ( void )
{
	char		*wan_ifname = NULL;
	int		StrLength = 0;

	wan_ifname = nvram_get ( WANIFVarName );

	if ( wan_ifname == NULL ) {
ZZZ:		wan_ifname = DefaultWANIFName;
		StrLength = strlen ( DefaultWANIFName );
	} else {
		StrLength = strlen ( wan_ifname );
		if ( ( StrLength <= 0 ) || ( StrLength >= WANIFNameSize ) ) {
			goto		ZZZ;
		}
	}	// end of ( wan_ifname == NULL )

	memset ( WANIFName , '\0' , WANIFNameSize );

	strncpy ( WANIFName , wan_ifname , StrLength );

	return	WANIFName;
}

// return 0 ->	It's 4702 platform.
// return 1 ->	It's 4712 platform.
// return 2 ->	Unknown platform.
char		WhichPlatform ( void )
{
	char		*cpu_type = nvram_get ( "cpu_type" );

	if ( cpu_type == NULL ) {
		return	2;	// Unknown platform.
	} else if ( strncmp ( cpu_type , CPU_ID_4712 , strlen ( CPU_ID_4712 ) ) == 0 ) {
		return	1;	// It's 4712 platform.
	} else if ( strncmp ( cpu_type , CPU_ID_4702 , strlen ( CPU_ID_4702 ) ) == 0 ) {
		return	0;	// It's 4702 platform.
	}

	return	2;	// Unknown platform.
}

//char		LED_Control_For_4712_Blinking_Flag = 0;		// 1 -> It was blinked at one time. 0 -> Blinking is not turned on.

//	return 1;	->	It's 4712 platform, and 4712 LED control is completed.
//	return 0;	->	It's not 4712 platform, and LED control will be completed by the other procedure.
char		LED_Control_For_4712_Blinking ( void )
{
//	if ( LED_Control_For_4712_Blinking_Flag == 1 ) {	// 1 -> It was blinked at one time. 0 -> Blinking is not turned on.
//		return	1;	// It's 4712 platform, and 4712 LED control is completed.
//	}

	if ( WhichPlatform () == 1 ) {	// It's 4712 platform.
		// system ( "/bin/more /proc/kmemory/WAN_RED_BLINK &" );
		if ( wan_port_reprot_from_gpio ( 0 ) == 1 ) {
//			LED_Control_For_4712_Blinking_Flag = 1;	// 1 -> It was blinked at one time. 0 -> Blinking is not turned on.
			File_Read ( "/proc/kmemory/WAN_RED_BLINK" );
		} else {
			File_Read ( "/proc/kmemory/WAN_INPUT" );
		}
		return	1;	// It's 4712 platform, and 4712 LED control is completed.
	} else {
		return	0;	// It's 4702 platform, and 4702 LED control will be completed by the other procedure.
	}

	return	0;	// It's 4702 platform, and 4702 LED control will be completed by the other procedure.
}

//	return 1;	->	It's 4712 platform, and 4712 LED control is completed.
//	return 0;	->	It's not 4712 platform, and LED control will be completed by the other procedure.
char		LED_Control_For_4712_WAN_INPUT_MODE ( void )
{
	if ( WhichPlatform () == 1 ) {	// It's 4712 platform.
		// system ( "/bin/more /proc/kmemory/WAN_INPUT &" );
		File_Read ( "/proc/kmemory/WAN_INPUT" );
		return	1;	// It's 4712 platform, and 4712 LED control is completed.
	} else {
		return	0;	// It's 4702 platform, and 4702 LED control will be completed by the other procedure.
	}

	return	0;	// It's 4702 platform, and 4702 LED control will be completed by the other procedure.
}

void		File_Read ( char *PathName )
{
	FILE		*fp = NULL;
	char		Buff [ 128 ];

	if ( PathName == NULL ) {
		return	NULL;
	}

	if ( ( fp = fopen ( PathName , "a+" ) ) != NULL ) {
		fgets ( Buff , 128 , fp );
	}

	fclose ( fp );

	return	NULL;
}

/* *********************************************************************************** */

#ifdef	RemoveWANLedControl
#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_001
#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_002
#else
#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_001		LED_wan_ip_ok(0); \
													LED_Control_For_4712_Blinking ();

#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_002		LED_wan_ip_ok(1); \
													LED_Control_For_4712_WAN_INPUT_MODE ();
#endif	// end ifdef  RemoveWANLedControl

#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_003		system ( "rc stop ; sleep 5 ; rc start &" ); \
													exit ( 0 );

#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_004		if ( tv.tv_sec < 59 ) {	\
														tv.tv_sec = 1;	\
													}

#ifdef	RemoveWANLedControl
#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_005
#else
#define	_Gemtek_Jimmy_Added_001_H_Extra_SRC_005		if ( state == BOUND ) { \
														LED_wan_ip_ok(1); \
														LED_Control_For_4712_WAN_INPUT_MODE (); \
													} else { \
														LED_wan_ip_ok(0); \
														LED_Control_For_4712_Blinking (); \
													}
#endif	// end ifdef  RemoveWANLedControl

#endif	// end of _Gemtek_Jimmy_Added_001_H_
