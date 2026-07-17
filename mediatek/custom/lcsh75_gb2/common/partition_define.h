#ifndef __PARTITION_DEFINE_H__
#define __PARTITION_DEFINE_H__


//Lenovo A690 NAND partition_define.h
#define PARTITION_LAYOUT_NAND_A690

#define KB  (1024)
#define MB  (1024 * KB)
#define GB  (1024 * MB)

#define PART_SIZE_PRELOADER			(256*KB)
#define PART_SIZE_DSP_BL			(768*KB)
#define PART_SIZE_NVRAM			(3072*KB)
#define PART_SIZE_SECCFG			(128*KB)
#define PART_SIZE_UBOOT			(384*KB)
#define PART_SIZE_BOOTIMG			(5120*KB)
#define PART_SIZE_RECOVERY			(5120*KB)
#define PART_SIZE_SEC_RO			(1152*KB)
#define PART_SIZE_MISC			(384*KB)
#define PART_SIZE_LOGO			(3072*KB)
#define PART_SIZE_EXPDB			(640*KB)
#define PART_SIZE_FAT			(5120*KB)
#define PART_SIZE_ANDROID			(302080*KB)
#define PART_SIZE_CACHE			(61440*KB)
#define PART_SIZE_USRDATA			(0*KB)
#define PART_SIZE_BMTPOOL			(0x50)

#define WRITE_SIZE_Byte		512

#define PART_NUM			15

#define MBR_START_ADDRESS_BYTE			(*1024)

typedef enum  {
	EMMC = 1,
	NAND = 2,
} dev_type;

typedef enum {
	USER = 0,
	BOOT_1,
	BOOT_2,
	RPMB,
	GP_1,
	GP_2,
	GP_3,
	GP_4,
} Region;

struct excel_info{
	char * name;
	unsigned long long size;
	unsigned long long start_address;
	dev_type type ;
	unsigned int partition_idx;
	Region region;
};
extern struct excel_info PartInfo[PART_NUM];


#endif
