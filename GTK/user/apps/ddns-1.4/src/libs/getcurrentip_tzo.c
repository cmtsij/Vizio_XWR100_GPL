/***************************************************************************
                          main.c  -  description
                             -------------------
    begin                : Tue Jun 27 07:20:43 EDT 2006
    email                : mdf@tzo.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  Test Application to retrieve teh current WAN IP address                *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include  <vendor_default.h> //TZO_ECHO_SERVER

#include <netdb.h>
#include "../modules/get_connection.h"

typedef struct	{
	unsigned int ID ;
	unsigned int Request  ;
	unsigned int IPAddress  ;
	unsigned int Signature  ;
	} IPECHO_MESSAGE ;

#define IPECHO_PORT 	        21332
#define TZO_IPECHO_HEADER_SIZE  16
#define IPECHO_ID_SERVER 		0x2b4e7cd1
#define IPECHO_ID_CLIENT 		0x43f752e4
#define IPECHO_REQUEST_ECHO 	0x100
#define IPECHO_SIGNATURE_INIT 	0x3e2198a6

unsigned int FlipByteOrder(unsigned int Target) {
    int Result ;
    char * lpResult = (char *)&Result ;
    char * lpTarget = (char *)&Target ;

    lpResult[0] = lpTarget[3] ;
    lpResult[1] = lpTarget[2] ;
    lpResult[2] = lpTarget[1] ;
    lpResult[3] = lpTarget[0] ;

return Result ;
}


unsigned int CalcIPEchoSignature(IPECHO_MESSAGE * lpMsg) {
    unsigned int Result = IPECHO_SIGNATURE_INIT ;

    Result ^= lpMsg->ID ;
    Result += lpMsg->Request ;
    Result ^= lpMsg->IPAddress ;

    return (Result) ;
}


unsigned int FetchIPAddress(char *  HostName) {
    struct hostent *lpHostEnt ;
    unsigned int IPAddress ;
    char * lpIPAddress = (char *)&IPAddress ;
    int i ;

    if (HostName == NULL)
    {
    	printf("80 \n");
        return(-1) ;
    }    

    if ((IPAddress = inet_addr(HostName)) != INADDR_NONE)
        return IPAddress ;

   // MyGetHostByName 
     if ((lpHostEnt = MyGetHostByName(HostName)) == 0)
    //if ((lpHostEnt = gethostbyname(HostName)) == 0)
      //  if ((lpHostEnt = gethostbyname(HostName)) == 0)
        {
           printf("90 \n");
           return(-1) ;
        }    
            
      // 64.27.166.100    = 0x401BA664
      //lpHostEnt->h_addr[0] = 100;
      //lpHostEnt->h_addr[1] = 166;
     // lpHostEnt->h_addr[2] = 27;
     // lpHostEnt->h_addr[3] = 64;

    for (i=0; i<4; i++)
        lpIPAddress[i] = lpHostEnt->h_addr[i] ;

    return (IPAddress) ;
}

int GetIPString(char * lpString, int IPAddress)
	{
	struct in_addr InAddr ;
	InAddr.s_addr = IPAddress ;
	strcpy(lpString, inet_ntoa(InAddr)) ;
	return 1 ;
	}


int CheckForNewIPAddress(unsigned int *LocalAddr) {
    IPECHO_MESSAGE Msg ;
    int Socket ;
    fd_set SocketArray ;
    int Result, ResponseLength ;
    struct sockaddr_in sin;
    int Tries = 10 ;
    unsigned int EchoServerIPAddress ;
    struct timeval tv = {0,0}  ;
    int BytesSent ;

    //if ((EchoServerIPAddress = FetchIPAddress(ECHO_SERVER_TO_USE)) == 0)  {
    	if ((EchoServerIPAddress = FetchIPAddress(TZO_ECHO_SERVER)) == 0)  {
    	printf("115 EchoServerIPAddress = %d \n",EchoServerIPAddress);
        printf("Unable to contact TZO Echo Servers\n") ;
        return(-1) ;
    }
    printf("119 EchoServerIPAddress = %d \n",EchoServerIPAddress);

    if ((Socket = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("Unable to create a socket\n") ;
        return(-1) ;
    }

// test
    // EchoServerIPAddress = 0x401BA664;--->"No resonce from server\n"
     
    //  EchoServerIPAddress = 0x64A61B40 ;
     
    sin.sin_family = AF_INET ;
    sin.sin_addr.s_addr = EchoServerIPAddress ;
    
    sin.sin_port = htons(IPECHO_PORT) ;

    Msg.ID = IPECHO_ID_CLIENT ;
    Msg.Request = IPECHO_REQUEST_ECHO ;
    Msg.IPAddress = *LocalAddr ;
    Msg.Signature = CalcIPEchoSignature(&Msg) ;

    ResponseLength = 0 ;
    
    printf("LocalAddr =%d \n",Msg.IPAddress);

    if ((BytesSent = sendto(Socket, (unsigned char *)&Msg, sizeof(Msg), 0, (const struct sockaddr *)&sin, sizeof(sin))) < 0) {
    	 //sin.sin_addr.s_addr =0x64A61B40 ;//0x401BA664;
    	 //if ((BytesSent = sendto(Socket, (unsigned char *)&Msg, sizeof(Msg), 0, (const struct sockaddr *)&sin, sizeof(sin))) < 0) {
        printf("Unable to sendto socket\n") ;
        close(Socket) ;
        
        
        return(-1) ;
    }

    while (Tries--)  {
        tv.tv_sec = 0 ;
        tv.tv_usec = 0 ;
        FD_ZERO(&SocketArray) ;
        FD_SET(Socket, &SocketArray) ;
        tv.tv_sec = 1 ;
        tv.tv_usec = 0 ;
        Result = select(Socket+1, &SocketArray, NULL, NULL, &tv) ;

        if (Result)
            break ;
        sleep(1) ;
        }

    if (!Result) {
        printf("No resonce from server\n") ;
	    close(Socket) ;
        return(-1) ;
    }

    if ((ResponseLength = recvfrom(Socket, (unsigned char *)&Msg, 16, 0, 0, 0)) <= 0) {
        printf("Bad responce from server\n") ;
        close(Socket) ;
        return(-1) ;
    }

    close(Socket) ;

    if (ResponseLength == sizeof(IPECHO_MESSAGE)) {
        *LocalAddr = FlipByteOrder(Msg.IPAddress) ;
        return 1 ;
    }
printf("strange server response, are you connecting to the right server?\n") ;
  return(-1) ;
}


//int main(int argc, char *argv[])
int tzo_getip(unsigned int IPaddr)
{
static int pre_IpAddess=0;	
int nIpAddess = 0 ;
char szIpAddress[32] ;
//nIpAddess=IPaddr;// Arthur

  switch (CheckForNewIPAddress(&nIpAddess)) {
        case -1 :
            printf("There was an error communicating with the TZO Echo Servers\n") ;
            return 0;// fail to communicate with the TZO Echo Servers, the upper layer should update it's IP.
           // break;
        case 1 :
            GetIPString(szIpAddress, nIpAddess) ;
            printf("Current IP Address %s\n", szIpAddress) ;
            if (nIpAddess==pre_IpAddess)
              return 1; // same IP , don't need to update it .
            else
              pre_IpAddess= nIpAddess;
              return 0;  // set the new IP to pre_IpAddess,he upper layer should update it's IP
              
           // break;
        }        

  //return EXIT_SUCCESS;
}
