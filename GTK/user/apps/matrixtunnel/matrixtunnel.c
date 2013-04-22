/*
 *	matrixtunnel.c
 *	Release $Name: MATRIXSSL_1_8_OPEN $
 *
 *	Simple example program for MatrixSSL
 *	Accepts a HTTPS request and echos the response back to the sender.
 */
/*
 *	Copyright (c) PeerSec Networks, 2002-2006. All Rights Reserved.
 *	The latest version of this code is available at http://www.matrixssl.org
 *
 *	This software is open source; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This General Public License does NOT permit incorporating this software 
 *	into proprietary programs.  If you are unable to comply with the GPL, a 
 *	commercial license for this software may be purchased from PeerSec Networks
 *	at http://www.peersec.com
 *	
 *	This program is distributed in WITHOUT ANY WARRANTY; without even the 
 *	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 *	See the GNU General Public License for more details.
 *	
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *	http://www.gnu.org/copyleft/gpl.html
 */
/******************************************************************************/

#define SYSLOG_NAMES

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <getopt.h>
#include <netdb.h>
#include <sys/select.h>
#include <syslog.h>
#include <stdarg.h>

/******************************************************************************/

#include <matrixSsl.h>
#include "sslSocket.h"

#define MAXPROXYCOUNT SOMAXCONN/2
#define USE_FORK 1

// FIXME. configurable?
#define RUNNING_DIR "/"

void dolog(int prio, const char *fmt, ...);

#ifdef NDEBUG
#define CLOG(_level, _format, _args...) dolog(_level,_format, ## _args);
#else
#define CLOG(_level, _format, _args...) dolog(_level,"%s:%d " _format, __FILE__, __LINE__, ## _args);
#endif

#define ELOG(_format, _args...) CLOG(LOG_ERR, _format, ## _args)
#define WLOG(_format, _args...) CLOG(LOG_WARNING, _format, ## _args)
#define NLOG(_format, _args...) CLOG(LOG_NOTICE, _format, ## _args)
#define ILOG(_format, _args...) CLOG(LOG_INFO, _format, ## _args)
#define DLOG(_format, _args...) CLOG(LOG_DEBUG, _format, ## _args)


struct proxyConnection {
	SOCKET		plain;
	
	sslConn_t	*secure;
	sslSessionId_t	*sessionId; // client mode
	short				cipherSuite;
	
	int				plain_up;		// connection is up
	int				secure_up;
	int				plain_eof;
	int				secure_eof;
	int				error;
	int				inuse;
	int				done;
};

int				isClient = 0;
sslKeys_t		*keys;
void*				cervalidator = NULL;
char				*dst_host = "localhost";
int				dst_port	= 0;
int				nosysl=0;
int				nofork=0;
int					quit=0;
char*			pidfile=NULL;
int gLogLevel = LOG_NOTICE;


#ifndef USE_FORK
struct proxyConnection	connections[MAXPROXYCOUNT];
#endif

extern char *optarg;
extern int optind, opterr, optopt;

static int certChecker(sslCertInfo_t *cert, void *arg);

struct timeval tonull={0,0};

void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

void kill_handler(int s)
{
	ILOG("caugth signal. exiting...");
	quit=1;
}

void usage(int status)
{
	fprintf(stderr,	"usage: [-c] [-v] [-d localip:port] [-r remoteip:port]\n\n"
					"    -A      Certificate Authority file \n"
					"    -p      private key and certificate chain PEM file name\n"
					"    -c      client mode. remote is ssl, local plain\n"
					"    -v      validate certificate\n"
					"    -d      listen locally on this [host:]port\n"
					"    -r      connect to remote machine on [host:]port\n"
					"    -P      pidfile\n"
					"    -f      foreground mode\n"
					"    -D      syslog level (0...7)\n"
					"                                  \n"
					"example:    matrixtunnel -A /etc/certSrv.pem -p /etc/privkeySrv.pem -d 443 -r 80\n"
					"debug mode: matrixtunnel -f -A /etc/certSrv.pem -p /etc/privkeySrv.pem -d 443 -r 80 -D 7 &\n");
	exit(status);
}

void
getprio(int pri, char *res, int reslen) {
	CODE *c_pri;
	
	c_pri = prioritynames;
	while (c_pri->c_name && !(c_pri->c_val == LOG_PRI(pri)))
		c_pri++;
	if(c_pri->c_name == NULL)
		snprintf(res, reslen, "<%d>", pri);
	else
		snprintf(res, reslen, "%s", c_pri->c_name);
}

void dolog(int prio, const char *fmt, ...)
{
	char *p;
	va_list ap;
	char	logprio[20];

	if(nosysl && prio <= gLogLevel) {
		getprio(prio,logprio,sizeof(logprio));
		fprintf(stderr,"%s ",logprio);
		va_start(ap, fmt);
		(void) vfprintf(stderr, fmt, ap);
		va_end(ap);
		fprintf(stderr,"\n");
	}
	else {
		va_start(ap, fmt);
		vsyslog(prio, fmt, ap);
		va_end(ap);
	}
}

void daemonize()
{
	int i,lfp;
	char str[10];
	
	if(getppid()==1) return; /* already a daemon */
	
	i=fork();
	if (i<0) exit(1); /* fork error */
	if (i>0) exit(0); /* parent exits */
	
	/* child (daemon) continues */
	setsid(); /* obtain a new process group */
	close(0);
	close(1);
	close(2);
	i=open("/dev/null",O_RDWR); dup(i); dup(i); /* handle standart I/O */
	umask(027); /* set newly created file permissions */
	chdir(RUNNING_DIR); /* change running directory */

	if(pidfile) {
		lfp=open(pidfile,O_RDWR|O_CREAT,0640);

		if (lfp<0) exit(1); /* can not open */

		if (lockf(lfp,F_TLOCK,0)<0) exit(0); /* can not lock */

		/* first instance continues */
		sprintf(str,"%d\n",getpid());

		write(lfp,str,strlen(str)); /* record pid to lockfile */
	}

	signal(SIGCHLD,sigchld_handler); /* ignore child */
	signal(SIGTSTP,SIG_IGN); /* ignore tty signals */
	signal(SIGTTOU,SIG_IGN);
	signal(SIGTTIN,SIG_IGN);
	signal(SIGHUP,kill_handler); /* catch hangup signal */
	signal(SIGTERM,kill_handler); /* catch kill signal */
}

void closeProxyConnection(struct proxyConnection *cp) {
	if(!(cp->error || cp->done) && !(cp->secure_eof && cp->plain_eof))
		return;
	
	DLOG("Closing connection");
	int secure_fd=cp->secure->fd;
	int status=0;
	if(cp->secure_up && !cp->error) {
		sslWriteClosureAlert(cp->secure);
	}
	status=close(cp->secure->fd);	
	socketShutdown(cp->secure->fd);
	sslFreeConnection(&cp->secure);
	
	status=close(cp->plain);
	socketShutdown(cp->plain);
	
	memset(cp,0,sizeof(struct proxyConnection));
	ILOG("Connection closed");
}

int
secureReady(struct proxyConnection *cp) {
	if (cp->secure->inbuf.buf) {
			DLOG("secure read: contents in inbuf");
		return 1;
	}
	
	fd_set	rs,ws;
		int	maxfd=cp->plain > cp->secure->fd ? cp->plain : cp->secure->fd;
		FD_ZERO(&rs);
		FD_ZERO(&ws);
		FD_SET(cp->secure->fd, &rs);
		FD_SET(cp->plain,&ws);
		if(select(maxfd+1, &rs, &ws, NULL, &tonull) < 0) {
			return 0;
		}
	DLOG("secure read: %d, plain write: %d", FD_ISSET(cp->secure->fd,&rs)>0, FD_ISSET(cp->plain,&ws)>0);
	return FD_ISSET(cp->secure->fd,&rs) && FD_ISSET(cp->plain,&ws);
}

int
plainReady(struct proxyConnection *cp) {
	fd_set	rs,ws;
    	int maxfd=cp->plain > cp->secure->fd ? cp->plain : cp->secure->fd;
		FD_ZERO(&rs);
		FD_ZERO(&ws);
		FD_SET(cp->plain, &rs);
		FD_SET(cp->secure->fd, &ws);
		if(select(maxfd+1, &rs, &ws, NULL, &tonull) < 1) {
			return 0;
		}
	DLOG("plain read: %d, secure write: %d", FD_ISSET(cp->plain,&rs)>0, FD_ISSET(cp->secure->fd,&ws)>0);
	return FD_ISSET(cp->plain,&rs) && FD_ISSET(cp->secure->fd,&ws);
}

static int
proxyReadwrite(struct proxyConnection *cp, int isServer)
{
	char buf[4096];
	int	status=0;
	int rc, total=0;
	int amt;
	int offset;
	
	char* sense = isServer ? "ssl->plain" : "plain->ssl";
	
	DLOG("%s reading",sense);
	while(1) {
		if(isServer) {
			if(!secureReady(cp)) {
				DLOG("%s connection not ready",sense);
				break;
			}
			rc = sslRead(cp->secure, buf, sizeof(buf), &status);
			
			if(status == SSLSOCKET_EOF || status == SSLSOCKET_CLOSE_NOTIFY) {
				DLOG("%s EOF. rc=%d, status=%d, errno=%d",sense,rc, status,errno);
				cp->done=1;
				return 0;
			}
		}
		else {
				if(!plainReady(cp)) {
					DLOG("%s connection not ready",sense);
				break;
			}
			rc = read(cp->plain, buf, sizeof(buf));
			
			if (rc == 0) {
				DLOG("%s EOF. rc=%d, status=%d, errno=%d",sense,rc, status,errno);
				cp->done=1;
				return 0;
			}
		}
		
		if (rc < 0) {
			if (errno == EINTR || errno == EAGAIN) //continue;
			{       
				/* Gemtek add +++ */
				errno=0;
				/* Gemtek add --- */
				continue;
			}
			cp->error=1;
			perror("read error");
			return -1;
		}
		
		total+=rc;
		DLOG("%s read %d bytes",sense,rc);
		offset = 0;
		amt = rc;
		
		while (amt>0) {
			DLOG("%s writing",sense);
			if(isServer)
				rc = write(cp->plain, buf+offset, amt);
			else
				rc = sslWrite(cp->secure, buf+offset, amt, &status);
			if (rc < 0) {
				if (errno == EINTR || errno == EAGAIN) //continue;
				{
                                        /* Gemtek add +++ */
				        errno=0;
					continue;
                                        /* Gemtek add --- */
				}
				cp->error=1;
				perror("write error");
				return -1;
			}
			DLOG("%s written %d bytes. amt=%d",sense, rc, amt);
			offset += rc;
			amt -= rc;
		}
	}
	return total;
}

void
proxyAccept(int srv_fd,	struct proxyConnection	*ncp)
{
	SOCKET			src_fd, dst_fd;
//	struct linger linger;
	int				status;
		int			res;
		struct timeval tv;
		int pc, sc;
		int			ccount=0;
		
	ILOG("Accepting new connection.");
	
	if ((src_fd = socketAccept(srv_fd, &status)) == INVALID_SOCKET) {
		WLOG("Error accepting connection: %d", status);
		return;
	}

	#ifdef USE_FORK
		// fork here
		fd_set	rs, ws, es;
		int fdmax;

		int pid=fork();
	  switch(pid) {
	  case -1:    /* error */
	      closesocket(ncp->plain);
	      closesocket(ncp->secure->fd);
	      break;

	  case  0:    /* child */
  #endif

	ncp->inuse=1;

	if (!isClient) {
		DLOG("Trying to accept ssl connection");
		if (sslAccept(&ncp->secure, src_fd, keys, cervalidator, 0)) {
			WLOG("Error could not establish ssl connection");
			socketShutdown(src_fd);
			/* Gemtek add +++ */
#ifdef USE_FORK
      quit=1;
#endif
      /* Gemtek add --- */
			return;
		}
	}

	setSocketNonblock(src_fd);
/*
	linger.l_onoff = 1;
	linger.l_linger = 1;
	setsockopt(src_fd, SOL_SOCKET, SO_LINGER, (char *) &linger,sizeof(linger));
*/
	/* try to connect to remote end of tunnel */
	DLOG("Trying to connect server %s:%d", dst_host, dst_port);
	if ((dst_fd = socketConnect(dst_host, dst_port, &status)) == INVALID_SOCKET) {
		WLOG("Error connecting to server %s:%d", dst_host, dst_port);
		socketShutdown(src_fd);
		/* Gemtek add +++ */
#ifdef USE_FORK
    quit=1;
#endif
    /* Gemtek add --- */
		return;
	}
	
	if (isClient) {
		DLOG("Trying to establish an ssl connection to server %s:%d", dst_host, dst_port);
		if ((sslConnect(&ncp->secure, dst_fd, keys, ncp->sessionId, ncp->cipherSuite, cervalidator)) == INVALID_SOCKET) {
			WLOG("Error connecting to ssl server %s:%d", dst_host, dst_port);
			socketShutdown(src_fd);
			return;
		}
	}
	setSocketNonblock(dst_fd);
/*
	linger.l_onoff = 1;
	linger.l_linger = 1;
	setsockopt(dst_fd, SOL_SOCKET, SO_LINGER, (char *) &linger,sizeof(linger));
*/
	ncp->plain = isClient ? src_fd : dst_fd;
	ncp->plain_up=1;
	ncp->secure_up=1;
	ILOG("Connection established. plain_fd:%d secure_fd:%d", ncp->plain, ncp->secure->fd);	
	
	// handle remaining bytes in buffer.
	/*
	res=proxyReadwrite(ncp,1);
	if(res<0) {
		ncp->error=1;
		closeProxyConnection(ncp);
	}
	*/
	// fork here
#ifdef USE_FORK
		
		closesocket(srv_fd);
		fdmax=ncp->plain > ncp->secure->fd ? ncp->plain : ncp->secure->fd;
		while (!quit) {
			FD_ZERO(&rs);
			FD_ZERO(&ws);
			FD_ZERO(&es);
			
				FD_SET(ncp->plain,&rs);
				FD_SET(ncp->plain,&ws);
				FD_SET(ncp->plain,&es);
				FD_SET(ncp->secure->fd,&rs);
				FD_SET(ncp->secure->fd,&ws);
				FD_SET(ncp->secure->fd,&es);
			
			
			tv.tv_sec=10;
			tv.tv_usec=0;

			DLOG("select on %d open connections. fdmax: %d", ccount, fdmax);
			res=select(fdmax+1,&rs,NULL,&es,&tv);
//			DLOG("select returned: %d", res);
			DLOG("proxyAccept->select returned: %d %s errno=%d", res , strerror(errno), errno );

			if(res<0) {
				perror("select");
				continue;
			}
/*
			if(res==0)
				continue;
*/
			if(FD_ISSET(ncp->secure->fd,&es) || FD_ISSET(ncp->plain,&es)) {
				ncp->error=1;
				break;
			}

			sc=proxyReadwrite(ncp,1);
			if(sc<0) break;
			pc=proxyReadwrite(ncp,0);
			if(pc<0) break;
			
			if(ncp->done) {
				quit=1;
				break;
			}
		}
		closeProxyConnection(ncp);
		DLOG("done. exiting...");
		exit(0);
		break;

	default:
      closesocket(src_fd);
							/* server */
		break;
	}
#endif
}

/******************************************************************************/
/*
	Stub for a user-level certificate validator.  Just using
	the default validation value here.
*/
static int certChecker(sslCertInfo_t *cert, void *arg)
{
	sslCertInfo_t	*next;
/*
	Make sure we are checking the last cert in the chain
*/
	next = cert;
	while (next->next != NULL) {
		next = next->next;
	}
#if ENFORCE_CERT_VALIDATION
/*
	This case passes the true RSA authentication status through
*/
	return next->verified;
#else
/*
	This case passes an authenticated server through, but flags a
	non-authenticated server correctly.  The user can call the
	matrixSslGetAnonStatus later to see the status of this connection.
*/
	if (next->verified != 1) {
		return 1;//SSL_ALLOW_ANON_CONNECTION;
	}
	return next->verified;
#endif /* ENFORCE_CERT_VALIDATION */
}		

/******************************************************************************/
/*
	This example application acts as an https server that accepts incoming
	client requests and reflects incoming data back to that client.  
*/
#if VXWORKS
int _matrixtunnel(char *arg1)
#elif WINCE
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPWSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char **argv)
#endif
{
	SOCKET			srv_fd;
	int				status;
	WSADATA			wsaData;
	
	// options
	char				*srv_host = NULL;
	int				srv_port = 0;
	char 				*keyfile = NULL; //"privkeySrv.pem";
	char 				*certfile = NULL; //"certSrv.pem";
	int				vlevel = 0;
	char 				*cpos,*opos;
	int tmpport;
	int c;
	int 			intarg;
 	
#if VXWORKS
	int					argc;
	char				**argv;
	parseCmdLineArgs(arg1, &argc, &argv);
#endif /* VXWORKS */

#if WINCE
	int					argc;
	char				**argv;
	char				args[256];

/*
 *	parseCmdLineArgs expects an ASCII string and CE is unicoded, so convert
 *	the command line.  args will get hacked up, so you can't pass in a
 *	static string.
 */
	WideCharToMultiByte(CP_ACP, 0, lpCmdLine, -1, args, 256, NULL, NULL);

/*
 *	Parse the command line into an argv array.  This allocs memory, so
 *	we have to free argv when we're done.
 */
	parseCmdLineArgs(args, &argc, &argv);
#endif /* WINCE */


/*
	prepare
*/
	
#ifndef USE_FORK
	memset(connections,0,MAXPROXYCOUNT*sizeof(struct proxyConnection));
#endif
	
/*
	getopt
*/
	/* Gemtek add +++ */
	if(argc == 1) usage(1);
	/* Gemtek add --- */
	
	for (;;) {
		c = getopt (argc, argv, "VD:P:fo:cd:r:p:A:v:h");
		if (c == -1) {
			break;
		}

		switch (c) {
			case 'c':
				// client mode
				isClient=1;
				break;
			
			case 'd':
				// daemon mode [host:]port
				cpos = NULL;
				tmpport = 0;
				if((cpos = strchr(optarg,':'))) {				
					*cpos = '\0';
					if(optarg && optarg[0])
						srv_host = optarg;
					optarg = ++cpos;
				}
				if(optarg && optarg[0]) {
					tmpport = (int)strtol(optarg, (char **)NULL, 0);
					if(tmpport) srv_port = tmpport;
				}
				break;
			
			case 'r':
				// remote [host:]port
				cpos = NULL;
				tmpport = 0;
				if((cpos = strchr(optarg,':'))) {				
					*cpos = '\0';
					if(optarg && optarg[0])
						dst_host = optarg;
					optarg = ++cpos;
				}
				if(optarg && optarg[0]) {
					tmpport = (int)strtol(optarg, (char **)NULL, 0);
					if(tmpport) dst_port = tmpport;
				}
				break;
				
			case 'p':
				// pemfile (requred in servermode)
				keyfile = optarg;
				break;
			
			case 'A':
				// CA file
				certfile = optarg;
				break;
			
			case 'v':
				// veryfication level
				if(optarg && optarg[0]) {
					vlevel = (int)strtol(optarg, (char **)NULL, 0);
					if(vlevel == 1 ) {
						cervalidator = certChecker;
					}
					else if(vlevel > 3 || vlevel < 0) {
						fprintf(stderr,"-v takes whole numbers between 0 and 3");
						exit(2);
					}
				}
				break;
			
			case 'P':
				// create a pidfile
				pidfile=optarg;
				break;
				
			case 'f':
				// run in foreground.
				nofork=1;
				nosysl=1;
				break;
				
			case 'o':
				// append logmessages to a file instead of stdout/syslog
				break;
				
			case 'O':
				// socket options. TODO
				break;
			
			case 'D':
				// debug level 0...7
				intarg=strtol(optarg,NULL,0);
				if(intarg<0 || intarg>7) {
					usage(1);
				}
				gLogLevel=intarg;
				break;
				
			case 'V':
				// version
				break;
				
			case '?':
			case 'h':
				usage(0);
				break;
			
			default:
				usage(1);
				break;
		}
	}


/* install handlers */
	signal( SIGPIPE, SIG_IGN );
	signal(SIGCHLD,sigchld_handler); /* ignore child */
	signal(SIGHUP,kill_handler); /* catch hangup signal */
	signal(SIGTERM,kill_handler); /* catch kill signal */
	
/*
	Initialize Windows sockets (no-op on other platforms)
*/
	WSAStartup(MAKEWORD(1,1), &wsaData);
	
	if(!nosysl) {
		openlog("matrixtunnel", LOG_PID, LOG_DAEMON);
		setlogmask(LOG_UPTO(gLogLevel));
	}
	
/*
	Initialize the MatrixSSL Library, and read in the public key (certificate)
	and private key.
*/
	if (matrixSslOpen() < 0) {
		ELOG("matrixSslOpen failed, exiting...");
		exit(1);
	}

/*
	Standard PEM files
*/
	if (matrixSslReadKeys(&keys, certfile, keyfile, NULL, NULL) < 0)  {
		ELOG("Error reading or parsing %s or %s, exiting...", 
			certfile, keyfile);
		exit(1);
	}

	// go to background
	if(!nofork) {
		daemonize();
	}

/*
	Create the listen socket
*/
	if ((srv_fd = socketListen(srv_port, &status)) == INVALID_SOCKET) {
		ELOG("Cannot listen on port %d, exiting...", srv_port);
		exit(1);
	}
/*
	Set blocking or not on the listen socket
*/
	setSocketBlock(srv_fd);

/*
	Main connection loop
*/
	struct proxyConnection	*cp=NULL;
	struct proxyConnection	*ncp;
	
	fd_set	rs, ws, es, cr;
	int			fdmax;
	struct timeval tv;
	int			res, dontClose;
	
	char		buf[4096];
	int pc, sc;
	
	int			ccount;
	
	while (!quit) {
		fdmax=srv_fd;
		ncp=NULL;
		
		FD_ZERO(&rs);
		FD_ZERO(&ws);
		FD_ZERO(&es);
		
		FD_SET(srv_fd,&rs);
		FD_SET(srv_fd,&ws);
		FD_SET(srv_fd,&es);
		ccount=0;
		
#ifndef USE_FORK
		DLOG("next select on fds: %d ",srv_fd);
		for(cp=connections;cp<&connections[MAXPROXYCOUNT];cp++) {
			if (cp->done) {
				closeProxyConnection(cp);
			}
			if (cp->secure_up) {
				FD_SET(cp->secure->fd,&rs);
				FD_SET(cp->secure->fd,&ws);
				FD_SET(cp->secure->fd,&es);

				if (fdmax < cp->secure->fd)
					fdmax = cp->secure->fd;
				
				DLOG("fd: %d",cp->secure->fd);
				ccount++;
			}
			if (cp->plain_up) {
				FD_SET(cp->plain,&rs);			
				FD_SET(cp->plain,&ws);
				FD_SET(cp->plain,&es);
					
				if (fdmax < cp->plain)
					fdmax = cp->plain;
				
				DLOG("fd: %d",cp->plain);
				ccount++;
			}
			if(!ncp && !cp->inuse){
				ncp=cp;
				memset(ncp,0,sizeof(struct proxyConnection));
			}
		}
#else
		struct proxyConnection	ncp_s;
		ncp=&ncp_s;
		memset(ncp,0,sizeof(struct proxyConnection));
#endif
		
		tv.tv_sec=10;
		tv.tv_usec=0;

		DLOG("main : select on %d open connections. fdmax: %d", ccount, fdmax);
		res=select(fdmax+1,&rs,NULL,&es,&tv);
		DLOG("select returned: %d %s", res , strerror(errno) );

		if(res<0) {
			perror("select");
			continue;
		}
		
		if(res==0)
			continue;

#ifndef USE_FORK
		// handle open connections
		for(cp=connections;cp<&connections[MAXPROXYCOUNT];cp++) {
			if (cp->secure_up && cp->plain_up) {
				if(FD_ISSET(cp->secure->fd,&es) || FD_ISSET(cp->plain,&es)) {
						closeProxyConnection(cp);
						continue;
				}
				
				if(secureReady(cp)) {
					sc=proxyReadwrite(cp,1);
					if(sc<0) {
						closeProxyConnection(cp);
						continue;
					}
				}
				
				if(plainReady(cp)) {
					pc=proxyReadwrite(cp,0);
					if(pc<0) {
						closeProxyConnection(cp);
						continue;
					}
				}
			}
		}
#endif

		// do we have new connections?
		if(FD_ISSET(srv_fd,&rs)) {
			proxyAccept(srv_fd,ncp);
		}			
	}

/*
	Close listening socket, free remaining items
*/
	socketShutdown(srv_fd);
	
#ifndef USE_FORK
	for(cp=connections;cp<&connections[MAXPROXYCOUNT];cp++) {
		closeProxyConnection(cp);
	}
#endif
	
	if(!nosysl) {
		closelog();
	}
	
	matrixSslFreeKeys(keys);
	matrixSslClose();
	WSACleanup();
	return 0;
}

/******************************************************************************/


