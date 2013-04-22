#ifndef __NVRAM_H
#define __NVRAM_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sysdep.h>

#define NVRAM_SPACE		0x10000

#define NVRAM_MTDPATH		"/dev/mtd4"
#define FACTORY_MTDPATH	"/dev/mtd5"
#define SHM_KEY			55365
//#define SHM_NVRAM_MTD_KEY	59865
#define MAX_ENTRY		2048				/* max entries of nvram parameters */
#define MAX_EMPTY_THRESHOLD	20

#define NVSERV_BUFSIZE	  256
#define NVRAM_MAXNAMESZ		32				/* Remember to minus 1 for NULL '\0' */
#define NVRAM_MAXVALUESZ	(NVSERV_BUFSIZE - 4)		/* Remember to minus 1 for NULL '\0' */

struct nventry
{
	char *name;
	char *value;
	unsigned int val_space;
};

struct def_nventry
{
	char name[NVRAM_MAXNAMESZ];
	char value[NVRAM_MAXVALUESZ];
	unsigned flag;
};

#define DONTCARE		0   //parameter must exist
#define FIXVALU		  1   //parameter must exist and value can't be changed

typedef struct def_nventry	def_entry_t;
typedef struct nventry	nvram_entry_t;

struct shmnvram
{
	volatile unsigned char writing;
	volatile unsigned char created;
	char *nvram_buf_end;
	struct nventry nvram_entry[MAX_ENTRY];
	unsigned char nvram_buf[NVRAM_SPACE];
};

char* nvram_get(char *name );
int nvram_get2buf(char *name,char *value );
int nvram_set(char *name, char *value);
int nvram_unset(char *name);
int nvram_commit(void);
int nvram_restore(void);
int nvram_clear(void);
int nvram_list_all(void);
int nvram_update(void);
int nvram_match(char *id1, char *id2);  //match the value of id1 and return 1 if matched .
int nvram_invmatch(char *name, char *invmatch);
int nvram_set_hex( char *name, unsigned char *value , int vlen);

int get_factory_to_nvram(void);
//type 0 set serial pin language_str fw_md5 to factory
//type 1 set all parameter to factory
int set_factory_from_nvram(int type);

#define nvram_safe_get(name) (nvram_get(name) ? : "")

#endif
