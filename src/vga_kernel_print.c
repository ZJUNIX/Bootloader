#include "vga.h"
#include "vga_kernel_print.h"
#include "utils.h"
//#include "arch.h"
//#include <arch.h>
//#include <zjunix/utils.h>



const int VGA_CHAR_MAX_ROW = 32;
const int VGA_CHAR_MAX_COL = 128;
const int VGA_CHAR_ROW = 30;
const int VGA_CHAR_COL = 80;
int cursor_row = 0;
int cursor_col = 0;
int cursor_freq = 31;

const int num_char_per_row = 80;

unsigned int* CHAR_VRAM = 0;//dummy use, deprecated

void kernel_set_cursor() {
    //*GPIO_CURSOR = ((cursor_freq & 0xff) << 16) + ((cursor_row & 0xff) << 8) + (cursor_col & 0xff);
    vga_set_cursor_positon(num_char_per_row * cursor_col + cursor_row );
}
/*
void init_vga() {
    unsigned int w = 0x000fff00;
    cursor_row = cursor_col = 0;
    cursor_freq = 31;
    kernel_set_cursor();
}
*/
unsigned int* set_vram(unsigned int* dest, unsigned int w, int len) {
    int starting_offset = (int) dest;
    w = 0;     
    while (len--)
        *dest++ = w;

    return dest;
}


void kernel_clear_screen(int scope) {
    
    unsigned int w = 0x000fff00;
    scope &= 31;
    cursor_col = 0;
    cursor_row = 0;
    kernel_set_cursor();
    set_vram(CHAR_VRAM, w, scope * VGA_CHAR_MAX_COL);
    
}

void kernel_scroll_screen() {
    unsigned int w = 0x000fff00;
    kernel_memcpy(CHAR_VRAM, (CHAR_VRAM + VGA_CHAR_MAX_COL), (VGA_CHAR_ROW - 2) * VGA_CHAR_MAX_COL * 4);
    set_vram((CHAR_VRAM + (VGA_CHAR_ROW - 2) * VGA_CHAR_MAX_COL), w, VGA_CHAR_MAX_COL);
}

void kernel_putchar_at(int ch, int fc, int bg, int row, int col) {
    unsigned int *p;
    row = row & 31;
    col = col & 127;
    //p = CHAR_VRAM + row * VGA_CHAR_MAX_COL + col;
    //*p = ((bg & 0xfff) << 20) + ((fc & 0xfff) << 8) + (ch & 0xff);//litle endian
    
    put_char(ch, row, col);
    
    //display: 
	//    blue green red    blue green red    ascii
	//       background        character       code
	//    31         20     19           8    7   0
}


int kernel_putchar(int ch, int fc, int bg) {
    unsigned int w = 0x000fff00;
    if (ch == '\r')
        return ch;
    if (ch == '\n') {
        set_vram(CHAR_VRAM + cursor_row * VGA_CHAR_MAX_COL + cursor_col, w, VGA_CHAR_COL - cursor_col);
        cursor_col = 0;
        if (cursor_row == VGA_CHAR_ROW - 2) {
            kernel_scroll_screen();
        } else {
            cursor_row++;
#ifdef VGA_CALIBRATE
            kernel_putchar(' ', fc, bg);
#endif  // VGA_CALIBRATE
        }
    } else if (ch == '\t') {
        if (cursor_col >= VGA_CHAR_COL - 4) {
            kernel_putchar('\n', 0, 0);
        } else {
            set_vram(CHAR_VRAM + cursor_row * VGA_CHAR_MAX_COL + cursor_col, w, 4 - cursor_col & 3);
            cursor_col = (cursor_col + 4) & (-4);
        }
    } else {
        if (cursor_col == VGA_CHAR_COL) {
            kernel_putchar('\n', 0, 0);
        }
        kernel_putchar_at(ch, fc, bg, cursor_row, cursor_col);
        cursor_col++;
    }
    kernel_set_cursor();
    return ch;
}



//warper function

/*
int kernel_putchar(int ch, int fc, int bg) 
{
    //void put_char(char c, uint32_t row, uint32_t col)
    //int cursor_row = 5;
    //int cursor_col = 5;
    put_char(ch, cursor_row, cursor_col);

}
*/

int kernel_puts(const char *s, int fc, int bg) {
    int ret = 0;
    while (*s) {
        ret++;
        kernel_putchar(*s++, fc, bg);
    }
    return ret;
}

int kernel_putint(int x, int fc, int bg) {
    char buffer[12];
    char *ptr = buffer + 11;
    int neg = 0;
    buffer[11] = 0;
    if (x == 0) {
        kernel_putchar('0', fc, bg);
        return x;
    }
    if (x < 0) {
        neg = 1;
        x = -x;
    }
    while (x) {
        ptr--;
        *ptr = (x % 10) + '0';
        x /= 10;
    }
    if (neg) {
        ptr--;
        *ptr = '-';
    }
    kernel_puts(ptr, fc, bg);
    return x;
}

//static const char *HEX_MAP = "0123456789abcdef";//put inside
int kernel_putintx(unsigned int x, int fc, int bg) {
    char HEX_MAP[] = "0123456789abcdef";//
    char buffer[12];
    char *ptr = buffer + 11;
    buffer[11] = 0;
    if (x == 0) {
        kernel_putchar('0', fc, bg);
        return x;
    }
    while (x) {
        ptr--;
        *ptr = HEX_MAP[x & 15];
        x >>= 4;
    }
    kernel_puts(ptr, fc, bg);
    return x;
}

int kernel_vprintf(const char *format, va_list ap) {
    int cnt = 0;
    while (*format) {
        if (*format != '%') {
            kernel_putchar(*format++, 0xfff, 0);
        } else {
            format++;
            switch (*format) {
                case 'c': {
                    char valch = va_arg(ap, int);
                    kernel_putchar(valch, 0xfff, 0);
                    format++;
                    cnt++;
                    break;
                }
                case 'd': {
                    int valint = va_arg(ap, int);
                    kernel_putint(valint, 0xfff, 0);
                    format++;
                    cnt++;
                    break;
                }
                case 'x': {
                    int valint = va_arg(ap, int);
                    kernel_putintx(valint, 0xfff, 0);
                    format++;
                    cnt++;
                    break;
                }
                case 's': {
                    char *valstr = va_arg(ap, char *);
                    kernel_puts(valstr, 0xfff, 0);
                    format++;
                    cnt++;
                    break;
                }
                default: {
                    cnt = -1;
                    goto exit;
                }
            }
        }
    }
exit:
    return cnt;
}

int kernel_printf(const char *format, ...) {
    int cnt = 0;
    va_list ap;
    va_start(ap, format);
    cnt = kernel_vprintf(format, ap);
    va_end(ap);
    return cnt;
}
