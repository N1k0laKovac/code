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

void enter_user_mode(uint64_t entry) {
    // �û�ջ��ַ
    uint64_t user_sp = 0x80400000; // �Զ�����û�ջλ��

    // ������һ��ָ��Ϊ entry
    write_csr(sepc, entry);

    // �����û�ջ
    asm volatile("mv sp, %0" : : "r"(user_sp));

    // ���� sstatus: SPP=0����һ�� sret �л��� U-mode��
    uint64_t sstatus = read_csr(sstatus);
    sstatus &= ~SSTATUS_SPP;       // ��� SPP�������û�ģʽ��
    sstatus |= SSTATUS_SPIE;       // ���ж�
    write_csr(sstatus, sstatus);

    // sret �� S ģʽ�е� U ģʽ
    asm volatile("sret");
}