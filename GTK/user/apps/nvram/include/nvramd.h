#ifndef __NVRAMD_H
#define __NVRAMD_H

#include <nvram.h>

#define NVRAM_MTDSIZE		NVRAM_SPACE	/* The size of the MTD that contains the NVRam parameters */
#define NVRAM_DATASIZE		NVRAM_SPACE	/* The size of NVRam data in the NVRam MTD */
#define FACTORY_MTDSIZE		NVRAM_SPACE	/* The size of the MTD that contains the NVRam parameters */
#define FACTORY_DATASIZE		NVRAM_SPACE	/* The size of NVRam data in the NVRam MTD */
//#define NVRAM_MTDSIZE		(128 * 1024)	/* The size of the MTD that contains the NVRam parameters */
//#define NVRAM_DATASIZE		( 128 * 1024)	/* The size of NVRam data in the NVRam MTD */
#define NVRAM_MAGIC		"NVRM"
#define FACTORY_MAGIC		"FACTORY"

typedef struct
{
	UINT8	magic[4];	/* It should always be NVRM */
	UINT32	checksum;	/* CRC32 for NVRAM */
	UINT32	entrynum;	/* The total number for entry elements inside the NVRAM */
	UINT32	nvramlen;	/* The data length of NVRAM data */
} nvram_hdr_t;

typedef struct
{
	UINT8	magic[8];	/* It should always be NVRM */
	UINT32	checksum;	/* CRC32 for NVRAM */
	UINT32	datalen;	/* The data length of NVRAM data */
	UINT8   factory_wrote; /*a flag to indicate the factory value has been writen*/
	char		serial[16]; /*serail number of this product */
	char		pin[9]; /*device pin of this product */
	char		language_str[4]; /*Default language string */
	char		fw_md5[33]; /* Firmwar MD5 infomation */
	char		WPA2[11]; /* Security Key for wireless ath0 */
	char		WPA[11]; /* Security Key for wireless ath1 */
	char		WEP[11]; /* Security Key for wireless ath2 */
} factory_t;


#define NVRAM_ENTRY_SZ(entry)	(strlen((entry)->name) + 1 + strlen((entry)->value) + 1)

int nvramd_init(void);
int nvramd_exit(void);
int nvramd_restore(void);
int nvramd_set(char *name, char *value);
int nvramd_unset(char *name);
int nvramd_commit(void);
const char *nvramd_get(char *name);

#endif
