/* -- updatedd: get_ifaddr.c --
 *
 * Copyright (c) 2002 Philipp Benner <philipp@philippb.tk>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The GNU C Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */


#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <net/if.h>
#include <arpa/inet.h>

unsigned long int
get_ifaddr(char *ifname) {

	int	s;
	struct	ifreq ifr;
	struct	sockaddr_in sa;

	(void)strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if((s = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	    return 0;

	if(s >= 0) {
		if(ioctl(s, SIOCGIFADDR, &ifr)) {
			(void)close(s);
			return 0;
		}

		(void)close(s);
		(void)memcpy(&sa, &ifr.ifr_addr, 16);
		return(sa.sin_addr.s_addr);
	} else {
		(void)close(s);
		return 0;
	}

}
