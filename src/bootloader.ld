OUTPUT_FORMAT("elf32-tradlittlemips", "elf32-tradbigmips", "elf32-tradlittlemips")
OUTPUT_ARCH(mips)
SECTIONS
{
    _rom_start = 0xbfc00000;
    .text_init _rom_start :
    {
        _ftext_init = ABSOLUTE(.);
        start.o(.text)
        preboot.o(.text)
        . = ALIGN(8);
        _etext_init = ABSOLUTE(.);
    } =0
    _text_init_size = _etext_init - _ftext_init;
    
    _ram_text_start = 0x80100000;
    _rom_text_start = _text_init_size + _rom_start;
    .text _ram_text_start : AT(_rom_text_start)
    {
        _ftext = ABSOLUTE(.);
        *(.text)
        *(.text.*)
        . = ALIGN(8);
        _etext = ABSOLUTE(.);
    } =0
    _text_size = _etext - _ftext;

    _ram_data_start = _etext;
    _rom_data_start = _text_size + _rom_text_start;
    .data _ram_data_start : AT(_rom_data_start)
    {
        _fdata = ABSOLUTE(.);
        *(.rodata)
        *(.rodata.*)
        *(.data)
        . = ALIGN(8);
        _gp = ABSOLUTE(. + 0x7ff0);
        *(.lit8)
        *(.lit4)
        *(.sdata)
        . = ALIGN(8);
        _edata = ABSOLUTE(.);
    } =0
    _data_size = _edata - _fdata;

    _rom_bss_start = _data_size + _rom_data_start;
    _ram_bss_start = .;
    _fbss = .;
    .sbss :
    {
        *(.sbss)
        *(.scommon)
    }
    .bss :
    {
        *(.bss)
        *(COMMON)
    }
    _ebss = .;
    _bss_size = _ebss - _fbss;

    /DISCARD/ :
    {
        *(.MIPS.abiflags)
    }
}