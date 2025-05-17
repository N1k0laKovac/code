SYSCALL = y
K=kernel
U=user

SRCS_ASM = \
	start.S \
	mem.S \
	entry.S \
	usys.S \

SRCS_C = \
	kernel.c \
	uart.c \
	printf.c \
	page.c \
	sched.c \
	user.c \
	trap.c \
	plic.c \
	timer.c \
	lock.c \
	syscall.c

include /mnt/d/os/ex/riscv-operating-system-mooc/code/os/11-ex3/common.mk

# 生成 user/usys.S 的规则
$U/usys.S: $U/usys.pl $K/syscall.h
	perl $U/usys.pl > $U/usys.S

# 确保 user/usys.S 被编译为 user/usys.o
usys.o: $U/usys.S
	$(CC) $(CFLAGS) -c -o $@ $<