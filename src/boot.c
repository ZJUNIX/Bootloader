#include "ata.h"
#include "vga.h"
#include "kernel_loader.h"
void boot()
{
	vga_init();
	int data[512];
	data[10] = 0;
	vga_test();
	ata_init();
	load_kernel();
	while (1)
		;
}