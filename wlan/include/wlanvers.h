/*
 * Copyright (c) 2009, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: //depot/sw/releases/7.3_AP/wlan/include/wlanvers.h#2 $
 */

#ifndef _WLANVERS_H_
#define _WLANVERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Global versioning information for Atheros driver/application builds */

/*
 * The version number is made up of major, minor, patch and build
 * numbers.  The build and release script will set the build number
 * using a Perforce counter.  Here the build number is set to 9999 so
 * that builds done without the build-release script are easily
 * identifiable.
 */

#define ATH_SW_VER_MAJOR      0
#define ATH_SW_VER_MINOR      0
#define ATH_SW_VER_PATCH      0
#define ATH_SW_VER_BUILD      9999

/*CopyRight (c) year information.*/
#define ATH_SW_COPYRIGHT_CURRENT_YEAR   "Copyright (C) 2001-2006 Atheros Communications, Inc."
#define ATH_SW_COPYRIGHT_CURRENT_YEAR_DRIVER "2001-2006 Atheros Communications, Inc. All Rights Reserved."
#define CSCO_SW_COPYRIGHT_CURRENT_YEAR   "Copyright (C) 2001-2006 Cisco."
#define CSCO_SW_COPYRIGHT_CURRENT_YEAR_DRIVER "2001-2006 Cisco. All Rights Reserved."

#define ATH_SW_COMPANY_NAME   "Atheros Communications, Inc."
#define ATH_SW_PRODUCT_NAME   "Driver for Atheros CB42/CB43/MB42/MB43 Network Adapter"
#define ATH_SW_FILE_DESC      "Atheros Extensible Wireless LAN device driver"
#define CSCO_SW_COMPANY_NAME   "Cisco."
#define CSCO_SW_PRODUCT_NAME   "Driver for Cisco Network Adapters"
#define CSCO_SW_FILE_DESC      "Cisco Extensible Wireless LAN device driver"

#define ATH_SW_FILE_NAME             "ATHR.SYS"
#define ATH_SW_ORIGINALFILE_NAME     "ATHR.SYS"
#define CSCO_SW_FILE_NAME             "CSCO21V.SYS"
#define CSCO_SW_ORIGINALFILE_NAME     "CSCO21V.SYS"

/*
 * Since no expansion is done when the "stringification" operator is specified,
 * we have to use two macros to do what we we want (to put quotes around our
 * defined constants, above).  The argument of ATH_XSTR() is expanded at prescan
 * to be ATH_STR(<some_number>) which is expanded to a number and then fed into
 * ATH_STR() to be stringified.  Gotta love precompilers.
 */
#define ATH_STR(x)            #x
#define ATH_XSTR(x)           ATH_STR(x)

#define ATHEROS_SW_MAJOR      ATH_XSTR(ATH_SW_VER_MAJOR)
#define ATHEROS_SW_MINOR      ATH_XSTR(ATH_SW_VER_MINOR)
#define ATHEROS_SW_PATCH      ATH_XSTR(ATH_SW_VER_PATCH)
#define ATHEROS_SW_BUILD      ATH_XSTR(ATH_SW_VER_BUILD)

#define ATHEROS_SW_VERSION    ATHEROS_SW_MAJOR "." ATHEROS_SW_MINOR "." \
                              ATHEROS_SW_PATCH "." ATHEROS_SW_BUILD

/* The following two definitions are used in NDIS *.rc files */
#define ATH_VERS_STRING       ATHEROS_SW_VERSION "\0"
#define ATH_VERS_NUMERIC      ATH_SW_VER_MAJOR, ATH_SW_VER_MINOR, \
                              ATH_SW_VER_PATCH, ATH_SW_VER_BUILD



#ifdef __cplusplus
}
#endif

#endif /* _WLANVERS_H_ */
