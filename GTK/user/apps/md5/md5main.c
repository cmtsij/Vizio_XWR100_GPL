/*
  Copyright (C) 2002 Aladdin Enterprises.  All rights reserved.

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  L. Peter Deutsch
  ghost@aladdin.com

 */
/* $Id: md5main.c,v 1.1 2002/04/13 19:20:28 lpd Exp $ */
/*
  Independent implementation of MD5 (RFC 1321).

  This code implements the MD5 Algorithm defined in RFC 1321, whose
  text is available at
	http://www.ietf.org/rfc/rfc1321.txt
  The code is derived from the text of the RFC, including the test suite
  (section A.5) but excluding the rest of Appendix A.  It does not include
  any code or documentation that is identified in the RFC as being
  copyrighted.

  The original and principal author of md5.c is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):

  2002-04-13 lpd Splits off main program into a separate file, md5main.c.
 */

#include "md5.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <sys/mman.h>	//mmap
#include <sys/types.h>	// stat
#include <sys/stat.h>
#include <fcntl.h>
/*
 * This file builds an executable that performs various functions related
 * to the MD5 library.  Typical compilation:
 *	gcc -o md5main -lm md5main.c md5.c
 */
static const char *const usage = "\
Usage:\n\
    md5main --test		# run the self-test (A.5 of RFC 1321)\n\
    md5main --t-values		# print the T values for the library\n\
    md5main --version		# print the version of the package\n\
";
static const char *const version = "2002-04-13";

/* Run the self-test. */
static int
do_test(void)
{
    static const char *const test[7*2] = {
	"", "d41d8cd98f00b204e9800998ecf8427e",
	"a", "0cc175b9c0f1b6a831c399e269772661",
	"abc", "900150983cd24fb0d6963f7d28e17f72",
	"message digest", "f96b697d7cb7938d525a2f31aaf161d0",
	"abcdefghijklmnopqrstuvwxyz", "c3fcd3d76192e4007dfb496cca67e13b",
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
				"d174ab98d277d9f5a5611c2c9f419d9f",
	"12345678901234567890123456789012345678901234567890123456789012345678901234567890", "57edf4a22be3c955ac49da2e2107b67a"
    };
    int i;
    int status = 0;

    for (i = 0; i < 7*2; i += 2) {
	md5_state_t state;
	md5_byte_t digest[16];
	char hex_output[16*2 + 1];
	int di;

	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)test[i], strlen(test[i]));
	md5_finish(&state, digest);
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	if (strcmp(hex_output, test[i + 1])) {
	    printf("MD5 (\"%s\") = ", test[i]);
	    puts(hex_output);
	    printf("**** ERROR, should be: %s\n", test[i + 1]);
	    status = 1;
	}
    }
    if (status == 0)
	puts("md5 self-test completed successfully.");
    return status;
}

/* Print the T values. */
static int
do_t_values(void)
{
    int i;
    for (i = 1; i <= 64; ++i) {
	unsigned long v = (unsigned long)(4294967296.0 * fabs(sin((double)i)));

	/*
	 * The following nonsense is only to avoid compiler warnings about
	 * "integer constant is unsigned in ANSI C, signed with -traditional".
	 */
	if (v >> 31) {
	    printf("#define T%d /* 0x%08lx */ (T_MASK ^ 0x%08lx)\n", i,
		   v, (unsigned long)(unsigned int)(~v));
	} else {
	    printf("#define T%d    0x%08lx\n", i, v);
	}
    }
    return 0;
}

char* do_verify(char* file_path)
{
	int fd;
	if( (fd = open(file_path , O_RDWR)) == -1 )
	{
		printf("Can't open file:\"%s\" for MD5 checking \n", file_path);
		return "error";
	}
	struct stat sb;
	fstat(fd, &sb);

	unsigned char *start;
	if((start = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0) )== (void *)-1)
	{
		printf("mmap fail!\n");
		perror("mkimage, mmap ");
		return "error";
	}

	md5_state_t state;
	md5_byte_t digest[16];
	md5_init(&state);
	md5_append(&state, (const md5_byte_t *)start, sb.st_size);	
	md5_finish(&state, digest);

	int di;
	static char hex_output[16*2 + 1];
	for (di = 0; di < 16; ++di)
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
	
	printf("MD5 (\"%s\") \n", &hex_output);
	munmap(start, sb.st_size);
	return hex_output;
}

/* Main program */
int
main(int argc, char *argv[])
{
    if (argc == 2) {
	if (!strcmp(argv[1], "--test"))
	    return do_test();
	else if (!strcmp(argv[1], "--t-values"))
	    return do_t_values();
	else if (!strcmp(argv[1], "--version")) {
	    puts(version);
	    return 0;
	}
	//if(strstr(argv[1], "tmp"))
	else
	{
		FILE *fp=NULL;
		char tmp[strlen(argv[1])+4];
		char* tmp_addr=NULL;
		
		if(strrchr(argv[1],'/'))
		{
			tmp_addr = strrchr(argv[1],'/');
			tmp_addr++;
			sprintf(tmp, "/tmp/MD5_%s", tmp_addr);
		}
		else
		{
			sprintf(tmp, "/tmp/MD5_%s", argv[1]);
		}
		//printf("log MD5 to  %s\n", tmp);			//Remove: debug only
		
		if( (fp = fopen(tmp , "w")) == NULL )
		{
			printf("Can't write %s for checking MD5 \n", tmp);
			return 0;
		}
		fprintf(fp, "%s\n", do_verify(argv[1]));
		fclose(fp);
		//printf("Result = %s\n", do_verify(argv[1]));
		return 0;
	}
    }

    puts(usage);
    return 0;
}
