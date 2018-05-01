#include "ata.h"
#include "io.h"
#include "vga.h"

void ata_delay()
{
	uint32_t cycle = 0x2000;
	while (cycle--)
		iportb(ATA_PRIMARY_IO + ATA_REG_STATUS);
}

void ata_delay_poll(uint32_t io)
{
	ata_delay();

	while (iportb(io + ATA_REG_STATUS) & ATA_SR_BSY)
		;

	while (!(iportb(io + ATA_REG_STATUS) & ATA_SR_DRQ))
		;
}
void ata_poll(uint32_t io)
{
	while (iportb(io + ATA_REG_STATUS) & ATA_SR_BSY)
		;

	while (!(iportb(io + ATA_REG_STATUS) & ATA_SR_DRQ))
		;
}

int ata_identify_master()
{
	uint8_t status = 0;
	uint32_t io = ATA_PRIMARY_IO;

	// select drive
	oportb(io + ATA_REG_HDDEVSEL, ATA_SELECT_DRIVE);

	// necessary reset
	oportb(io + ATA_REG_SECCOUNT0, 0);
	oportb(io + ATA_REG_LBA0, 0);
	oportb(io + ATA_REG_LBA1, 0);
	oportb(io + ATA_REG_LBA2, 0);

	// send identify signal
	oportb(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	status = iportb(io + ATA_REG_STATUS);

	if (status) {
		// the primary device exists
		ata_poll(io);

		uint16_t *tmp_buf = (uint16_t *)0x80000000;
		for (int i = 0; i < 256; i++)
			*(tmp_buf + i * 2) = iportw(io + ATA_REG_DATA);

		return 0;
	} else {
		// primary device does not exist
		return 1;
	}
}

int ata_init()
{
	int ret = ata_identify_master();
	// halt if no master drive
	if (ret != 0)
		while (1)
			;
}

void ata_lba_write(uint32_t lba, uint8_t num_sectors, uint16_t *buf)
{
	uint32_t io = ATA_PRIMARY_IO;

	// Send drive and bit 24 - 27 of LBA
	oportb(io + ATA_REG_HDDEVSEL, ((uint8_t)(lba >> 24)) | 0xE0);

	// Send number of sectors
	oportb(io + ATA_REG_SECCOUNT0, num_sectors);

	// Send lba bit 0 - 23 byte by byte
	oportb(io + ATA_REG_LBA0, (uint8_t)lba);
	oportb(io + ATA_REG_LBA1, (uint8_t)(lba >> 8));
	oportb(io + ATA_REG_LBA2, (uint8_t)(lba >> 16));

	// Send command: write with retry
	oportb(io + ATA_REG_COMMAND, 0x30);

	for (int sec = 0; sec < num_sectors; sec++) {
		// Check status, wait for ready
		ata_delay_poll(io);

		// Send data
		for (int i = 0; i < 256; i++) {
			oportw(io + ATA_REG_DATA, *(buf + i + 256 * sec));
			// Clear cache by sending 0xE7
			oportb(io + ATA_REG_COMMAND, 0xE7);
		}
		ata_delay();
	}
}

void ata_lba_read(uint32_t lba, uint8_t num_sectors, uint8_t *buf)
{
	uint32_t io = ATA_PRIMARY_IO;

	// Send drive and bit 24 - 27 of LBA
	oportb(io + ATA_REG_HDDEVSEL, ((uint8_t)(lba >> 24)) | 0xE0);

	// Send number of sectors
	oportb(io + ATA_REG_SECCOUNT0, num_sectors);

	// Send lba bit 0 - 23 byte by byte
	oportb(io + ATA_REG_LBA0, (uint8_t)lba);
	oportb(io + ATA_REG_LBA1, (uint8_t)(lba >> 8));
	oportb(io + ATA_REG_LBA2, (uint8_t)(lba >> 16));

	// Send command: read with retry
	oportb(io + ATA_REG_COMMAND, 0x20);

	for (int sec = 0; sec < num_sectors; sec++) {
		// Check status, wait for ready
		ata_poll(io);

		// Read data
		uint16_t data = 0;
		uint16_t *addr = 0;
		for (int i = 0; i < 256; i++) {
			data = iportw(io + ATA_REG_DATA);
			addr = (uint16_t *)(buf + (i + 256 * sec) * 2);
			*addr = data;
		}
	}
}