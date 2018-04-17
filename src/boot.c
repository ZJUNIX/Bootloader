#include "vga.h"

void boot(){
    vga_init();
    int data[512];
    data[10] = 0;
    vga_test();
}