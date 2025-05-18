#define LOAD		lw
#define STORE		sw
#define SIZE_REG	4

.macro reg_save base
	STORE ra,   0*SIZE_REG(\base)
	STORE sp,   1*SIZE_REG(\base)
	STORE t0,   4*SIZE_REG(\base)
	STORE t1,   5*SIZE_REG(\base)
	STORE t2,   6*SIZE_REG(\base)
	STORE s0,   7*SIZE_REG(\base)
	STORE s1,   8*SIZE_REG(\base)
	STORE a0,   9*SIZE_REG(\base)
	STORE a1,  10*SIZE_REG(\base)
	STORE a2,  11*SIZE_REG(\base)
	STORE a3,  12*SIZE_REG(\base)
	STORE a4,  13*SIZE_REG(\base)
	STORE a5,  14*SIZE_REG(\base)
	STORE a6,  15*SIZE_REG(\base)
	STORE a7,  16*SIZE_REG(\base)
	STORE s2,  17*SIZE_REG(\base)
	STORE s3,  18*SIZE_REG(\base)
	STORE s4,  19*SIZE_REG(\base)
	STORE s5,  20*SIZE_REG(\base)
	STORE s6,  21*SIZE_REG(\base)
	STORE s7,  22*SIZE_REG(\base)
	STORE s8,  23*SIZE_REG(\base)
	STORE s9,  24*SIZE_REG(\base)
	STORE s10, 25*SIZE_REG(\base)
	STORE s11, 26*SIZE_REG(\base)
	STORE t3,  27*SIZE_REG(\base)
	STORE t4,  28*SIZE_REG(\base)
	STORE t5,  29*SIZE_REG(\base)
	
.endm


.macro reg_restore base
	LOAD ra,   0*SIZE_REG(\base)
	LOAD sp,   1*SIZE_REG(\base)
	LOAD t0,   4*SIZE_REG(\base)
	LOAD t1,   5*SIZE_REG(\base)
	LOAD t2,   6*SIZE_REG(\base)
	LOAD s0,   7*SIZE_REG(\base)
	LOAD s1,   8*SIZE_REG(\base)
	LOAD a0,   9*SIZE_REG(\base)
	LOAD a1,  10*SIZE_REG(\base)
	LOAD a2,  11*SIZE_REG(\base)
	LOAD a3,  12*SIZE_REG(\base)
	LOAD a4,  13*SIZE_REG(\base)
	LOAD a5,  14*SIZE_REG(\base)
	LOAD a6,  15*SIZE_REG(\base)
	LOAD a7,  16*SIZE_REG(\base)
	LOAD s2,  17*SIZE_REG(\base)
	LOAD s3,  18*SIZE_REG(\base)
	LOAD s4,  19*SIZE_REG(\base)
	LOAD s5,  20*SIZE_REG(\base)
	LOAD s6,  21*SIZE_REG(\base)
	LOAD s7,  22*SIZE_REG(\base)
	LOAD s8,  23*SIZE_REG(\base)
	LOAD s9,  24*SIZE_REG(\base)
	LOAD s10, 25*SIZE_REG(\base)
	LOAD s11, 26*SIZE_REG(\base)
	LOAD t3,  27*SIZE_REG(\base)
	LOAD t4,  28*SIZE_REG(\base)
	LOAD t5,  29*SIZE_REG(\base)
	LOAD t6,  30*SIZE_REG(\base)
.endm



.text


.globl trap_vector

.balign 4
trap_vector:
	
	csrrw	t6, mscratch, t6	
	reg_save t6

	
	mv	t5, t6			# t5 points to the context of current task
	csrr	t6, mscratch		# read t6 back from mscratch
	STORE	t6, 30*SIZE_REG(t5)	# save t6 with t5 as base

	
	csrr	a0, mepc
	STORE	a0, 31*SIZE_REG(t5)

	
	csrw	mscratch, t5

	
	csrr	a0, mepc
	csrr	a1, mcause
	csrr	a2, mscratch
	call	trap_handler

	
	csrw	mepc, a0

	
	csrr	t6, mscratch
	reg_restore t6

	
	mret


.globl switch_to
.balign 4
switch_to:
	
	csrw	mscratch, a0
	
	LOAD	a1, 31*SIZE_REG(a0)
	csrw	mepc, a1

	
	
	mv	t6, a0
	reg_restore t6

	
	mret

.end

