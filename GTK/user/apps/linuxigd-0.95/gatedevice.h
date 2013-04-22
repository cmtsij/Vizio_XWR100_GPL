#ifndef _GATEDEVICE_H_
#define _GATEDEVICE_H_ 1

// +++ Gemtek
//#include <upnp/upnp.h>
#include <upnp.h>
// --- Gemtek

// IGD Device Globals
UpnpDevice_Handle deviceHandle;
char *gateUDN;

long int startup_time;

// State Variables
char ConnectionType[50];
char PossibleConnectionTypes[50];
char ConnectionStatus[20];
long int StartupTime;
char LastConnectionError[35];
long int AutoDisconnectTime;
long int IdleDisconnectTime;
long int WarnDisconnectDelay;
int RSIPAvailable;
int NATEnabled;
char ExternalIPAddress[20];
// +++ Gemtek, Fix Vendor DMA device compatibility issue
char GMTK_ExternalIPAddress[20];
// --- Gemtek, Fix Vendor DMA device compatibility issue
int PortMappingNumberOfEntries;
int PortMappingEnabled;

// Helper routines
char* GetFirstDocumentItem( IN IXML_Document * doc, const char *item );

// Linked list for portmapping entries
struct portMap *pmlist_Head;
struct portMap *pmlist_Current;

// WanIPConnection Actions 
int EventHandler(Upnp_EventType EventType, void *Event, void *Cookie);
int StateTableInit(char *descDocUrl);
int HandleSubscriptionRequest(struct Upnp_Subscription_Request *sr_event);
int HandleGetVarRequest(struct Upnp_State_Var_Request *gv_event);
int HandleActionRequest(struct Upnp_Action_Request *ca_event);

int GetConnectionTypeInfo(struct Upnp_Action_Request *ca_event);
int GetNATRSIPStatus(struct Upnp_Action_Request *ca_event);
int SetConnectionType(struct Upnp_Action_Request *ca_event);
int RequestConnection(struct Upnp_Action_Request *ca_event);
int GetTotalBytesSent(struct Upnp_Action_Request *ca_event);
int GetTotalBytesReceived(struct Upnp_Action_Request *ca_event);
int GetTotalPacketsSent(struct Upnp_Action_Request *ca_event);
int GetTotalPacketsReceived(struct Upnp_Action_Request *ca_event);
int GetCommonLinkProperties(struct Upnp_Action_Request *ca_event);
int InvalidAction(struct Upnp_Action_Request *ca_event);
int GetStatusInfo(struct Upnp_Action_Request *ca_event);
int AddPortMapping(struct Upnp_Action_Request *ca_event);
int GetGenericPortMappingEntry(struct Upnp_Action_Request *ca_event);
int GetSpecificPortMappingEntry(struct Upnp_Action_Request *ca_event);
int GetExternalIPAddress(struct Upnp_Action_Request *ca_event);
int DeletePortMapping(struct Upnp_Action_Request *ca_event);

// Definitions for mapping expiration timer thread
#define THREAD_IDLE_TIME 5000
#define JOBS_PER_THREAD 10
#define MIN_THREADS 2 
#define MAX_THREADS 12 

int ExpirationTimerThreadInit(void);
int ExpirationTimerThreadShutdown(void);
int ScheduleMappingExpiration(struct portMap *mapping, char *DevUDN, char *ServiceID);
int CancelMappingExpiration(int eventId);
void DeleteAllPortMappings(void);

#endif //_GATEDEVICE_H
