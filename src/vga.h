#ifndef _VGA_H_
#define _VGA_H_

#include "inttypes.h"

void vga_init();
uint32_t get_fb_seg();
void vga_disable_cursor();
void vga_enable_cursor();


#endif // _VGA_H_