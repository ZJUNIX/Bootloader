# target info
export PROJECT      := bootloader
export TARGET       := $(PROJECT).bin
export MAP          := $(PROJECT).map
export ELF          := $(PROJECT).elf
export DISASSEMBLY  := $(PROJECT).disasm

# cross compile tolchain
export CROSS_COMPILE    := mips-img-elf-

export CC   := $(CROSS_COMPILE)gcc
export AS   := $(CROSS_COMPILE)as
export LD   := $(CROSS_COMPILE)ld
export OC   := $(CROSS_COMPILE)objcopy
export OD   := $(CROSS_COMPILE)objdump

# QEMU
QEMU	:= qemu-system-mipsel

# flags
export TARGET_FLAG  := -mips32 -EL
export CCFLAG       := $(TARGET_FLAG) -c -fno-builtin -mno-abicalls \
                       -nostdinc -std=gnu99
export ASFLAG       := $(TARGET_FLAG)
export LDFLAG       := -EL -T bootloader.ld
export QEMU_MACHINE := mips
export QEMU_FLAG	:= -machine $(QEMU_MACHINE) \
                       -m 256 -bios $(TARGET) \
					   -monitor stdio -vga cirrus #\
					   -drive file=disk.vhd,format=vpc
export QEMU_GDB_FLAG:= -S -gdb tcp::1234

# objects
export OBJS := start.o preboot.o boot.o vga.o io.o  utils.o vga_kernel_print.o

# dependencies
%.o: %.c
	$(CC) $(CCFLAG) $< -o $@

%.o: %.s
	$(AS) $(ASFLAG) $< -o $@

# make
.PHONY: default
default: objcopy

.PHONY: all
all: objcopy disassembly

$(ELF): $(OBJS)
	$(LD) $(LDFLAG) $(OBJS) -Map $(MAP) -o $@

.PHONY: objcopy
objcopy: $(TARGET)

$(TARGET): $(ELF)
	$(OC) -S -O binary $(ELF) $(TARGET)

.PHONY: disassembly
disassembly: $(DISASSEMBLY)

$(DISASSEMBLY): $(ELF)
	$(OD) -S $(ELF) > $(DISASSEMBLY)

.PHONY: run
run: all
	$(QEMU) $(QEMU_FLAG)

.PHONY: run-gdb
run-gdb: all
	$(QEMU) $(QEMU_FLAG) $(QEMU_GDB_FLAG)

# clean
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET) $(MAP) $(ELF) $(DISASSEMBLY)