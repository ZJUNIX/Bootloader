#ifndef _ATA_H
#define _ATA_H

#include "inttypes.h"

// address
#define ATA_PRIMARY_IO 0x1F0

// offset
#define ATA_REG_DATA 0x00
#define ATA_REG_SECCOUNT0 0x02
#define ATA_REG_LBA0 0x03
#define ATA_REG_LBA1 0x04
#define ATA_REG_LBA2 0x05
#define ATA_REG_HDDEVSEL 0x06
#define ATA_REG_COMMAND 0x07
#define ATA_REG_STATUS 0x07

// commands
#define ATA_CMD_IDENTIFY 0xEC

// status
#define ATA_SR_BSY 0x80
#define ATA_SR_DRQ 0x08
#define ATA_SR_ERR 0x01

// others
#define ATA_SELECT_DRIVE 0xA0

int ata_init();
void ata_lba_write(uint32_t lba, uint8_t num_sectors, uint16_t *buf);
void ata_lba_read(uint32_t lba, uint8_t num_sectors, uint8_t *buf);

#endif