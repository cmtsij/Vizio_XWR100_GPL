#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <time.h>
#include <net/if.h>
// +++ Gemtek
//#include <upnp/upnp.h>
#include <upnp.h>
#include "debug.h"
// --- Gemtek
#include "globals.h"
#include "config.h"
#include "gatedevice.h"
#include "util.h"
#include "pmlist.h"

// Global variables
struct GLOBALS g_vars;

// +++ Gemtek, Fix UPnP Certification Tool
char gateicfgSCPDUDN[64]	=	"uuid:75802409-bccb-40e7-8e6c-fa195ecce13e";
char gateconnSCPDUDN[64]	= "uuid:75802409-bccb-40e7-8e6c-fa295ecce13e";
// --- Gemtek, Fix UPnP Certification Tool

int main (int argc, char** argv)
{
	char descDocUrl[7+15+1+5+1+sizeof(g_vars.descDocName)+1]; // http://ipaddr:port/docName<null>
	char intIpAddress[16];     // Server internal ip address
	sigset_t sigsToCatch;
	int ret, signum, arg = 1, foreground = 0;

	if (argc < 3 || argc > 4) {
	  printf("Usage: upnpd [-f] <external ifname> <internal ifname>\n");
	  printf("  -f\tdon't daemonize\n");
	  printf("Example: upnpd ppp0 eth0\n");
	  exit(0);
	}

	parseConfigFile(&g_vars);

	// check for '-f' option
	if (strcmp(argv[arg], "-f") == 0) {
		foreground = 1;
		arg++;
	}

	// Save interface names for later use
	strncpy(g_vars.extInterfaceName, argv[arg++], IFNAMSIZ);
	strncpy(g_vars.intInterfaceName, argv[arg++], IFNAMSIZ);

	// Get the internal ip address to start the daemon on
	if (GetIpAddressStr(intIpAddress, g_vars.intInterfaceName) == 0) {
		fprintf(stderr, "Invalid internal interface name '%s'\n", g_vars.intInterfaceName);
		exit(EXIT_FAILURE);
	}

	// +++ Gemtek, Fix Vendor DMA device compatibility issue
	// FixME : 
	//		The ExternalIPAddress should be refreshed once device gets related query message
	//		But it fail to get the ExternalIPAddress on WRTB193N, 
	//		it may be that kernel is too busy to get the ip address from vlan2 interface
	memset(GMTK_ExternalIPAddress, 0, sizeof(GMTK_ExternalIPAddress));
	if (GetIpAddressStr(GMTK_ExternalIPAddress, g_vars.extInterfaceName) == 0) {
		fprintf(stderr, "Invalid external interface name '%s'\n", g_vars.extInterfaceName);
	}
	// --- Gemtek, Fix Vendor DMA device compatibility issue


	// +++ Gemtek, Add Dynamic UUID
	snprintf(gateicfgSCPDUDN, 30, "uuid:%s", g_vars.device_uuid);
	strcat(gateicfgSCPDUDN, "fa195ecce13e");
	snprintf(gateconnSCPDUDN, 30, "uuid:%s", g_vars.device_uuid);
	strcat(gateconnSCPDUDN, "fa295ecce13e");
	// --- Gemtek, Add Dynamic UUID
	
	if (!foreground) {
		struct rlimit resourceLimit = { 0, 0 };
		pid_t pid, sid;
		unsigned int i;

		// Put igd in the background as a daemon process.
		pid = fork();
		if (pid < 0)
		{
			perror("Error forking a new process.");
			exit(EXIT_FAILURE);
		}
		if (pid > 0)
			exit(EXIT_SUCCESS);

		// become session leader
		if ((sid = setsid()) < 0)
		{
			perror("Error running setsid");
			exit(EXIT_FAILURE);
		}

		// close all file handles
		resourceLimit.rlim_max = 0;
		ret = getrlimit(RLIMIT_NOFILE, &resourceLimit);
		if (ret == -1) /* shouldn't happen */
		{
		    perror("error in getrlimit()");
		    exit(EXIT_FAILURE);
		}
		if (0 == resourceLimit.rlim_max)
		{
		    fprintf(stderr, "Max number of open file descriptors is 0!!\n");
		    exit(EXIT_FAILURE);
		}	
		for (i = 0; i < resourceLimit.rlim_max; i++)
		    close(i);
	
		// fork again so child can never acquire a controlling terminal
		pid = fork();
		if (pid < 0)
		{
			perror("Error forking a new process.");
			exit(EXIT_FAILURE);
		}
		if (pid > 0)
			exit(EXIT_SUCCESS);
	
		if ((chdir("/")) < 0)
		{
			perror("Error setting root directory");
			exit(EXIT_FAILURE);
		}
	}

	umask(0);

// End Daemon initialization

	openlog("upnpd", LOG_CONS | LOG_NDELAY | LOG_PID | (foreground ? LOG_PERROR : 0), LOG_LOCAL6);

	// Initialize UPnP SDK on the internal Interface
	trace(3, "Initializing UPnP SDK ... ");
	if ( (ret = UpnpInit(intIpAddress,0) ) != UPNP_E_SUCCESS)
	{
		syslog (LOG_ERR, "Error Initializing UPnP SDK on IP %s ",intIpAddress);
		syslog (LOG_ERR, "  UpnpInit returned %d", ret);
		UpnpFinish();
		exit(1);
	}
	trace(2, "UPnP SDK Successfully Initialized.");

	// Set the Device Web Server Base Directory
	trace(3, "Setting the Web Server Root Directory to %s",g_vars.xmlPath);
	if ( (ret = UpnpSetWebServerRootDir(g_vars.xmlPath)) != UPNP_E_SUCCESS )
	{
		syslog (LOG_ERR, "Error Setting Web Server Root Directory to: %s", g_vars.xmlPath);
		syslog (LOG_ERR, "  UpnpSetWebServerRootDir returned %d", ret); 
		UpnpFinish();
		exit(1);
	}
	trace(2, "Succesfully set the Web Server Root Directory.");

	//initialize the timer thread for expiration of mappings
	if (ExpirationTimerThreadInit()!=0) {
	  syslog(LOG_ERR,"ExpirationTimerInit failed");
	  UpnpFinish();
	  exit(1);
	}

	// Form the Description Doc URL to pass to RegisterRootDevice
	sprintf(descDocUrl, "http://%s:%d/%s", UpnpGetServerIpAddress(),
				UpnpGetServerPort(), g_vars.descDocName);

	// Register our IGD as a valid UPnP Root device
	trace(3, "Registering the root device with descDocUrl %s", descDocUrl);
	if ( (ret = UpnpRegisterRootDevice(descDocUrl, EventHandler, &deviceHandle,
					   &deviceHandle)) != UPNP_E_SUCCESS )
	{
		syslog(LOG_ERR, "Error registering the root device with descDocUrl: %s", descDocUrl);
		syslog(LOG_ERR, "  UpnpRegisterRootDevice returned %d", ret);
		UpnpFinish();
		exit(1);
	}

	trace(2, "IGD root device successfully registered.");
	
	// Initialize the state variable table.
	StateTableInit(descDocUrl);
	
	// Record the startup time, for uptime
	startup_time = time(NULL);

// +++ Gemtek, To Fix UPnP CertificationTestTool ver 1.5 test case 2.00.00	
#if 0
	// Send out initial advertisements of our device's services with timeouts of 30 minutes
	if ( (ret = UpnpSendAdvertisement(deviceHandle, 1800) != UPNP_E_SUCCESS ))
	{
		syslog(LOG_ERR, "Error Sending Advertisements.  Exiting ...");
		UpnpFinish();
		exit(1);
	}
	trace(2, "Advertisements Sent.  Listening for requests ... ");
#endif
	// Send out initial advertisements of our device's services with timeouts of 1 minutes
	if ( (ret = UpnpSendAdvertisement(deviceHandle, 300) != UPNP_E_SUCCESS ))
	{
		syslog(LOG_ERR, "Error Sending Advertisements.  Exiting ...");
		UpnpFinish();
		exit(1);
	}
	trace(2, "Advertisements Sent.  Listening for requests ... ");
// --- Gemtek, To Fix UPnP CertificationTestTool ver 1.5 test case 2.00.00	
	
	
	// Loop until program exit signals received
	do {
	  sigemptyset(&sigsToCatch);
	  sigaddset(&sigsToCatch, SIGINT);
	  sigaddset(&sigsToCatch, SIGTERM);
	  sigaddset(&sigsToCatch, SIGUSR1);
	  pthread_sigmask(SIG_SETMASK, &sigsToCatch, NULL);
	  sigwait(&sigsToCatch, &signum);
	  trace(3, "Caught signal %d...\n", signum);
	  switch (signum) {
	  case SIGUSR1:
	    DeleteAllPortMappings();
	    break;
	  default:
	    break;
	  }
	} while (signum!=SIGTERM && signum!=SIGINT);

	trace(2, "Shutting down on signal %d...\n", signum);

	// Cleanup UPnP SDK and free memory
	DeleteAllPortMappings();
	ExpirationTimerThreadShutdown();

	UpnpUnRegisterRootDevice(deviceHandle);
	UpnpFinish();

	// Exit normally
	return (0);
}
