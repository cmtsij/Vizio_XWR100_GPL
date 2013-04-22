/***************************************************************************
 *                  Sample HTTP calls for Cisco-Linksys                   *
 *                  ------------------------------------                   *
 *   begin                : Fri June 20 2008                               *
 *   copyright            : (C) 2008 by TZO                                *
 *   email                : mdf@tzo.com                                    *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include "nvram.h"
#include "tzoupdate.h"

#define SOCKET_ERROR    -1
#define WSAEWOULDBLOCK	-1
#define TRUE            1
#define FALSE           0
#define INVALID_SOCKET	-1

#define TCP_SOCKET	1
#define UDP_SOCKET	2
#define MAX_MESSAGE_SIZE 6000

#define EXIT_OK			0
#define EXIT_TEMP_BLOCK		1
#define EXIT_EXPIRED		2
#define EXIT_FATAL		3
#define EXIT_CONFIG_ERROR	4

typedef struct
	{
	char * lpMem ;
	long Size ;
	long Ptr ;
	} MEMSTRUCT ;

unsigned char TzoUpdateServerName[64] = {"vizio.rh.tzo.com"} ;
unsigned char TzoEchoServerName[64] = {"vizio.echo.tzo.com"} ;
int DefaultHttpPort = 80 ;

/*
 * The User Agent, This should be modified for corporate usage
 */
char TZO_UserAgentString[128] = {"vizio-Gemtek"} ;

/*
 *  This is the version of the release controlled by TZO
 */
char TZO_VERSION[32] = {"1.5"} ;

/*
 * These have been hardcoded to get you started
 */
//char szGlobalTZOKey[64] = {"K392684398408799"} ;
//char szGlobalEmailAddress[64] = {"Marmot_Chen@gemtek.com.tw"} ;
//char szGlobalDomainName[64] = {"gemtek1.myrouter.com"} ;
char szGlobalTZOKey[64];
char szGlobalEmailAddress[64];
char szGlobalDomainName[64];
char szGlobalPort[10] = {"80"} ; // Alternate HTTP port 21333
int Verbose = 1 ;

#define SIZEOF_IP_ADDR 32

int MemRelease(MEMSTRUCT * lpMemBlock) {
	if (lpMemBlock->lpMem)
		free(lpMemBlock->lpMem) ;
	lpMemBlock->lpMem = 0 ;
	lpMemBlock->Size = 0 ;
	lpMemBlock->Ptr = 0 ;

	return 1 ;
}


int MemCreate(MEMSTRUCT * lpMemBlock, long Size) {
	lpMemBlock->lpMem = malloc(Size) ;
	if (lpMemBlock->lpMem == 0)
		return 0 ;
	memset(lpMemBlock->lpMem,0,Size) ;
	lpMemBlock->Size = Size ;
	lpMemBlock->Ptr = 0 ;
	lpMemBlock->lpMem[0] = 0 ;

	return 1 ;
}


int EvalInt(char * lpMem, int * p, int Count) {
	int val = 0 ;

	while (*p < Count) {
		if (lpMem[*p] != ' ')
			break ;
		(*p) ++ ;
	}

	while (*p < Count) {
		if ((lpMem[*p] < '0') || (lpMem[*p] > '9'))
			break ;
		val = val * 10 + (lpMem[*p] - '0') ;
		(*p) ++ ;
	}

	(*p) ++ ;
	return val ;
}


int OpenConnectionByAddr(unsigned int IPAddress, unsigned short Port, int SocketType)	{
	struct sockaddr_in sin ;
	int err ;
	int RcvBufSize = MAX_MESSAGE_SIZE ;
	int NewSocket = 0 ;


	switch (SocketType)	{
		case UDP_SOCKET : {
			if ((NewSocket = socket(PF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
				return 0 ;
			break ;
		}

		case TCP_SOCKET : {
			if ((NewSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
				return 0 ;

			if ((err = setsockopt(NewSocket, SOL_SOCKET, SO_RCVBUF, (char *)&RcvBufSize, sizeof(int))) != 0) {
				close(NewSocket) ;
				return 0 ;
			}

			sin.sin_family = AF_INET ;
			sin.sin_addr.s_addr = 0 ;
			sin.sin_port = 0 ;
			if ((err = bind(NewSocket, (struct sockaddr *) &sin, sizeof(sin))) != 0) {
				close(NewSocket) ;
				return 0 ;
			}

			sin.sin_family = AF_INET ;
			sin.sin_addr.s_addr = IPAddress ;
			sin.sin_port = htons(Port) ;
			if ((err = connect(NewSocket, (struct sockaddr *) &sin, sizeof(sin))) != 0) {
				err = errno ;
				if (err == WSAEWOULDBLOCK)
					return NewSocket ;
				close(NewSocket) ;
				return 0 ;
			}

			break ;
		}
	}
	return NewSocket ;
}


unsigned int FetchIPAddress(unsigned char * HostName) {
	struct hostent * lpHostEnt ;
	unsigned int IPAddress = 0 ;
	unsigned char * lpIPAddress = (unsigned char *)&IPAddress ;
	int i ;

	if ((IPAddress = inet_addr((char *)HostName)) != INADDR_NONE)
		return IPAddress ;

	if ((lpHostEnt = MyGetHostByName((char *)HostName)) == 0)	{
		//if ((lpHostEnt = MyGetHostByName((char *)HostName)) == 0) {
			printf("FetchIPAddress() : Check internet connection\n") ;
			return 0 ;
		//}
	}

	for (i=0; i<4; i++)
		lpIPAddress[i] = lpHostEnt->h_addr[i] ;

	return IPAddress ;
}


int SendToSocket(int Socket, char * lpRecord, int RecordSize) {
	int BytesSent ;
	int p = 0 ;

	while(p < RecordSize) {
		BytesSent = send(Socket, &lpRecord[p], RecordSize - p, 0) ;
		if (BytesSent == SOCKET_ERROR)
			return FALSE ;
		p += BytesSent ;
	}

	return TRUE ;
}


int FindStringInMem(char * str, char * lpMem, int Offset, int Count) {
	int len = strlen(str) ;
	int last = Count - len + 1 ;
	int i, j ;

	for (i=Offset; i<last; i++) {
		for (j=0; j<len; j++)
			if (lpMem[i + j] != str[j])
				break ;
		if (j == len)
			return i ;
	}

	return -1 ;
}


int ReadFromSocket(int Socket, MEMSTRUCT *lpMemStruct, unsigned int TimeOut)	{
	int p = 0 ;
	int BytesRead ;
	int ContentLength ;
	int mp ;
	time_t WaitTimer = time(NULL) + TimeOut ;
	unsigned short Result ;
	fd_set fSocketArray ;

	memset(&lpMemStruct->lpMem[0], 0, lpMemStruct->Size) ;
	lpMemStruct->Ptr = 0 ;

	while(time(NULL) < WaitTimer)	{
		struct timeval tv = {0,5} ;
		FD_ZERO(&fSocketArray) ;
		FD_SET(Socket, &fSocketArray) ;
		tv.tv_sec = 0 ;
		tv.tv_usec = 5 ;
		Result = select(Socket+1, &fSocketArray, NULL, NULL, &tv) ;

		if (Result == 0) {
			usleep(100) ;
			continue ;
		}

		if ((BytesRead = recv(Socket, &lpMemStruct->lpMem[p], lpMemStruct->Size-10, 0)) == SOCKET_ERROR)
			return 0 ;

		if (BytesRead == 0)
			return 1 ;

		lpMemStruct->lpMem[p + BytesRead] = 0 ;

		p += BytesRead ;

		lpMemStruct->Ptr = p ;

		if ((mp = FindStringInMem("Content-length: ", lpMemStruct->lpMem, 0, p)) < 0)
			if ((mp = FindStringInMem("Content-Length: ", lpMemStruct->lpMem, 0, p)) < 0)
				continue ;

		mp += strlen("Content-length: ") ;

		ContentLength = EvalInt(lpMemStruct->lpMem, &mp, p) ;

		if ((mp = FindStringInMem("\r\n\r\n", lpMemStruct->lpMem, 0, p)) < 0)
			continue ;

		mp += strlen("\r\n\r\n") ;

		if (p >= (ContentLength + mp))
			return 1 ;
	}

	return 0 ;
}


int AppendMem(int op, char * lpMem, char * str) {
	int l = strlen((char *)str) ;
	int i ;

	for (i=0; i<l; i++)
		lpMem[op++] = str[i] ;

	lpMem[op] = 0 ;
	return op ;
}


int FormHTTPRequest(unsigned char * WebHostName, char * FileName, char * lpMem, char * lpArgs) {
	int op = 0 ;
	char ts[512] ;

	sprintf(ts, "%s %s HTTP/1.0\r\n", (lpArgs[0] == 0) ? "GET" : "POST", FileName) ;
	op = AppendMem(op, lpMem, ts) ;

	sprintf(ts, "Host: %s\r\n", WebHostName) ;
	op = AppendMem(op, lpMem, ts) ;

	sprintf(ts, "User-Agent: TZO HTTP Update / Version %s [%s]\r\n", TZO_VERSION, TZO_UserAgentString) ;
	op = AppendMem(op, lpMem, ts) ;

	if (lpArgs[0]) {
		sprintf(ts, "Content-type: application/x-www-form-urlencoded\r\n") ;
		op = AppendMem(op, lpMem, ts) ;

		sprintf(ts, "Content-length: %d\r\n\r\n", strlen((char *)lpArgs)) ;
		op = AppendMem(op, lpMem, ts) ;

		op = AppendMem(op, lpMem, (char *)lpArgs) ;
	} else {
		sprintf(ts, "\r\n") ;
		op = AppendMem(op, lpMem, ts) ;
	}

	return op ;
}

int ShowDataBuf(char * Direction, char *szDataBuf) {
	int i ;
	printf("%s ", Direction) ;
	for (i = 0 ; i < strlen(szDataBuf) ; i++) {
		if (szDataBuf[i] == '\n')
			printf("\n%s ", Direction) ;
		else
			printf("%c", szDataBuf[i]) ;
	}
	printf("\n") ;
	return 1 ;
}


int TzoGetCurrentIP(unsigned char * szCurrentIPAddress) {
	int ServerSocket ;
	MEMSTRUCT DataBuf ;
	unsigned int ServerIPAddress ;
	char szMsg[1024] ;
	int Loc = 0 ;

	szCurrentIPAddress[0] = 0 ;

	if ((MemCreate(&DataBuf, MAX_MESSAGE_SIZE)) == FALSE)  {
		nvram_set("TZODDNSok", "Internal error allocating memory");
		return(ERR_MEMCREATE_FAILED) ;
	}

	if ((ServerIPAddress = FetchIPAddress(TzoEchoServerName)) == 0) {
		if (Verbose)
			printf("* Unable to fetch address to <%s>\n", TzoEchoServerName) ;
		nvram_set("TZODDNSok", "Could not fetch IP address for domain");
		return(ERR_FETCHIPADDRESS_FAILED) ;
	}

	if ((ServerSocket = OpenConnectionByAddr(ServerIPAddress, DefaultHttpPort, TCP_SOCKET)) == INVALID_SOCKET) {
		if (Verbose)
			printf("* Unable to Open connection to <%s>\n", TzoEchoServerName) ;
		nvram_set("TZODDNSok", "Could not connect to server");
		return(ERR_OPENCONNECTION_FAILED) ;
	}

	sprintf(szMsg, "/ip.shtml") ;

	DataBuf.Ptr = FormHTTPRequest(TzoEchoServerName, szMsg, DataBuf.lpMem, "") ;

	if (Verbose) {
		printf ("* Data sent to %s on %d\n", TzoEchoServerName, DefaultHttpPort) ;
		ShowDataBuf(">", DataBuf.lpMem) ;
	}

	if ((SendToSocket(ServerSocket, DataBuf.lpMem, DataBuf.Ptr)) == 0) {
		close(ServerSocket) ;
		if (Verbose)
			printf("* Unable to send data to <%s>\n", TzoEchoServerName) ;
		nvram_set("TZODDNSok", "Could not send data to server");
		return(ERR_SENDTOSOCKET_FAILED) ;
	}

	if ((ReadFromSocket(ServerSocket, &DataBuf,  15)) == 0) {
		close(ServerSocket) ;
		if (Verbose)
			printf("* Unable to recieve data from <%s>\n", TzoEchoServerName) ;
		nvram_set("TZODDNSok", "Could not read from server");
		return(ERR_READFROMSOCKET_FAILED) ;
	}
	close(ServerSocket) ;

	if (Verbose) {
		printf("* Data read from Echo Server \n") ;
		ShowDataBuf("<", DataBuf.lpMem) ;
	}

	if ((Loc = FindStringInMem("IPAddress:", DataBuf.lpMem, 0, DataBuf.Ptr)) >= 0) {
		int StartLoc = Loc + strlen("IPAddress:") ;
		int i, ii ;
		for (i = StartLoc, ii = 0 ; i < DataBuf.Ptr ; i++)
			szCurrentIPAddress[ii++] = DataBuf.lpMem[i] ;
		szCurrentIPAddress[ii] = 0 ;
	} else {
		if (Verbose)
			printf("* Bad packet data from <%s>\n", TzoEchoServerName) ;
		nvram_set("TZODDNSok", "Server responed with an invalid packet");
		return(ERR_BAD_PACKET_RETURNED);
	}
	MemRelease(&DataBuf) ;

	return(TRUE) ;
}


int TZOLogon(unsigned char *szKey, unsigned char *szEmail, unsigned char *szDomain, unsigned char * szCurrentIPAddress, char *szReturnBuffer) {
	int ServerSocket ;
	MEMSTRUCT DataBuf ;
	unsigned int ServerIPAddress ;
	char szMsg[1024], szServerReturnData[200] ;
	int Loc ;

	if ((MemCreate(&DataBuf, MAX_MESSAGE_SIZE)) == FALSE)  {
		return(ERR_MEMCREATE_FAILED) ;	}

	if ((ServerIPAddress = FetchIPAddress(TzoUpdateServerName)) == 0) {
		if (Verbose)
			printf("** Unable to fetch address to <%s>\n", TzoUpdateServerName) ;
		return(ERR_FETCHIPADDRESS_FAILED) ;
	}

	if ((ServerSocket = OpenConnectionByAddr(ServerIPAddress, DefaultHttpPort, TCP_SOCKET)) == INVALID_SOCKET) {
		if (Verbose)
			printf("** Unable to Open connection to <%s>\n", TzoUpdateServerName) ;
		return(ERR_OPENCONNECTION_FAILED) ;
	}

	sprintf(szMsg, "/webclient/tzoperl.html?TZOName=%s&Email=%s&TZOKey=%s&IPAddress=%s&system=tzodns&info=1", szDomain, szEmail, szKey, szCurrentIPAddress) ;

	DataBuf.Ptr = FormHTTPRequest(TzoUpdateServerName, szMsg, DataBuf.lpMem, "") ;

        if (Verbose) {
                printf("** Data Sent to %s on %d\n", TzoUpdateServerName, DefaultHttpPort) ;
		ShowDataBuf(">>", DataBuf.lpMem) ;
	}

	if ((SendToSocket(ServerSocket, DataBuf.lpMem, DataBuf.Ptr)) == 0) {
		close(ServerSocket) ;
		if (Verbose)
			printf("** Unable to send data to <%s>\n", TzoUpdateServerName) ;
		return(ERR_SENDTOSOCKET_FAILED) ;
	}

	if ((ReadFromSocket(ServerSocket, &DataBuf,  15)) == 0) {
		close(ServerSocket) ;
		if (Verbose)
			printf("** Unable to read data from <%s>\n", TzoUpdateServerName) ;
		return(ERR_READFROMSOCKET_FAILED) ;
	}

	if (Verbose) {
		printf("** Data read from Update Server \n") ;
		ShowDataBuf("<<", DataBuf.lpMem) ;
	}

	if ((Loc = FindStringInMem("\r\n\r\n", DataBuf.lpMem, 0, DataBuf.Ptr)) >= 0) {
		int StartLoc = Loc + strlen("\r\n\r\n") ;
		int i, ii ;
		for (i = StartLoc, ii = 0 ; i < DataBuf.Ptr ; i++)
			szServerReturnData[ii++] = DataBuf.lpMem[i] ;
		szServerReturnData[ii] = 0 ;
		if ((Loc = FindStringInMem("\r\n", szServerReturnData, 0, strlen(szServerReturnData))) >= 0) {
			StartLoc = Loc + strlen("\r\n") ;
			for (i = StartLoc, ii = 0 ; i < strlen(szServerReturnData) ; i++)
				szReturnBuffer[ii++] = szServerReturnData[i] ;
			szReturnBuffer[ii] = 0 ;
		}
	} else {
		close(ServerSocket) ;
		if (Verbose)
			printf("** Bad packet data from <%s>\n", TzoUpdateServerName) ;
		return(ERR_BAD_PACKET_RETURNED);
	}

	close(ServerSocket) ;
	return atoi(szServerReturnData) ;
}


int tzo(int argc, char *argv[]) {
	unsigned char szCurrrentIpAddress[SIZEOF_IP_ADDR] ;
	unsigned char szLastIpAddress[SIZEOF_IP_ADDR] ;
	unsigned char szReturnBuffer[200] ;
	int ReturnVal ;
	
	memset(TzoUpdateServerName, 0 , sizeof(TzoUpdateServerName));
	memset(TzoEchoServerName, 0 , sizeof(TzoEchoServerName));
	memset(szGlobalTZOKey, 0 , sizeof(szGlobalTZOKey));
	memset(szGlobalEmailAddress, 0 , sizeof(szGlobalEmailAddress));
	memset(szGlobalDomainName, 0 , sizeof(szGlobalDomainName));
	memset(szCurrrentIpAddress,0,SIZEOF_IP_ADDR) ;
	memset(szLastIpAddress,0,SIZEOF_IP_ADDR) ;
	
	strcpy(TzoUpdateServerName, nvram_safe_get("tzo_update_server")) ;
	strcpy(TzoEchoServerName, nvram_safe_get("tzo_echo_server")) ;
	strcpy(szGlobalTZOKey, nvram_safe_get("tzo_ddns_password")) ;
	strcpy(szGlobalEmailAddress, nvram_safe_get("tzo_ddns_username")) ;
	strcpy(szGlobalDomainName, nvram_safe_get("tzo_ddns_domain")) ;

	DefaultHttpPort = atoi(szGlobalPort) ;
	if ((DefaultHttpPort != 80) && (DefaultHttpPort != 21333)) {
		printf("TZO ERROR : Port <%d> is invalid, port 80 and 21333 supported\n", DefaultHttpPort) ;
		return EXIT_FATAL ;
	}

	/*
	 * Get the current external WAN IP address.
	 */
	if (TzoGetCurrentIP(szCurrrentIpAddress) != TRUE) {
		printf("TZO ERROR : There was a problem getting your current WAN IP address\n") ;
		//nvram_set("TZODDNSok", "There was a problem getting your current WAN IP address");
		return EXIT_FATAL ;
	}

	if (Verbose)
		printf("* Your Current Wan IP Address is <%s>\n", szCurrrentIpAddress) ;


	/*
	 * Need to update the IP Address on the servers.
	 */
	switch (ReturnVal = TZOLogon(szGlobalTZOKey, szGlobalEmailAddress, szGlobalDomainName, szCurrrentIpAddress, (char *)szReturnBuffer)) {
		case UPDATE_SUCCESS :
			nvram_set("TZODDNSok", "The update was successful, and the hostname is now updated");
			break;
		case UPDATE_NOCHANGE :
			if (Verbose)
				printf("* TzoLogon() : Success\n") ;
			nvram_set("TZODDNSok", "No Change in the IP Address");
			break ;
		case ERR_BAD_AUTH :
			nvram_set("TZODDNSok", "Bad Authentication - Username or Password");
			return EXIT_FATAL ;
		case ERR_NOT_AVAIL_CU :
			nvram_set("TZODDNSok", "An option available only to credited users");
			return EXIT_FATAL ;
		case ERR_BLOCKED_UPDATES :
			nvram_set("TZODDNSok", "The hostname specified is blocked for update abuse, please wait 1 minute");
			return EXIT_FATAL ;
		case ERR_NO_HOST_EXIST :
			nvram_set("TZODDNSok", "The hostname specified does not exis");
			return EXIT_FATAL ;
		case ERR_BLOCKED_AGENT :
			nvram_set("TZODDNSok", "The user agent that was sent has been blocked for not following specifications");
			return EXIT_FATAL ;
		case ERR_BAD_HOST_NAME :
			nvram_set("TZODDNSok", "The hostname specified is not a fully-qualified domain name");
			return EXIT_FATAL ;
		case ERR_HOST_MISMATCH :
			nvram_set("TZODDNSok", "The hostname specified exists, but not under the username specifiedd");
			return EXIT_FATAL ;
		case ERR_SYSTEM_TYPE :
			nvram_set("TZODDNSok", "Bad System type");
			return EXIT_FATAL ;
		case ERR_HOST_COUNT :
			nvram_set("TZODDNSok", "Too many or too few hosts found");
			return EXIT_FATAL ;
		case ERR_ACCOUNT_BLOCKED :
			nvram_set("TZODDNSok", "Blocked from updating");
			return EXIT_FATAL ;
		case ERR_ACCOUNT_TEMP_BLOCK :
			printf("TZO Warning : \n %s", szReturnBuffer) ;
			nvram_set("TZODDNSok", "Same IP address update in less than a minute");
			return EXIT_TEMP_BLOCK ;
		case ERR_ACCOUNT_EXPIRED :
			nvram_set("TZODDNSok", "The TZO account has expired");
			printf("TZO Error : \n %s", szReturnBuffer) ;
			return EXIT_EXPIRED ;
		default :
			/*
			 * Fatal error
			 */
			printf("TZO FATAL : \n %s", szReturnBuffer) ;
			return EXIT_FATAL ;
		}

	return EXIT_OK ;
}