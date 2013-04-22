/********************************************************************
 * Mini-RADIUS Server 1.0                                           *
 *                                                                  *
 * This server is intended to used in embedded system,              *
 * so just make as simple as it can.                                *
 * In this version, it only supports EAP-MD5.                       *
 *                                                                  *
 *                                                       2004.03.30 *
 *                                Dante Su (dante_su@gemtek.com.tw) *
 ********************************************************************/
 
#ifndef _SYSDEP_H
#define _SYSDEP_H

// This system uses 32Bit CPU.
#ifndef UINT32
#define UINT32		unsigned int
#endif

#ifndef INT32
#define INT32		int
#endif

#ifndef UINT16
#define UINT16		unsigned short
#endif

#ifndef INT16
#define INT16		short
#endif

#ifndef UINT8
#define UINT8		unsigned char
#endif

#ifndef INT8
#define INT8		char
#endif

#ifdef NVDEBUG
#define PINFO(fmt, args...)		printf(fmt, ## args)
#define PDEBUG(fmt, args...)		printf(fmt, ## args)
//#define PERROR(fmt, args...)		printf(fmt, ## args)
#else
#define PINFO(fmt, args...)		printf(fmt, ## args)
#define PDEBUG(fmt, args...)		do { } while(0)
//#define PERROR(fmt, args...)		printf(fmt, ## args)
#endif

#endif
