

#include "ILibWebClient.h"

#ifndef __DLNAPROTOCOLINFO_H__
#define __DLNAPROTOCOLINFO_H__

enum DLNAProtocolInfo_ProtocolType
{
	DLNAProtocolInfo_ProtocolType_UNKNOWN = 0,
	DLNAProtocolInfo_ProtocolType_ANY = 3,
	DLNAProtocolInfo_ProtocolType_HTTP = 1,
	DLNAProtocolInfo_ProtocolType_RTP = 2
};

struct DLNAProtocolInfo
{
	int SupportsTimeBasedSeek;
	int SupportsByteBasedSeek;
	
	char *Profile;
	char *MimeType;
	char *ContentFeatures;

	char **SupportedPlaySpeeds;
	int SupportedPlaySpeedsLength;

	int IsConvertedContent;
	enum DLNAProtocolInfo_ProtocolType Protocol;
	int IsPlaySingleUri;

	//
	// Flags
	//
	int SenderPaced;
	int LimitedOperations_TimeBasedSeek;
	int LimitedOperations_ByteBasedSeek;

	int DLNAPlayContainer;
	int S0_Increasing;
	int SN_Increasing;

	int RTSP_Pause;
	int HTTP_Stalling;
	
	int TM_S;
	int TM_I;
	int TM_B;

	int MaxSpeed_Major;
	int MaxSpeed_Minor;

	int DLNA_Major_Version;
	int DLNA_Minor_Version;

	void *NameValueTable;
};


struct DLNA_ContentServer_Info
{
	//Content-Type
	//Content-Length
	//Server

	//PlaySpeed
	//TransferMode
	
	int RealTimeInfo_MaxLagTime;
	long RealTimeInfo_Duration;

	char *PeerManager;
	char *FriendlyName;

	int SCMS_CopyProtected;
	int SCMS_Generation;

	char *ContentType;
	char *ifo_FileURI;

	struct DLNAProtocolInfo *DLNA_ContentFeatures;

	long AvailableSeekRange_ByteRange_LowerBound;
	long AvailableSeekRange_ByteRange_UpperBound;

	char *AvailableSeekRange_NPTRange_LowerBound;
	char *AvailableSeekRange_NPTRange_UpperBound;
};

struct DLNAProtocolInfo *DLNAProtocolInfo_Parse(char *protocolInfo, int protocolInfoLength);
char *DLNAProtocolInfo_Serialize(struct DLNAProtocolInfo *protocolInfo);
void DLNAProtocolInfo_Destruct(struct DLNAProtocolInfo *protocolInfo);
int DLNAProtocolInfo_IsMatch(struct DLNAProtocolInfo *source, struct DLNAProtocolInfo *target);


typedef void(*DLNA_OnGetContentInfo)(struct DLNA_ContentServer_Info *info, void *user);
void DLNA_GetContentInfo(ILibWebClient_RequestManager request_manager, char *contentURI, void *user, DLNA_OnGetContentInfo OnGetContentInfo);

char *DLNAProtocolInfo_GetNameValuePair_Value(struct DLNAProtocolInfo *protocolInfo, char *Name);

#endif


