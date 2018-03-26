# Bootloader for ZJUNIX

## Introduction

![ZJUNIX Bootloader](https://img.shields.io/badge/ZJUNIX-Bootloader-blue.svg)

This is a bootloader for ZJUNIX, with respect to mips32 specification, and is able to run on QEMU.

## Build and Run

Build tools:

1. make
2. Cross compiler for mips (either one is ok)
    - mips-mti-elf/mips-img-elf: Provided by [MIPS](https://www.mips.com/develop/tools/codescape-mips-sdk/), available on both Windows and Linux.
    - mips-linux-gnu: If you use this compiler, the compilation flags in Makefile should be changed, as they are written for mips-mti-elf/mips-img-elf.

To run:

1. Install QEMU
2. Build (make all)
3. Run (make run)

To debug:

1. Run QEMU in debug mode (make run-gdb)
2. Connect cross compiler's gdb to QEMU (mips-mti-elf-gdb -x scripts/debug.gdb)
3. Within gdb, set break points and execute `continue` to start debugging bootloader

## Reference

1. VGA
    - VGA Specification:
        - [VGA Hardware](https://wiki.osdev.org/VGA_Hardware): A short introduction of VGA
        - [FreeVGA](http://www.osdever.net/FreeVGA/home.htm): Detailed description of VGA, including hardware specification and programming instructions
    - Code snippets:
        - [x86 version](http://git.qemu.org/vgabios.git): written in C and x86 assembly
        - [C version without BIOS](https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c): written in C without BIOS invocation, but it doesn't setup palettes, which is an indespensible part of bare metal vga driver
2. Compiler
    - mips-mti-elf/mips-img-elf: [MIPS](https://www.mips.com/develop/tools/codescape-mips-sdk/)

## License

[![License](https://img.shields.io/badge/License-BSD%203--Clause-blue.svg)](./LICENSE)