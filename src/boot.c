#include "vga.h"
#include "vga_kernel_print.h"

int test = 0;

void boot(){
    vga_init();
    int data[512];
    data[10] = 0;

    if(test==0)
        vga_test();
    //kernel_printf("Hello");
}