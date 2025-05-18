#include "os.h"
#include "riscv.h"
extern void trap_vector(void);
extern void uart_isr(void);
extern void timer_handler(void);
extern void schedule(void);
extern void do_syscall(struct context *cxt);

void trap_init()
{
	/*
	 * set the trap-vector base-address for machine-mode
	 */
	w_mtvec((reg_t)trap_vector);
}

void external_interrupt_handler()
{
	int irq = plic_claim();

	if (irq == UART0_IRQ){
      		uart_isr();
	} else if (irq) {
		printf("unexpected interrupt irq = %d\n", irq);
	}
	
	if (irq) {
		plic_complete(irq);
	}
}

reg_t trap_handler(reg_t epc, reg_t cause, struct context *cxt)
{
	reg_t return_pc = epc;
	reg_t cause_code = cause & MCAUSE_MASK_ECODE;
	
	if (cause & MCAUSE_MASK_INTERRUPT) {
		/* Asynchronous trap - interrupt */
		switch (cause_code) {
		case 3:
			uart_puts("software interruption!\n");
			/*
			 * acknowledge the software interrupt by clearing
    			 * the MSIP bit in mip.
			 */
			int id = r_mhartid();
    			*(uint32_t*)CLINT_MSIP(id) = 0;

			schedule();

			break;
		case 7:
			uart_puts("timer interruption!\n");
			timer_handler();
			break;
		case 11:
			uart_puts("external interruption!\n");
			external_interrupt_handler();
			break;
		default:
			printf("Unknown async exception! Code = %ld\n", cause_code);
			break;
		}
	} else {
		/* Synchronous trap - exception */
		// printf("Sync exceptions! Code = %ld\n", cause_code);
		switch (cause_code) {
		case 8:
			uart_puts("System call from U-mode!\n");
			do_syscall(cxt);
			return_pc += 4;
			break;
		default:
			panic("OOPS! What can I do!");
			return_pc += 4;
		}
	}

	return return_pc;
}

void trap_test()
{
	/*
	 * Synchronous exception code = 7
	 * Store/AMO access fault
	 */
	*(int *)0x00000000 = 100;

	/*
	 * Synchronous exception code = 5
	 * Load access fault
	 */
	//int a = *(int *)0x00000000;

	uart_puts("Yeah! I'm return back from trap!\n");
}

#include "riscv.h"   // 包含 w_mstatus, w_mepc, etc.

#define USER_STACK_TOP 0x80410000  // 根据你的映射，用户栈顶地址

// 传入一个 32 位的用户入口地址
void enter_user_mode(uint32_t entry) {
    // 1) 在 mscratch 中不用保存（内核已经用 mscratch 存上下文）
    // 2) 写 mepc = entry
    w_mepc(entry);

    // 3) 设置 mstatus.MPP = 0（用户模式）
    //    并开启 MIE 中断位
    uint32_t m = r_mstatus();
    // 清除 MPP 字段（位 11-12）
    m &= ~(3 << 11);
    // 设置 MPIE = 1（mret 之后允许中断）
    m |= (1 << 7);
    w_mstatus(m);

    // 4) 设置用户栈指针（U-mode 的 sp），通常放到寄存器里
    //    sret 之后，sp 会保留这个值
    asm volatile("mv sp, %0" : : "r"(USER_STACK_TOP));

    // 5) 触发 mret，从机器模式切到用户模式
    asm volatile("mret");
}
