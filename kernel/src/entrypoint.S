/* Standard MIPS register names: */
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

	.set noreorder

	.section .boot
	.global _start
_start:
	/* initialize small data pointer */
	la gp, _gp

	/* reset cop0 status register */
	jal cop0_status_reset

	/* Copy bootcode flags, stored in DMEM */
	lui v0, 0xA400
	lw t0, 0x0000(v0)
	lw t1, 0x0004(v0)
	sw t0, %gprel(__boot_memsize)(gp)
	sw t1, %gprel(__boot_random_seed)(gp)

	/* Copy other bootcode flags */
	lbu t0, 0x0009(v0)
	lbu t1, 0x000A(v0)
	sw t0, %gprel(__boot_tvtype)(gp)
	sw t1, %gprel(__boot_resettype)(gp)

	/* call main app */
	li a0, 0
	jal main
	li a1, 0

_abort:
	j _abort
	nop
