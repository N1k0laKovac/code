SYSCALL = y

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

include /mnt/d/os/ex/riscv-operating-system-mooc/code/os/11-syscall/common.mk

# 生成 user/usys.S 的规则
usys.S: usys.pl syscall.h
	perl usys.pl > usys.S

# 确保 user/usys.S 被编译为 user/usys.o
usys.o: usys.S
	$(CC) $(CFLAGS) -c -o $@ $<