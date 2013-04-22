/****************************************************************************************************************
* This is a NTPCLIENT.c control program 
* currently , it accessed 32 time servers
* when system detected "WAN started" , then begin to send ntp packet , until it gets response from the time server.
* at the begining, the period is 1 --> 2 --> 4 --> 8 -->15 -->30 --> 60secs -->" programmer define "
* ( time servers on 13 groups)
* note :in my case:
* 13 * (1+2+4+8+15+30+60+90+90+120+120+120+120+120) = 13 * 900 secs = 195 mins 
* 13 * sleep(2) *max_cycle  ~ 4 mins
* for performance , it will send ntp packet to 2~3 time servers at the same time ,
* and the system  must  get response from time server in about 3 hrs . ( this control program will stop sending packet 
* after the time )
*
* 1. The one who want to modify the ntp packet cycle can just modify "cycle_period[]"
* 2. The one who need the forever loop can modify the "if" condition :  if(cycle_no>=max_cycle)
*  V1.0
*  Arthur Lin  , Gemtek 
*  2003.8.21
*
****************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>

#include <linux/fcntl.h>
#include <linux/stat.h>
#include <unistd.h>

#include <time.h>
#include <sys/time.h>
#define   T_ZONE		8

int check_time(void);
extern int getntp_main(int argc, char *argv[]);

#include <bcmnvram.h>

int main (int argc, char * argv[])
{

	int 	ret , i;
	char	trytime=0;
	unsigned int cycle_period[]={1,2,4,8,15,30,60,90,120,120,120,120,120};
	char cmdbuf[128];
	char *time_server[]={
									"192.53.103.104",
									"129.69.1.153",
									"216.152.68.16",
									"204.123.2.5",
									"128.118.46.3",
									"129.6.15.29",
									"133.100.9.2",
									"207.200.81.113",
									"209.81.9.7",
									"143.232.55.5",
									"128.250.36.2",
									"204.123.2.5",
									"137.92.140.80",
									"128.250.36.3",
									"200.19.119.69",
									"142.3.100.2",
									"136.159.2.254",
									"128.233.3.100",
									"128.233.3.101",
									"193.5.216.14",
									"129.132.2.21",
									"146.83.8.200",
									"200.68.60.246",
									"195.113.144.201",
									"131.188.3.220",
									"131.188.3.221",
									"131.188.3.222",
									"131.188.3.223",
									"130.149.17.21",
									"130.149.17.8",
									"192.53.103.103",
									"212.242.86.186",
									"192.38.7.240",
									(char*)0,
									};
				char *gargv[7];
				
				gargv[0]="Getntp";
				gargv[1]="-h";
				gargv[2]=NULL;
				gargv[3]="-s";
				gargv[4]="-i";
				gargv[5]="3";
				gargv[6]=NULL;
			
			
									
#ifdef NTP_WITH_DEFAULT_SERVER
			if(strlen(nvram_safe_get("ntp_server1")) > 0)
			{
				if(!strcmp(nvram_safe_get("ntp_server1"), "Other")) {
					fprintf(stderr,"exeNtp : Get 1st ntp server : %s\n", nvram_safe_get("ntp_server_other1"));
					//sprintf( cmdbuf ,"/bin/Getntp -h %s -s -i 3 &", nvram_safe_get("ntp_server_other1"));
					gargv[2]=nvram_safe_get("ntp_server_other1");
				}
				else {
					fprintf(stderr,"exeNtp : Get 1st ntp server : %s\n", nvram_safe_get("ntp_server1"));
					//sprintf( cmdbuf ,"/bin/Getntp -h %s -s -i 3 &", nvram_safe_get("ntp_server1"));
					gargv[2]=nvram_safe_get("ntp_server1");
				}
			  //system ( cmdbuf );
			  getntp_main( 6 , gargv );	
			}
			
			if(strlen(nvram_safe_get("ntp_server2")) > 0)
			{
				if(!strcmp(nvram_safe_get("ntp_server2"), "Other")) {
					fprintf(stderr,"exeNtp : Get 1st ntp server : %s\n", nvram_safe_get("ntp_server_other2"));
					//sprintf( cmdbuf ,"/bin/Getntp -h %s -s -i 3 &", nvram_safe_get("ntp_server_other2"));
					gargv[2]=nvram_safe_get("ntp_server_other2");
				}
				else {
					fprintf(stderr,"exeNtp : Get 2nd ntp server : %s\n", nvram_safe_get("ntp_server2"));				
					//sprintf( cmdbuf ,"/bin/Getntp -h %s -s -i 3 &", nvram_safe_get("ntp_server2"));
					gargv[2]=nvram_safe_get("ntp_server2");
				}
			  //system ( cmdbuf );
			  getntp_main( 6 , gargv );	
			}
			
			if(strlen(nvram_safe_get("ntp_server3")) > 0)
			{
				fprintf(stderr,"exeNtp : Get 3rd ntp server : %s\n", nvram_safe_get("ntp_server3"));				
				//sprintf( cmdbuf ,"/bin/Getntp -h %s -s -i 3 &", nvram_safe_get("ntp_server3"));
			  //system ( cmdbuf );
			  gargv[2]=nvram_safe_get("ntp_server3");
			  getntp_main( 6 , gargv );	
			}			   
#endif			

									
			for ( i = 0 ;time_server[i] != NULL ; i++ )
			{	
				gargv[2]=time_server[i];
/*				
				sprintf( cmdbuf ,"/bin/Getntp -h %s -s -i 3 &",time_server[i] );					
      	system ( cmdbuf );
*/      				
        //fprintf(stderr , "%s" , gargv[2]);
				//fprintf(stderr ,"%s %s %s %s %s %s \n" ,gargv[0],gargv[1], gargv[2],gargv[3],gargv[4],gargv[5]);
				
				getntp_main( 6 , gargv );	
      	//sleep(5);
      	if( check_time() )// Time is greater than 1970
      	{
      			  printf("got time \n");
      				break;
      	}
      }          
                    
	return 0;
}

int check_time(void)
{

	struct 	tm			*rv_tm;
	struct 	timezone	curtz;
	struct 	timeval		curtime;
	time_t	timesec , timenow;


	gettimeofday(&curtime, &curtz);
	timesec = curtime.tv_sec;
	timesec = timesec + (3600 * T_ZONE);
	rv_tm = gmtime(&timesec);
	
	timenow = time( (time_t*) 0);
	//printf(" The year is %d \n", (rv_tm->tm_year+1900));
	if ((rv_tm->tm_year+1900) == 1970)
		return 0;
	else
		return 1;

}

