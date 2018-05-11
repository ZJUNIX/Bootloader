#include "ata.h"
#include "kernel_loader.h"
#include "vga.h"

void goto_kernel()
{
	void (*kernel_entry)() = (void (*)())0x80001000;
	kernel_entry();
}

void boot()
{
	vga_init();
	vga_test();
	ata_init();
	load_kernel();
	goto_kernel();
	while (1)
		;
}