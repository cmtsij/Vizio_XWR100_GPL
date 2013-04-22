/* -- updatedd: error.c --
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
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <netdb.h>
#include <syslog.h>
#include <signal.h>

#define MAXDATA	256
#define EXIT	1
#define CORE	2

#define STANDARD	0
#define PERROR		1
#define HERROR		2

extern int errno;
extern int h_errno;

struct error_s {
	int facility;
	char *pname;
	char *email;
} *err_s;

void if_segfault(void);
void segfault_exit(int sig);
void print_stderr(const int error_type, const char *message, va_list az);
void print_syslog(const int priority, const int errno_type, const char *message, va_list az);

void
init_error(struct error_s *ptr, const int facility, char *progname, char *email) {
	err_s = ptr;
	(void)memset(err_s, 0, sizeof(err_s));
	err_s->facility = facility;
	err_s->pname = progname;
	err_s->email = email;
	if_segfault();

	errno = 0;
	h_errno = 0;

	return;
}

void
error_f(const int instruction, const int error_type, const char *message, va_list az) {

//	va_list az;

//	va_start(az, message);
	print_stderr(error_type, message, az);
//	va_end(az);

	if(instruction == EXIT) {
		del_iptable_rule(); //GTK add : del iptable rule 20080220
	  exit(EXIT_FAILURE);
	}
	else if(instruction == CORE)
	    abort();
	else {
		errno = 0;
		h_errno = 0;
	}

	return;

}

void
log_error(const int priority, const int instruction, const int error_type, const char *message, va_list az) {

//	va_list az;

//	va_start(az, message);
	print_syslog(priority, error_type, message, az);
//	va_end(az);

	if(instruction == EXIT)
	    exit(EXIT_FAILURE);
	else if(instruction == CORE)
	    abort();
	else {
		errno = 0;
		h_errno = 0;
	}

	return;

}

void
if_segfault() {

	struct sigaction sig;

	sig.sa_handler = &segfault_exit;
	if(sigaction(SIGSEGV, &sig, NULL) == -1)
	    error_f(EXIT, STANDARD, "sigaction() failed", NULL);
	return;

}

void
segfault_exit(int sig) {

	sig = sig;
	(void)fprintf(stderr, 	"\nERROR: %s received SIGNAL SEGMENTATION VIOLATION.\n",
				err_s->pname);
	if(err_s->email != NULL)
	    (void)fprintf(stderr,   "Please send a bugreport to %s.\n\n",
				    err_s->email);
	else
	    (void)fprintf(stderr, "\n");

	abort();

}

void
print_stderr(const int error_type, const char *message, va_list az) {

	(void)fprintf(stderr, "%s: ", err_s->pname);
	(void)vfprintf(stderr, message, az);

	if(error_type == HERROR)
	    (void)fprintf(stderr, ": %s\n", hstrerror(h_errno));
	else if(error_type == PERROR)
	    (void)fprintf(stderr, ": %s\n", strerror(errno));
	else
	    (void)fprintf(stderr, "\n");

	(void)fflush(stderr);
	return;

}

void
print_syslog(const int priority, const int error_type, const char *message, va_list az) {

	openlog(err_s->pname, LOG_CONS, err_s->facility);

	syslog(priority, message, az);
	if(error_type == HERROR)
	    syslog(priority, "%s", hstrerror(h_errno));
	else if(error_type == PERROR)
	    syslog(priority, "%s", strerror(errno));

	closelog();
	return;

}
