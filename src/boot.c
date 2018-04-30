#include "vga.h"
#include "vga_kernel_print.h"

int test_global = 0;

void boot(){
    vga_init();
    int data[512];
    data[10] = 0;
    if(test_global==0)
    {
        vga_test();
    }
    //
    //put_string("1111", 5, 1, 1);
    //put_string("2222", 5, 2, 2);
    //kernel_printf("Hello");
     //put_string("2222", 20, 2, 2);
}