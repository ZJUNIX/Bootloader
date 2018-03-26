SECTIONS
{
    .       = 0xbfc00000;
    .text   : { *(.text) }

    .       = 0xbfe00000;
    .data   : { *(.data) }
    .bss    : { *(.bss) }
    .sdata  : { *(.sdata) }
}