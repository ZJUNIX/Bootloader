#include "kernel_loader.h"
#include "ata.h"

int get_le(int sector, int offset, int byte)
{
	// Read in little-endian manner. Length can be 1, 2 or 4 bytes.
	int i;
	int result = 0;
	uint8_t ch[READ_LENGTH_MAX];
	uint8_t sector_data[SECTOR_SIZE];

	ata_lba_read(sector, 1, sector_data);
	for (i = 0; i < byte; i++) {
		ch[i] = sector_data[offset + i];
	}

	for (i = 0; i < byte; i++)
		result += (*(ch + i)) << (BITS_PER_BYTE * i);

	return result;
}

void read_sector(char *buf, int sector, int offset, int byte)
{
	int i;
	uint8_t sector_data[SECTOR_SIZE];
	ata_lba_read(sector, 1, sector_data);

	for (i = 0; i < byte; i++)
		buf[i] = sector_data[offset + i];
}

int string_match(char *s1, char *s2, int len)
{
	int i;

	for (i = 0; i < len; i++)
		if (s1[i] != s2[i])
			return 0;
	return 1;
}

int load_kernel()
{
	int next_cluster, current_sector_num, current_cluster_num, bytes_to_read, found = 0, load_done = 0;
	uint8_t current_sector_data[SECTOR_SIZE];
	struct FAT32_basic_info fat;

	char *cursor;
	struct target_file kernel_bin;
	struct FAT32_short_file_name_dentry *dentry;

	// Read MBR to get the base address of FAT32
	fat.base_addr = get_le(0, DPT_OFFSET, 4);

	// Read basic infomation about FAT32
	fat.sector_reserved = get_le(fat.base_addr, SECTOR_RESERVED_OFFSET, 2);
	fat.sector_per_cluster = get_le(fat.base_addr, SECTOR_PER_CLUSTER_OFFSET, 1);
	fat.FAT_base = fat.base_addr + fat.sector_reserved;
	fat.FAT_count = get_le(fat.base_addr, FAT_COUNT_OFFSET, 1);
	fat.FAT_size = get_le(fat.base_addr, FAT_SIZE_OFFSET, 4);
	fat.data_sector = fat.FAT_base + fat.FAT_count * fat.FAT_size;
	fat.root_cluster_count = 0;
	fat.root_cluster[fat.root_cluster_count++] = get_le(fat.base_addr, ROOT_CLUSTER_OFFSET, 4);

	// Find all the clusters belonging to root directory
	next_cluster = get_le(fat.FAT_base + fat.root_cluster[0] / (SECTOR_SIZE / ADDRESS_SIZE),
			      fat.root_cluster[0] % (SECTOR_SIZE / ADDRESS_SIZE) * ADDRESS_SIZE, ADDRESS_SIZE);

	while (next_cluster != END_SIGNAL) {
		fat.root_cluster[fat.root_cluster_count++] = next_cluster;
		next_cluster = get_le(fat.FAT_base + next_cluster / (SECTOR_SIZE / ADDRESS_SIZE),
				      next_cluster % (SECTOR_SIZE / ADDRESS_SIZE) * ADDRESS_SIZE, ADDRESS_SIZE);
	}

	// Find kernel.bin in each cluster of root
	for (int i = 0; i < fat.root_cluster_count; i++) {

		// for each sector in a cluster
		current_cluster_num = fat.root_cluster[i];
		for (int j = 0; j < fat.sector_per_cluster; j++) {
			current_sector_num = fat.data_sector + (current_cluster_num - BEGIN_CLUSTER) * fat.sector_per_cluster + j;

			read_sector(current_sector_data, current_sector_num, 0, SECTOR_SIZE);

			// for each sector in a cluster
			for (dentry = (struct FAT32_short_file_name_dentry *)current_sector_data;
			     (uint8_t *)dentry != current_sector_data + SECTOR_SIZE; dentry += 1) {

				if (dentry->attribute == ATTR_DISK || dentry->attribute == ATTR_LONG_FILE_NAME
				    || dentry->base_name[0] == DELETED)
					continue;

				if (dentry->base_name[0] == '\0')
					break;

				if (string_match(dentry->base_name, BASE_NAME_TO_FIND, BASE_NAME_TO_FIND_LENGTH)
				    && string_match(dentry->extended_name, EXTENDED_NAME_TO_FIND, EXTENDED_NAME_TO_FIND_LENGTH)
				    && dentry->reserved == ALL_LOWERCASE)
					found = 1;

				if (found)
					goto find_finish;
			}
		}
	}

find_finish:
	// find kernel.bin, load it into main memory
	if (found) {
		kernel_bin.size = dentry->size;
		kernel_bin.data = (char *)LOAD_KERNEL_ADDRESS;
		kernel_bin.cluster_count = 0;
		kernel_bin.cluster[kernel_bin.cluster_count++] = (dentry->cluster_high[0] << BITS_PER_BYTE * 2)
								 + (dentry->cluster_high[1] << BITS_PER_BYTE * 3) + dentry->cluster_low[0]
								 + (dentry->cluster_low[1] << BITS_PER_BYTE);

		// Find clusters belonging to kernel.bin
		next_cluster = get_le(fat.FAT_base + kernel_bin.cluster[0] / (SECTOR_SIZE / ADDRESS_SIZE),
				      kernel_bin.cluster[0] % (SECTOR_SIZE / ADDRESS_SIZE) * ADDRESS_SIZE, ADDRESS_SIZE);

		while (next_cluster != END_SIGNAL) {
			kernel_bin.cluster[kernel_bin.cluster_count++] = next_cluster;
			next_cluster = get_le(fat.FAT_base + next_cluster / (SECTOR_SIZE / ADDRESS_SIZE),
					      next_cluster % (SECTOR_SIZE / ADDRESS_SIZE) * ADDRESS_SIZE, ADDRESS_SIZE);
		}

		// Finally read all data of kernel.bin ; for each cluster
		cursor = kernel_bin.data;
		bytes_to_read = kernel_bin.size;

		for (int i = 0; i < kernel_bin.cluster_count; i++) {
			current_cluster_num = kernel_bin.cluster[i];

			// for each sector
			for (int j = 0; j < fat.sector_per_cluster; j++, cursor += SECTOR_SIZE, bytes_to_read -= SECTOR_SIZE) {
				current_sector_num = fat.data_sector + (current_cluster_num - BEGIN_CLUSTER) * fat.sector_per_cluster + j;

				if (bytes_to_read < SECTOR_SIZE) {
					read_sector(cursor, current_sector_num, 0, bytes_to_read);
					load_done = 1;
				} else
					read_sector(cursor, current_sector_num, 0, SECTOR_SIZE);

				if (load_done == 1)
					break;
			}

			if (load_done == 1)
				break;
		}
	} else {
		// not found
		while (1)
			;
	}

	return 0;
}