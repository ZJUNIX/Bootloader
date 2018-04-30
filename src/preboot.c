#include "inttypes.h"

extern uint8_t _rom_text_start[], _ram_text_start[], _text_size[];
extern uint8_t _rom_data_start[], _ram_data_start[], _data_size[];
extern uint8_t _rom_bss_start[], _ram_bss_start[], _bss_size[];

void copy_section(uint8_t *dest, uint8_t *src, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++) {
		*(dest + i) = *(src + i);
	}
}

void copy_code()
{
	copy_section(_ram_text_start, _rom_text_start, (uint32_t)_text_size);
}

void copy_data()
{
	copy_section(_ram_data_start, _rom_data_start, (uint32_t)_data_size);
}

void copy_bss()
{
	copy_section(_ram_bss_start, _rom_bss_start, (uint32_t)_bss_size);
}

void preboot()
{
	copy_code();
	copy_data();
	copy_bss();
}