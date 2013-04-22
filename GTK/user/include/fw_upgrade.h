
#ifndef __FW_UPGRADE_H__
#define __FW_UPGRADE_H__

#define GMTK_HDR "VIZIO_247N" 
#define GMTK_LANG_HDR "VIZIO247LG" 
#define FW_PATH "/tmp/firmware.bin"
#define FIRMWARE_MTDPATH		"/dev/mtd3"
#define LANGPACK_MTDPATH		"/dev/mtd6"
#define KERNEL_MTDPATH			"/dev/mtd2"
//#define FIS_MTDPATH							"/dev/mtd6"
//#define REDBOOTCF_MTDPATH				"/dev/mtd7"
#define KERNEL_IMG_PATH		"/tmp/kernel.img"
#define ROOTFS_IMG_PATH		"/tmp/rootfs.img"
#define LANGPACK_IMG_PATH		"/tmp/langpack.img"
enum 
{
	E_FWCRC=100,
	E_FWMGI,
	E_FWID,
	E_FWNOMEN,
	E_FWSIZE
};

typedef struct
{
	UINT32		checksum;		/* CRC32 */
	UINT8			magic[11];		/* The value of GTIMG_MAGIC */
	UINT32		kernel_length;		/* The length of the kernel image */
	//UINT32		kernel_entry_point;	/* Kernel's entry point for RedBoot's information */
	UINT32		kernel_upgrade_flag;	/* Set to 1 if we need to upgrade the kernel parition of the Flash */
	UINT32		rootfs_length;		/* The length of the rootfs image */
	//UINT32		rootfs_entry_point;	/* Not in use */
	UINT32		rootfs_upgrade_flag;	/* Set to 1 if we need to upgrade the rootfs parition of the Flash */

	// Add 3 items by Vic Yu, 2006-05/10
	UINT32		kernel_checksum;
	UINT32		rootfs_checksum;
	UINT32		fw_totalsize;
	UINT32		reserved[4];
}imghdr_t , *pLinuxFWHeader_T;

typedef struct
{
	unsigned char	magic [ 10 ];
	unsigned long	checksum;
	unsigned long totalsize;
	unsigned long langDesc_Len;
	char 	      langDesc[129];
}langhdr_t , *pLangHeader_T;

int check_fw_header(char *fw_img, int fw_len);
int write_to_flash();
void stop_application();

#endif
