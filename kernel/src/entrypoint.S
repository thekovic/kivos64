# Standard MIPS register names:
#define zero	$0
#define z0	$0
#define v0	$2
#define v1	$3
#define a0	$4
#define a1	$5
#define a2	$6
#define a3	$7
#define t0	$8
#define t1	$9
#define t2	$10
#define t3	$11
#define t4	$12
#define t5	$13
#define t6	$14
#define t7	$15
#define s0	$16
#define s1	$17
#define s2	$18
#define s3	$19
#define s4	$20
#define s5	$21
#define s6	$22
#define s7	$23
#define t8	$24
#define t9	$25
#define k0	$26	/* kernel private register 0 */
#define k1	$27	/* kernel private register 1 */
#define gp	$28	/* global data pointer */
#define sp 	$29	/* stack-pointer */
#define fp	$30	/* frame-pointer */
#define ra	$31	/* return address */
#define pc	$pc	/* pc, used on mips16 */

#define fp0	$f0
#define fp1	$f1

	# Interrupt vector table.
	.section .intvectors, "ax"
	.set reorder

	.org 0
	j _inthandler
	nop

	.org 0x80
	j _inthandler
	nop

	.org 0x100
	j _inthandler
	nop

	.org 0x180
	j _inthandler
	nop

	.set noreorder

	.section .boot
	.global _start
_start:
	# Initialize small data pointer.
	la gp, _gp

	# Reset cop0 status register.
	jal cop0_status_reset
	nop

	# Copy bootcode flags stored in DMEM.
	lui v0, 0xA400
	lw t0, 0x0000(v0)
	lw t1, 0x0004(v0)
	sw t0, %gprel(__boot_memsize)(gp)
	sw t1, %gprel(__boot_random_seed)(gp)

	# Copy other bootcode flags.
	lbu t0, 0x0009(v0)
	lbu t1, 0x000A(v0)
	sw t0, %gprel(__boot_tvtype)(gp)
	sw t1, %gprel(__boot_resettype)(gp)

	# Initialize kernel modules.
	jal init_kernel
	nop

	# Call main app.
	li a0, 0
	jal main
	li a1, 0

# Infinite abort loop
_abort:
	j _abort
	nop

	.section .text

	.global _inthandler
	.global _inthandler_end

	.p2align 5
	.func _inthandler
_inthandler:

	.set noat
	.set noreorder

# The exception stack contains a dump of all GPRs/FPRs. This requires 544 bytes.
# On top of that, we need 32 bytes of empty space at offset 0-31, because
# that is required by MIPS ABI when calling C functions (it's a space called
# "argument slots" -- even if the function takes no arguments, or are only passed in 
# registers, the ABI requires reserving that space and called functions might
# use it to store local variables).
# So we keep 0-31 empty, and we start saving GPRs from 32, and then FPR.
#define EXC_STACK_SIZE (544+32)
#define STACK_GPR      32
#define STACK_HI     (STACK_GPR+(32*8))
#define STACK_LO     (STACK_HI+8)
#define STACK_SR     (STACK_LO+8)
#define STACK_CR     (STACK_SR+4)
#define STACK_EPC    (STACK_CR+4)
#define STACK_FC31   (STACK_EPC+4)
#define STACK_FPR    (STACK_FC31+4)

	addiu sp, -EXC_STACK_SIZE

	# Save caller-saved GPRs.
	sd $1, (STACK_GPR +  1 * 8)(sp) # AT
	.set at
	sd v0, (STACK_GPR +  2 * 8)(sp)
	sd v1, (STACK_GPR +  3 * 8)(sp)
	sd a0, (STACK_GPR +  4 * 8)(sp)
	sd a1, (STACK_GPR +  5 * 8)(sp)
	sd a2, (STACK_GPR +  6 * 8)(sp)
	sd a3, (STACK_GPR +  7 * 8)(sp)
	sd t0, (STACK_GPR +  8 * 8)(sp)
	sd t1, (STACK_GPR +  9 * 8)(sp)
	sd t2, (STACK_GPR + 10 * 8)(sp)
	sd t3, (STACK_GPR + 11 * 8)(sp) 
	sd t4, (STACK_GPR + 12 * 8)(sp)
	sd t5, (STACK_GPR + 13 * 8)(sp)
	sd t6, (STACK_GPR + 14 * 8)(sp)
	sd t7, (STACK_GPR + 15 * 8)(sp)
	sd t8, (STACK_GPR + 24 * 8)(sp)
	sd t9, (STACK_GPR + 25 * 8)(sp)
	sd ra, (STACK_GPR + 31 * 8)(sp)

	mflo k0
	mfhi k1
	sd k0,STACK_LO(sp)
	sd k1,STACK_HI(sp)

	# Store COP0 EPC register.
	mfc0 k0, $14
	sw k0, STACK_EPC(sp)
	# Store COP0 STATUS register.
	mfc0 k1, $12
	sw k1, STACK_SR(sp)

	jal interrupt_handler
	addiu a0, sp, 32

	# Restore COP0 STATUS register. This also disables reentrant exceptions
	# by restoring the EXL bit.
	.set noat
	lw t0, STACK_SR(sp)
	mtc0 t0, $12
	# Restore the COP0 other registers
	ld t0, STACK_LO(sp)
	ld t1, STACK_HI(sp)
	lw t2, STACK_EPC(sp)
	mtlo t0
	mthi t1
	mtc0 t2, $14

	# restore GPRs
	ld $1, (STACK_GPR +  1 * 8)(sp)
	ld v0, (STACK_GPR +  2 * 8)(sp)
	ld v1, (STACK_GPR +  3 * 8)(sp)
	ld a0, (STACK_GPR +  4 * 8)(sp)
	ld a1, (STACK_GPR +  5 * 8)(sp)
	ld a2, (STACK_GPR +  6 * 8)(sp)
	ld a3, (STACK_GPR +  7 * 8)(sp)
	ld t0, (STACK_GPR +  8 * 8)(sp)
	ld t1, (STACK_GPR +  9 * 8)(sp)
	ld t2, (STACK_GPR + 10 * 8)(sp)
	ld t3, (STACK_GPR + 11 * 8)(sp) 
	ld t4, (STACK_GPR + 12 * 8)(sp)
	ld t5, (STACK_GPR + 13 * 8)(sp)
	ld t6, (STACK_GPR + 14 * 8)(sp)
	ld t7, (STACK_GPR + 15 * 8)(sp)
	ld t8, (STACK_GPR + 24 * 8)(sp)
	ld t9, (STACK_GPR + 25 * 8)(sp)
	ld ra, (STACK_GPR + 31 * 8)(sp)
	addiu sp, EXC_STACK_SIZE
	eret
	nop

_inthandler_end:
	.endfunc

	# This will never get here hopefully.
	j _abort
	nop
