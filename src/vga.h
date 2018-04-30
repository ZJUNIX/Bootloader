#ifndef _VGA_H_
#define _VGA_H_

#include "inttypes.h"

void vga_init();
void vga_test();
uint32_t get_fb_seg();
void vga_disable_cursor();
void vga_enable_cursor();
void vga_set_cursor_shape(uint8_t start, uint8_t end);
void vga_set_cursor_positon(uint16_t pos);

void put_char(char c, uint32_t row, uint32_t col);
void put_string(char *msg, uint32_t len, uint32_t row, uint32_t col);
char get_char(uint32_t row, uint32_t col);


#endif // _VGA_H_