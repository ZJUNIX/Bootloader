#ifndef _KERNEL_LOADER_H
#define _KERNEL_LOADER_H

// macro you may want to modify
#define LOAD_KERNEL_ADDRESS 0x80000000

// macros about FAT32
#define BITS_PER_BYTE 8
#define SECTOR_SIZE 512
#define ADDRESS_SIZE 4
#define BASE_NAME_LENGTH 8
#define EXTENDED_NAME_LENGTH 3
#define DPT_OFFSET 454
#define ROOT_CLUSTER_MAX 100
#define ROOT_CLUSTER_OFFSET 0x2C
#define SECTOR_RESERVED_OFFSET 0x0E
#define SECTOR_PER_CLUSTER_OFFSET 0x0D
#define FAT_COUNT_OFFSET 0x10
#define FAT_SIZE_OFFSET 0x24
#define ALL_LOWERCASE 0x18
#define TIME_LENGTH 2
#define DATE_LENGTH 2
#define END_SIGNAL 0x0FFFFFFF
#define BEGIN_CLUSTER 2
#define DENTRY_SIZE 32
#define DELETED 0xE5
#define ATTR_DISK 0x08
#define ATTR_LONG_FILE_NAME 0x0F

// other macros
#define READ_LENGTH_MAX 4
#define BASE_NAME_TO_FIND "KERNEL"
#define BASE_NAME_TO_FIND_LENGTH 6
#define EXTENDED_NAME_TO_FIND "BIN"
#define EXTENDED_NAME_TO_FIND_LENGTH 3

struct FAT32_basic_info {
	int base_addr;			    // sector address
	int sector_reserved;		    // sector offset
	int sector_per_cluster;		    // amount
	int FAT_base;			    // sector address
	int FAT_count;			    // amount
	int FAT_size;			    // sector
	int data_sector;		    // sector address
	int root_cluster_count;		    // number of root_cluster
	int root_cluster[ROOT_CLUSTER_MAX]; // root cluster list, <= 100 default
};

// FAT32 short file name dentry
struct FAT32_short_file_name_dentry {
	char base_name[BASE_NAME_LENGTH];
	char extended_name[EXTENDED_NAME_LENGTH];
	char attribute;
	char reserved;
	char msec_when_created;
	char created_time[TIME_LENGTH];
	char created_date[DATE_LENGTH];
	char accessed_date[DATE_LENGTH];
	char cluster_high[ADDRESS_SIZE / 2];
	char modified_time[TIME_LENGTH];
	char modified_date[DATE_LENGTH];
	char cluster_low[ADDRESS_SIZE / 2];
	unsigned int size;
};

struct target_file {
	int size;		       // in bytes
	char *data;		       // raw data
	int cluster_count;	     // number of clusters
	int cluster[ROOT_CLUSTER_MAX]; // cluster list, <= 100
};

int load_kernel();
int get_le(int, int, int);
void read_sector(char *, int, int, int);
int string_match(char *, char *, int);

#endif