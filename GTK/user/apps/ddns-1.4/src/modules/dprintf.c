/* -- updatedd: dprintf.c --
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLEN	2048

int
dprintf(int s, char *fmt, ...) {

	char *buffer;
	int n;
	va_list va;

	va_start(va, fmt);
	n = vsprintf(NULL, fmt, va);
	if(n > MAXLEN)
	    error_f(EXIT, STANDARD, "dprintf() failed: string is too long");
	if((buffer = (char *)malloc((n+1) * sizeof(char))) == NULL)
	    error_f(EXIT, PERROR, "malloc() failed");
	(void)vsnprintf(buffer, n, fmt, va);
	*(buffer+n) = '\0';
	va_end(va);

	if(write(s, buffer, n) == -1)
	    n = -1;
	free(buffer);

	return n;

}
