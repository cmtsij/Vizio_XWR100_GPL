#ifndef	EventLog_H
#define	EventLog_H

#define	SERV_UDP_PORT	514
#define	EVENT_NUM	50
#define	MsgSize		200

void	check_eventlog_buffer ( void )
{
	FILE	*fp = NULL;
	int	event_num = EVENT_NUM , position = 0 , i = 0;
	char	event_buf [ EVENT_NUM ] [ MsgSize ] , *p = NULL , msg [ MsgSize ];

	if ( ( fp = fopen ( "/var/log/eventmsg" , "r" ) ) == NULL ) { return 0; }

	memset ( event_buf , '\0' , ( EVENT_NUM * MsgSize ) );
	while ( !feof ( fp ) ) {
		p = NULL;
		memset ( msg , '\0' , MsgSize );
		fgets ( msg , MsgSize , fp );
		if ( strlen ( msg ) >= ( MsgSize - 1 ) ) { return 0; }
		p = strstr ( msg , "##" );
		if ( p == NULL ) {}
		else {
			strcpy ( event_buf [ position++ ] , msg );
			//printf("event log[%d] check : broadcom.c : %s\n", position - 1, event_buf[position - 1]);
		}
	}
	fclose ( fp );

	if ( position == 0 ) { return; }

	if ( position >= event_num ) {
		if ( ( fp = fopen ( "/var/log/eventmsg" ,"w" ) ) == NULL ) {
			printf ( "open /var/log/eventmsg error !!!\n" );
			return	0;
		}
		for ( i = 0 ; i < event_num - 1 ; i++ ) {
			fprintf ( fp ,"%s\n" , event_buf [ i + 1 ] );
		}
		fclose ( fp );
	}

/*	} else {
		for ( i = 0 ; i < position ; i++ ) {
			fprintf ( fp ,"%s\n" , event_buf [ i ] );
		}
	}	*/
	return	0;
}

char	*GettingTimeStamp ( void )
{
	static char	bp [ 128 ];
	struct  tm		*rv_tm = NULL;
	struct  timeval	curtime;
	time_t		systime;

	static const char *months[] = {
             		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
             		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
     	};

	static const char *days[] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};

	check_eventlog_buffer ();

	memset ( &bp , '\0' , 128 );
	time ( &systime );
	rv_tm = gmtime ( &systime );

	sprintf ( bp ,"%s, %s %2d %2d:%02d:%02d %4d##" ,
		days[rv_tm->tm_wday], months[rv_tm->tm_mon], rv_tm->tm_mday, 
		rv_tm->tm_hour, rv_tm->tm_min, rv_tm->tm_sec,
		1900+rv_tm->tm_year );

	return	bp;
}


int ap_syslogd(char* msg)
{
 char *remote_ipaddr = NULL ;
 char *log_ipaddr_4 = NULL ;
 int sockfd ;
 struct sockaddr_in serv_addr, cli_addr ;
 int n ;
 
	if ( nvram_get ( "EVENTLOGENABLE" ) == NULL ) { return 1; }

 if(!strcmp( nvram_get("EVENTLOGENABLE"), "0"))
    return 1;

if ( nvram_get ( "log_ipaddr" ) == NULL ) { return 1; }

 remote_ipaddr = nvram_get( "log_ipaddr" );
 
 if( remote_ipaddr != NULL &&  strcmp(remote_ipaddr, "0.0.0.0") )
 { 
 
  bzero( (char *)&serv_addr, sizeof(serv_addr) ) ;
  serv_addr.sin_family = AF_INET ;
  serv_addr.sin_addr.s_addr = inet_addr(remote_ipaddr) ;
  serv_addr.sin_port = htons(SERV_UDP_PORT) ;
  
  if( (sockfd = socket( AF_INET, SOCK_DGRAM, 0 )) < 0 )
  {
   perror( "open a UDP socket fail " );
   return -1 ;
  }
  
  bzero( (char *)&cli_addr, sizeof(cli_addr) ) ;
  cli_addr.sin_family = AF_INET ;
  cli_addr.sin_addr.s_addr = htonl(INADDR_ANY) ;
  cli_addr.sin_port = htons(0) ;
  
  if( bind( sockfd, (struct sockaddr *)&cli_addr, sizeof(cli_addr)) < 0 )
  {
   perror( "client: can't bind local address " );
   return -1 ;
  }
 
  n = strlen(msg) ;
  
  if( sendto(sockfd, msg, n, 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != n )
  {
   perror( "sendto error " );
   return -1 ;
  }
  
  close(sockfd) ;
 }
 
 return 0;
}

char	*EVENTLOGENABLE1 = NULL;
/*
char	*EVENTLOGENABLE2 = NULL;
char	*EVENTLOGENABLE3 = NULL;
*/
void	write_eventlog ( int type , char *msg )
{
	int	fd = 0;
	FILE	*fp = NULL;

	if ( EVENTLOGENABLE1 == NULL ) {
		EVENTLOGENABLE1 = nvram_get ( "EVENTLOGENABLE1" );
		if ( EVENTLOGENABLE1 == NULL ) {
			return;
		}
	}

/*	if ( EVENTLOGENABLE2 == NULL ) {
		EVENTLOGENABLE2 = PeterChen_nvram_get ( "EVENTLOGENABLE2" );
		goto	PeterChenBaBa2;
	} else {
PeterChenBaBa2:	if ( EVENTLOGENABLE2 == NULL ) {
			return;
		}
	}

	if ( EVENTLOGENABLE3 == NULL ) {
		EVENTLOGENABLE3 = PeterChen_nvram_get ( "EVENTLOGENABLE3" );
		goto	PeterChenBaBa3;
	} else {
PeterChenBaBa3:	if ( EVENTLOGENABLE3 == NULL ) {
			return;
		}
	}

	if ( PeterChen_nvram_get ( "EVENTLOGENABLE1" ) == NULL ) { return; }
	if ( PeterChen_nvram_get ( "EVENTLOGENABLE2" ) == NULL ) { return; }
	if ( PeterChen_nvram_get ( "EVENTLOGENABLE3" ) == NULL ) { return; }

	if ( type == 0 && !strcmp ( PeterChen_nvram_get ( "EVENTLOGENABLE1" ) , "0" ) ) {
		return;
	} else if ( type == 1 && !strcmp ( PeterChen_nvram_get ( "EVENTLOGENABLE2" ) , "0" ) ) {
		return;
	} else if ( type == 2 && !strcmp ( PeterChen_nvram_get ( "EVENTLOGENABLE3" ) , "0" ) ) {
		return;
	}	*/

//	if ( type == 0 && !strcmp ( EVENTLOGENABLE1 , "0" ) ) {
	if ( !strcmp ( EVENTLOGENABLE1 , "0" ) ) {
		return;
	}

/*	} else if ( type == 1 && !strcmp ( EVENTLOGENABLE2 , "0" ) ) {
		return;
	} else if ( type == 2 && !strcmp ( EVENTLOGENABLE3 , "0" ) ) {
		return;
	}	*/

	check_eventlog_buffer ();

	fp = fopen ( "/var/log/eventmsg" ,"a+" );
	if ( fp == NULL ) {
		printf ( "open /var/log/eventmsg error !!! \n" );
		return;
	} else {
		fprintf ( fp ,"%d %s\n" , type, msg );
	}

	fclose ( fp );
	return;
}
#endif
