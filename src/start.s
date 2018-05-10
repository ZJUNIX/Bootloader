.extern preboot
.extern boot

.set noreorder

__start:
	jal	boot_start
	nop	

.org 0x0200
exc_tlb_refill:
	j	exc_tlb_refill

.org 0x300
exc_cache_error:
	j	exc_cache_error

.org 0x380
exc_tlb_refill_exl:
	j	exc_tlb_refill_exl

.org 0x400
exc_interrupt:
	j	exc_interrupt

.org 0x480
exc_ejtag_debug:
	j	exc_ejtag_debug

boot_start:
	# init stack pointer
	li	$sp, 0x80400000
	# init cp0:status
	li	$t0, 0x00400004
	mtc0	$t0, $12
	# init cp0:compare
	mtc0	$zero, $11, 0
	# copy code to ram
	jal	preboot
	nop
	# start bootloader
	la	$t0, boot
	jalr	$t0
	nop	

boot_halt:
	j	boot_halt
	nop
