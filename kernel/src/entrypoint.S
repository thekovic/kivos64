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
	# Reset cop0 status register.
	jal cop0_status_reset
	nop

	jal bootflags_init
	nop

	# Initialize kernel modules.
	jal init_kernel
	nop

	# Call main app.
	li $a0, 0
	la $t0, main
	jalr $t0
	li $a1, 0

# Infinite abort loop
_abort:
	j _abort
	nop

	.section .irqs

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
#define EXC_STACK_SIZE  (544+32)
#define STACK_GPR       (32)
#define STACK_HI        (STACK_GPR+(32*8))
#define STACK_LO        (STACK_HI+8)
#define STACK_SR        (STACK_LO+8)
#define STACK_CR        (STACK_SR+4)
#define STACK_EPC       (STACK_CR+4)
#define STACK_FC31      (STACK_EPC+4)
#define STACK_FPR       (STACK_FC31+4)

	addiu $sp, -EXC_STACK_SIZE

	# Save syscall number.
	move $k0, $v0

save_gp_regs:
	# Save caller-saved GPRs.
	sd $1,  (STACK_GPR +  1 * 8)($sp) # AT
	.set at
	sd $v0, (STACK_GPR +  2 * 8)($sp)
	sd $v1, (STACK_GPR +  3 * 8)($sp)
	sd $a0, (STACK_GPR +  4 * 8)($sp)
	sd $a1, (STACK_GPR +  5 * 8)($sp)
	sd $a2, (STACK_GPR +  6 * 8)($sp)
	sd $a3, (STACK_GPR +  7 * 8)($sp)
	sd $t0, (STACK_GPR +  8 * 8)($sp)
	sd $t1, (STACK_GPR +  9 * 8)($sp)
	sd $t2, (STACK_GPR + 10 * 8)($sp)
	sd $t3, (STACK_GPR + 11 * 8)($sp) 
	sd $t4, (STACK_GPR + 12 * 8)($sp)
	sd $t5, (STACK_GPR + 13 * 8)($sp)
	sd $t6, (STACK_GPR + 14 * 8)($sp)
	sd $t7, (STACK_GPR + 15 * 8)($sp)
	sd $t8, (STACK_GPR + 24 * 8)($sp)
	sd $t9, (STACK_GPR + 25 * 8)($sp)
	sd $ra, (STACK_GPR + 31 * 8)($sp)

	mflo $t0
	mfhi $t1
	sd $t0,STACK_LO($sp)
	sd $t1,STACK_HI($sp)

	# Store COP0 EPC register.
	mfc0 $t0, $14
	sw $t0, STACK_EPC($sp)
	# Store COP0 STATUS register.
	mfc0 $t1, $12
	sw $t1, STACK_SR($sp)

	# Store COP0 CAUSE register.
	mfc0 $t0, $13
	sw $t0, STACK_CR($sp)

	jal interrupt_reset_mode
	nop

	lw $t0, STACK_CR($sp)
	nop
	andi $t1, $t0, 0xff
	beqz $t1, interrupt_found
	nop

exception_found:
	jal exception_reset_mode
	nop

save_fpu_regs:
	.set noat
	cfc1 $1, $f31
	sw $1, STACK_FC31($sp)
	.set at
	sdc1 $f0, (STACK_FPR+ 0*8)($sp)
	sdc1 $f1, (STACK_FPR+ 1*8)($sp)
	sdc1 $f2, (STACK_FPR+ 2*8)($sp)
	sdc1 $f3, (STACK_FPR+ 3*8)($sp)
	sdc1 $f4, (STACK_FPR+ 4*8)($sp)
	sdc1 $f5, (STACK_FPR+ 5*8)($sp)
	sdc1 $f6, (STACK_FPR+ 6*8)($sp)
	sdc1 $f7, (STACK_FPR+ 7*8)($sp)
	sdc1 $f8, (STACK_FPR+ 8*8)($sp)
	sdc1 $f9, (STACK_FPR+ 9*8)($sp)
	sdc1 $f10,(STACK_FPR+10*8)($sp)
	sdc1 $f11,(STACK_FPR+11*8)($sp)
	sdc1 $f12,(STACK_FPR+12*8)($sp)
	sdc1 $f13,(STACK_FPR+13*8)($sp)
	sdc1 $f14,(STACK_FPR+14*8)($sp)
	sdc1 $f15,(STACK_FPR+15*8)($sp)
	sdc1 $f16,(STACK_FPR+16*8)($sp)
	sdc1 $f17,(STACK_FPR+17*8)($sp)
	sdc1 $f18,(STACK_FPR+18*8)($sp)
	sdc1 $f19,(STACK_FPR+19*8)($sp)

save_callee_saved_regs:
	sd   $s0, (STACK_GPR+16*8)($sp)
	sd   $s1, (STACK_GPR+17*8)($sp)
	sd   $s2, (STACK_GPR+18*8)($sp)
	sd   $s3, (STACK_GPR+19*8)($sp)
	sd   $s4, (STACK_GPR+20*8)($sp)
	sd   $s5, (STACK_GPR+21*8)($sp)
	sd   $s6, (STACK_GPR+22*8)($sp)
	sd   $s7, (STACK_GPR+23*8)($sp)
	sd   $gp, (STACK_GPR+28*8)($sp)
	sd   $fp, (STACK_GPR+30*8)($sp)
	sdc1 $f20,(STACK_FPR+20*8)($sp)
	sdc1 $f21,(STACK_FPR+21*8)($sp)
	sdc1 $f22,(STACK_FPR+22*8)($sp)
	sdc1 $f23,(STACK_FPR+23*8)($sp)
	sdc1 $f24,(STACK_FPR+24*8)($sp)
	sdc1 $f25,(STACK_FPR+25*8)($sp)
	sdc1 $f26,(STACK_FPR+26*8)($sp)
	sdc1 $f27,(STACK_FPR+27*8)($sp)
	sdc1 $f28,(STACK_FPR+28*8)($sp)
	sdc1 $f29,(STACK_FPR+29*8)($sp)
	sdc1 $f30,(STACK_FPR+30*8)($sp)
	sdc1 $f31,(STACK_FPR+31*8)($sp)
	
	# Load syscall function arguments.
	ld   $t4,(STACK_GPR+12*8)($sp)
	ld   $t5,(STACK_GPR+13*8)($sp)
	ld   $t6,(STACK_GPR+14*8)($sp)
	ld   $t7,(STACK_GPR+15*8)($sp)
	jal exception_handler
	addiu $a0, $sp, 32
	# Save syscall return value.
	sd $t8, (STACK_GPR + 24 * 8)($sp)
	# Bump EPC to acknowledge syscall.
	lw $t2, STACK_EPC($sp)
	addi $t2, $t2, 4
	sw $t2, STACK_EPC($sp)

	j exception_end
	nop

interrupt_found:
	jal interrupt_handler
	addiu $a0, $sp, 32
	j interrupt_end
	nop

exception_end:
	# Restore caller-saved registers. These are only saved during an exception.
	ld   $s0, (STACK_GPR+16*8)($sp)
	ld   $s1, (STACK_GPR+17*8)($sp)
	ld   $s2, (STACK_GPR+18*8)($sp)
	ld   $s3, (STACK_GPR+19*8)($sp)
	ld   $s4, (STACK_GPR+20*8)($sp)
	ld   $s5, (STACK_GPR+21*8)($sp)
	ld   $s6, (STACK_GPR+22*8)($sp)
	ld   $s7, (STACK_GPR+23*8)($sp)
	ld   $gp, (STACK_GPR+28*8)($sp)
	ld   $fp, (STACK_GPR+30*8)($sp)
	ldc1 $f20,(STACK_FPR+20*8)($sp)
	ldc1 $f21,(STACK_FPR+21*8)($sp)
	ldc1 $f22,(STACK_FPR+22*8)($sp)
	ldc1 $f23,(STACK_FPR+23*8)($sp)
	ldc1 $f24,(STACK_FPR+24*8)($sp)
	ldc1 $f25,(STACK_FPR+25*8)($sp)
	ldc1 $f26,(STACK_FPR+26*8)($sp)
	ldc1 $f27,(STACK_FPR+27*8)($sp)
	ldc1 $f28,(STACK_FPR+28*8)($sp)
	ldc1 $f29,(STACK_FPR+29*8)($sp)
	ldc1 $f30,(STACK_FPR+30*8)($sp)
	ldc1 $f31,(STACK_FPR+31*8)($sp)

interrupt_end:
	# Restore FPU regs if FPU is enabled (aka we handled an exception).
	mfc0 $t0, $12
	and $t0, 0x20000000
	beqz $t0, interrupt_end_gp_regs
	nop

	ldc1 $f0, (STACK_FPR+ 0*8)($sp)
	ldc1 $f1, (STACK_FPR+ 1*8)($sp)
	ldc1 $f2, (STACK_FPR+ 2*8)($sp)
	ldc1 $f3, (STACK_FPR+ 3*8)($sp)
	ldc1 $f4, (STACK_FPR+ 4*8)($sp)
	ldc1 $f5, (STACK_FPR+ 5*8)($sp)
	ldc1 $f6, (STACK_FPR+ 6*8)($sp)
	ldc1 $f7, (STACK_FPR+ 7*8)($sp)
	ldc1 $f8, (STACK_FPR+ 8*8)($sp)
	ldc1 $f9, (STACK_FPR+ 9*8)($sp)
	ldc1 $f10,(STACK_FPR+10*8)($sp)
	ldc1 $f11,(STACK_FPR+11*8)($sp)
	ldc1 $f12,(STACK_FPR+12*8)($sp)
	ldc1 $f13,(STACK_FPR+13*8)($sp)
	ldc1 $f14,(STACK_FPR+14*8)($sp)
	ldc1 $f15,(STACK_FPR+15*8)($sp)
	ldc1 $f16,(STACK_FPR+16*8)($sp)
	ldc1 $f17,(STACK_FPR+17*8)($sp)
	ldc1 $f18,(STACK_FPR+18*8)($sp)
	ldc1 $f19,(STACK_FPR+19*8)($sp)

	lw $t0, STACK_FC31($sp)
	ctc1 $t0, $f31
	
interrupt_end_gp_regs:
	# Restore COP0 STATUS register. This also disables reentrant exceptions
	# by restoring the EXL bit.
	.set noat
	lw $t0, STACK_SR($sp)
	mtc0 $t0, $12
	# Restore the COP0 other registers
	ld $t0, STACK_LO($sp)
	ld $t1, STACK_HI($sp)
	lw $t2, STACK_EPC($sp)
	mtlo $t0
	mthi $t1
	mtc0 $t2, $14

	# restore GPRs
	ld $1, (STACK_GPR +  1 * 8)($sp)
	ld $v0, (STACK_GPR +  2 * 8)($sp)
	ld $v1, (STACK_GPR +  3 * 8)($sp)
	ld $a0, (STACK_GPR +  4 * 8)($sp)
	ld $a1, (STACK_GPR +  5 * 8)($sp)
	ld $a2, (STACK_GPR +  6 * 8)($sp)
	ld $a3, (STACK_GPR +  7 * 8)($sp)
	ld $t0, (STACK_GPR +  8 * 8)($sp)
	ld $t1, (STACK_GPR +  9 * 8)($sp)
	ld $t2, (STACK_GPR + 10 * 8)($sp)
	ld $t3, (STACK_GPR + 11 * 8)($sp) 
	ld $t4, (STACK_GPR + 12 * 8)($sp)
	ld $t5, (STACK_GPR + 13 * 8)($sp)
	ld $t6, (STACK_GPR + 14 * 8)($sp)
	ld $t7, (STACK_GPR + 15 * 8)($sp)
	ld $t8, (STACK_GPR + 24 * 8)($sp)
	ld $t9, (STACK_GPR + 25 * 8)($sp)
	ld $ra, (STACK_GPR + 31 * 8)($sp)
	addiu $sp, EXC_STACK_SIZE
	eret
	nop

_inthandler_end:
	.endfunc

	# This will never get here hopefully.
	j _abort
	nop
