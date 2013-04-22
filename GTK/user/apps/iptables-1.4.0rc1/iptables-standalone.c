#define	_GTK_Added_
/*
 * Author: Paul.Russell@rustcorp.com.au and mneuling@radlogic.com.au
 *
 * Based on the ipchains code by Paul Russell and Michael Neuling
 *
 * (C) 2000-2002 by the netfilter coreteam <coreteam@netfilter.org>:
 * 		    Paul 'Rusty' Russell <rusty@rustcorp.com.au>
 * 		    Marc Boucher <marc+nf@mbsi.ca>
 * 		    James Morris <jmorris@intercode.com.au>
 * 		    Harald Welte <laforge@gnumonks.org>
 * 		    Jozsef Kadlecsik <kadlec@blackhole.kfki.hu>
 *
 *	iptables -- IP firewall administration for kernels with
 *	firewall table (aimed for the 2.3 kernels)
 *
 *	See the accompanying manual page iptables(8) for information
 *	about proper usage of this program.
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iptables.h>

#ifdef		_GTK_Added_
static int		gtk_iptables_batch ( unsigned char *patch_file_name )
{
	#define		MAX_IPTC_HANDLES		1024
	#define		MAX_IPT_ARGS			128
	#define		Empty_Character			' '
	#define		Quotation_Character			'\''
	static char		buf [ 1024 ];		/* must hold longest iptables command line */
	static char		*argv [ MAX_IPT_ARGS + 1 ];
	char			*cp = NULL ,*cp_blank=NULL , *cp_quotation=NULL;
	int				argc = 0 , ret = 0;
	char			*table [ MAX_IPTC_HANDLES ];
	iptc_handle_t	handle [ MAX_IPTC_HANDLES ];
	int				i = 0 , j = 0;
	FILE			*fp = NULL;

	if ( NULL == patch_file_name ) {
		printf ( "!!!!!!!!!! %s : NULL == patch_file_name ... failed !!!!!!!!!!\n" , __FUNCTION__ );
		exit ( 0 );
	}
	if ( 0 == strlen ( patch_file_name ) ) {
		printf ( "!!!!!!!!!! %s : 0 == strlen ( patch_file_name ) ... failed !!!!!!!!!!\n" , __FUNCTION__ );
		exit ( 0 );
	}

	for ( i = 0 ; i < MAX_IPTC_HANDLES ; i++ ) {
		table [ i ] = 0;
		handle [ i ] = 0;
	}

	if ( NULL == ( fp = fopen ( patch_file_name , "r" ) ) ) {
		printf ( "!!!!!!!!!! %s : fopen ... failed !!!!!!!!!!\n" , __FUNCTION__ );
		exit ( 0 );
	}

	memset ( buf , '\0' , sizeof ( buf ) );
	while ( fgets ( buf , sizeof ( buf ) , fp ) ) {
		if ( NULL != ( cp = strchr ( buf , ';' ) ) ) {
Weird:		if ( NULL != ( cp = strchr ( buf , '\n' ) ) ) { *(cp) = '\0'; }
			if ( NULL != ( cp = strstr ( buf , "echo " ) ) ) {
				cp = cp + strlen ( "echo " );
				printf ( "%s\n" , cp );
				memset ( buf , '\0' , sizeof ( buf ) );
				continue;
			}
			printf ( "%s : weird cmd (%s) ... To launch it by system\n" , __FUNCTION__ , buf );
			system ( buf );
			memset ( buf , '\0' , sizeof ( buf ) );
			continue;
		} else if ( NULL != ( cp = strchr ( buf , '\n' ) ) ) {
			if ( cp > buf ) {
				unsigned char		MyBuf [ 1024 ];
				memset ( MyBuf , '\0' , sizeof ( MyBuf ) );
				if ( ( ( cp - buf ) > 0 ) && ( ( cp - buf ) < sizeof ( MyBuf ) ) ) {
					memcpy ( MyBuf , buf , ( cp - buf ) );
					if ( NULL != ( cp = strchr ( MyBuf , Empty_Character ) ) ) {
						*(cp) = '\0';
						if ( NULL == strstr ( MyBuf , "iptables" ) ) {
							goto	Weird;
						}
					}
				}
			} else if ( cp <= buf ) {
				if ( '\n' != *( buf + 0 ) ) {
					printf ( "%s : weird cmd --- %s\n" , __FUNCTION__ , buf );
				}
				memset ( buf , '\0' , sizeof ( buf ) );
				continue;
			}
		}

		if ( ( cp = strchr ( buf , '\n' ) ) ) {
			*cp = '\0';
			if ( ( cp != buf ) && ( *( cp - 1 ) == Empty_Character ) ) {
				*( cp - 1 ) = '\0';
			}
		}

		for ( cp = buf , argc = 0 ; ( cp && ( argc < MAX_IPT_ARGS ) ) ; argc++ ) {
			argv [ argc ] = cp;
			cp_quotation = strchr( cp , Quotation_Character );
			cp_blank = strchr ( cp , Empty_Character );
			
			if( NULL != cp_quotation && ( cp_blank - cp_quotation ) > 0 )
			{
				/*
				We got a quotation mark , and it is closer than the next blank.
				Skip the blank check.
				*/
				cp = strchr( cp_quotation+1 , Quotation_Character );
			}		
			if ( ( cp = strchr ( cp , Empty_Character ) ) ) {
				*cp++ = '\0';

				j = 0;
				while ( Empty_Character == *cp ) {
					*cp++ = '\0';
					if ( '\0' == *cp ) { break; }
					if ( j++ >= 1024 ) { break; }
				}
			}
		}

		if ( cp && ( argc >= MAX_IPT_ARGS ) ) {
			printf ( "!!!!!!!!!!!!!! gtk_iptables_batch: too many args for batch processor !!!!!!!!!!!!!!\n" );
			exit ( 1 );
		}
		argv [ argc ] = NULL;

		ret = do_command ( argc , argv , &table , &handle );
		if (ret)
			ret = iptc_commit(&handle);
	
		if (!ret) {
			fprintf(stderr, "gtk_iptables_batch: %s\n",
				iptc_strerror(errno));
			if (errno == EAGAIN) {
				exit(RESOURCE_PROBLEM);
			}
		}
		memset ( buf , '\0' , sizeof ( buf ) );
	}

	if ( NULL != fp ) {
		fclose ( fp );
		fp = NULL;
	}

	/*if ( ret ) {
		for ( i = 0 ; i < MAX_IPTC_HANDLES ; i++ ) {
			if ( handle [ i ] ) {
				iptc_commit ( &handle [ i ] );
			}
		}
	}*/

	return	0;
	#define MAX_IPTC_HANDLES  6
}
#endif		/* end of _GTK_Added_ */


#ifdef IPTABLES_MULTI
int
iptables_main(int argc, char *argv[])
#else
int
main(int argc, char *argv[])
#endif
{
	int ret;
	char *table = "filter";
	iptc_handle_t handle = NULL;

	program_name = "iptables";
	program_version = IPTABLES_VERSION;

	lib_dir = getenv("IPTABLES_LIB_DIR");
	if (!lib_dir)
		lib_dir = IPT_LIB_DIR;

#ifdef NO_SHARED_LIBS
	init_extensions();
#endif
#ifdef		_GTK_Added_
	if ( strstr ( argv [ 0 ] , "gtk_iptables_batch" ) ) {
		if ( 2 == argc ) {
			if ( NULL != argv [ 1 ] ) {
				if ( strlen ( argv [ 1 ] ) > 0 ) {
					gtk_iptables_batch ( argv [ 1 ] );
				}
			}
		}
		exit ( 0 );
	}
#endif		/* end of _GTK_Added_ */
	ret = do_command(argc, argv, &table, &handle);
	if (ret)
		ret = iptc_commit(&handle);

	if (!ret) {
		fprintf(stderr, "iptables: %s\n",
			iptc_strerror(errno));
		if (errno == EAGAIN) {
			exit(RESOURCE_PROBLEM);
		}
	}

	exit(!ret);
}
