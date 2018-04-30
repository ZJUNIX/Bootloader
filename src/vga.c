#include "vga.h"
#include "inttypes.h"
#include "io.h"
#include "vga_configs.h"
#include "vga_fonts.h"

void write_reg(vga_regs port, uint8_t index, uint8_t data)
{
	uint8_t prev_index; // For interrupt-safe
	switch (port) {
	case MISC:
		oportb(VGA_MISC_WRITE, data);
		break;
	case AC:
		iportb(VGA_AC_RESET);
		prev_index = iportb(VGA_AC_INDEX);
		oportb(VGA_AC_INDEX, index);
		oportb(VGA_AC_WRITE, data);
		oportb(VGA_AC_INDEX, prev_index);
		break;
	default:
		prev_index = iportb((uint32_t)port);
		oportb((uint32_t)port, index);
		oportb((uint32_t)port + 1, data);
		oportb((uint32_t)port, prev_index);
		break;
	}
}
uint8_t read_reg(vga_regs port, uint8_t index)
{
	uint8_t prev_index;
	uint8_t data;
	switch (port) {
	case AC:
		iportb(VGA_AC_RESET);
		prev_index = iportb(VGA_AC_INDEX);
		oportb(VGA_AC_INDEX, index);
		data = iportb(VGA_AC_READ);
		oportb(VGA_AC_INDEX, prev_index);
		break;
	default:
		prev_index = iportb((uint32_t)port);
		oportb((uint32_t)port, index);
		data = iportb((uint32_t)port + 1);
		oportb((uint32_t)port, prev_index);
		break;
	}
	return data;
}

uint32_t get_fb_seg()
{
	uint32_t seg_index;
	oportb(VGA_GC_INDEX, 6);
	seg_index = iportb(VGA_GC_READ);
	/* index = bit[2:3] */
	seg_index = (seg_index >> 2) & 3;

	uint32_t seg_addr;
	switch (seg_index) {
	case 0:
	/* pass through */
	case 1:
		seg_addr = 0xa0000;
		break;
	case 2:
		seg_addr = 0xb0000;
		break;
	case 3:
		seg_addr = 0xb8000;
		break;
	}
	seg_addr |= 0xb0000000;
	return seg_addr;
}

void memcpy(uint8_t *dest, uint8_t *src, uint32_t count)
{
	for (uint32_t i = 0; i < count; i++) {
		*dest = *src;
		dest++;
		src++;
	}
}

void write_vram(uint32_t addr_offset, uint8_t *data, uint32_t count)
{
	uint8_t *addr = (uint8_t *)get_fb_seg();
	addr += addr_offset;
	memcpy(addr, data, count);
}

void set_palette()
{
	/* set the PEL mask */
	oportb(0x3C6, 0xFF);

	/* set whole dac away, from 0 */
	oportb(VGA_DAC_WRITE_INDEX, 0x00);

	for (uint32_t i = 0; i < 0x0100; i++) {
		if (i <= 0x3f) {
			oportb(VGA_DAC_WRITE, palette[i * 3 + 0]);
			oportb(VGA_DAC_WRITE, palette[i * 3 + 1]);
			oportb(VGA_DAC_WRITE, palette[i * 3 + 2]);
		} else {
			oportb(VGA_DAC_WRITE, 0);
			oportb(VGA_DAC_WRITE, 0);
			oportb(VGA_DAC_WRITE, 0);
		}
	}
}

void get_font_access()
{
	write_reg(SEQ, 0X00, 0X01);
	write_reg(SEQ, 0x02, 0x04);
	write_reg(SEQ, 0x04, 0x07);
	write_reg(SEQ, 0x00, 0x03);

	write_reg(GC, 0x04, 0x02);
	write_reg(GC, 0x05, 0x00);
	write_reg(GC, 0x06, 0x04);
}

void release_font_access()
{
	write_reg(SEQ, 0x00, 0x01);
	write_reg(SEQ, 0x02, 0x03);
	write_reg(SEQ, 0x04, 0x03);
	write_reg(SEQ, 0x00, 0x03);

	uint8_t data = iportb(VGA_MISC_READ);
	data &= 0x01;
	data <<= 2;
	data |= 0x0a;

	write_reg(GC, 0x06, data);
	write_reg(GC, 0x04, 0x00);
	write_reg(GC, 0x05, 0x10);
}

void set_font()
{
	get_font_access();
	for (uint32_t i = 0; i < 0x0100; i++) {
		write_vram(i * 32, font_8x16 + i * 16, 16);
	}
	release_font_access();
}

void set_text_block()
{
	write_reg(SEQ, 0x03, 0x00);
}

void set_mode(vga_config_param *mode)
{
	/* MISC */
	write_reg(MISC, 0x00, mode->miscreg);

	/* SEQ */
	write_reg(SEQ, 0, 0x03);
	for (int i = 0; i < 4; i++) {
		write_reg(SEQ, i + 1, mode->sequ_regs[i]);
	}

	/* CRTC */
	/* unlock first */
	oportb(VGA_CRTC_INDEX, 0x03);
	oportb(VGA_CRTC_WRITE, iportb(VGA_CRTC_WRITE) | 0x80);
	oportb(VGA_CRTC_INDEX, 0x11);
	oportb(VGA_CRTC_WRITE, iportb(VGA_CRTC_WRITE) & 0x7f);
	/* write regs */
	for (int i = 0; i < 25; i++) {
		write_reg(CRTC, i, mode->crtc_regs[i]);
	}

	/* GC */
	for (int i = 0; i < 9; i++) {
		write_reg(GC, i, mode->grdc_regs[i]);
	}

	/* AC */
	for (int i = 0; i < 20; i++) {
		write_reg(AC, i, mode->actl_regs[i]);
	}
	write_reg(AC, 0x14, 0x00);

	/* enable video */
	iportb(VGA_AC_RESET);
	oportb(VGA_AC_WRITE, 0x20);
}

void vga_init()
{
	set_mode(&config_mode_0x03);
	set_palette();
	set_font();
}

void put_char(char c, uint32_t row, uint32_t col)
{
	uint8_t *addr = (uint8_t *)get_fb_seg();
	addr += 2 * (80 * row + col);
	*addr = c;
	*(addr + 1) = 7;
}

char get_char(uint32_t row, uint32_t col)
{
	char ret = 0;
	uint8_t *addr = (uint8_t *)get_fb_seg();
	addr += 2 * (80 * row + col);
	//*addr = c;
	//*(addr + 1) = 7;
	ret = *addr;
	return ret;
}

void put_string(char *msg, uint32_t len, uint32_t row, uint32_t col)
{
	for (uint32_t i = 0; i < len; i++) {
		put_char(msg[i], row, col + i);
	}
}

void vga_disable_cursor()
{
	uint8_t tmp;
	tmp = read_reg(CRTC, 0x0A);
	write_reg(CRTC, 0x0A, tmp | (uint8_t)0x20);
}

void vga_enable_cursor()
{
	uint8_t tmp;
	tmp = read_reg(CRTC, 0x0A);
	write_reg(CRTC, 0x0A, tmp & (uint8_t)0xDF);
}

void vga_set_cursor_positon(uint16_t pos)
{
	/* cursor location low */
	write_reg(CRTC, 0x0F, (uint8_t)pos);
	/* cursor location high */
	write_reg(CRTC, 0x0E, (uint8_t)(pos >> 8));
}

void vga_set_cursor_shape(uint8_t start, uint8_t end)
{
	uint8_t tmp;
	start &= 0x1F;
	end &= 0x1F;

	tmp = read_reg(CRTC, 0x0A);
	tmp &= 0xE0;
	write_reg(CRTC, 0x0A, tmp | start);

	tmp = read_reg(CRTC, 0x0B);
	tmp &= 0xE0;
	write_reg(CRTC, 0x0B, tmp | end);
}

void vga_test()
{
	put_string("ZJUNIX Bootloader.", 18, 0, 0);
	while (1)
		;
}