/* -- updatedd: base64encode.c --
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

static char table64[]=
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void base64encode(char *intext, char *output)
{
     unsigned char ibuf[3];
     unsigned char obuf[4];
     int i;
     int inputparts;
   
     while(*intext) {
	    for (i = inputparts = 0; i < 3; i++) { 
	             if(*intext) {
			        inputparts++;
			        ibuf[i] = *intext;
			        intext++;
		     }
	             else
		         ibuf[i] = 0;
	    }
	
	    obuf [0] = (ibuf [0] & 0xFC) >> 2;
	    obuf [1] = ((ibuf [0] & 0x03) << 4) | ((ibuf [1] & 0xF0) >> 4);
	    obuf [2] = ((ibuf [1] & 0x0F) << 2) | ((ibuf [2] & 0xC0) >> 6);
	    obuf [3] = ibuf [2] & 0x3F;
	
	    switch(inputparts) {
	     case 1: /* only one byte read */
	               sprintf(output, "%c%c==", 
			                   table64[obuf[0]],
			                   table64[obuf[1]]);
	               break;
	     case 2: /* two bytes read */
	               sprintf(output, "%c%c%c=", 
			                   table64[obuf[0]],
			                   table64[obuf[1]],
			                   table64[obuf[2]]);
	               break;
	     default:
	               sprintf(output, "%c%c%c%c", 
			                   table64[obuf[0]],
			                   table64[obuf[1]],
			                   table64[obuf[2]],
			                   table64[obuf[3]] );
	               break;
	    }
	    output += 4;
     }
     *output=0;
}
