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

#endif // _VGA_H_