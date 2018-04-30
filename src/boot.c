#include "vga.h"
#include "vga_kernel_print.h"

int test_global = 0;

void boot()
{
	vga_init();
	int data[512];
	data[10] = 0;
	if (test_global != 0) {
		vga_test();
	}
	//
	// put_string("1111", 5, 1, 1);
	// put_string("2222", 5, 2, 2);

	//vga_set_cursor_positon(100);
	//while(1);

	int test_num_int = 2018;
	char c = 'a';
	kernel_printf("Hello %d %c \n888", test_num_int, 'a');
	while(1==1)
	{
		kernel_printf("On that bold hill, against a broad blue stream,\n");
		kernel_printf("stood Arthur Phillip on a day of dream;\n");
		kernel_printf("what time the mists of morning westward rolled\n");
		kernel_printf("and heaven flowered on a bay of gold.\n");
		kernel_printf("Here, in the hour that shines and sounds afar,\n");
		kernel_printf("flamed first Old England's banner like a star;\n");
		kernel_printf("Here in a time august with prayer and praise,\n");
		kernel_printf("was born the nation of these splendid days,\n");
		kernel_printf("and here, this land's majestic yesterday\n");
		kernel_printf("of immemorial silence died away \n");
		int count_down=1000000000;
		while(count_down--);
	}
	// put_string("2222", 20, 2, 2);
}