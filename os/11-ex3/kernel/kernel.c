#include "os.h"

/*
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 */
extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void schedule(void);
extern void os_main(void);
extern void trap_init(void);
extern void plic_init(void);
extern void timer_init(void);

extern uint8_t __user_elf_start[];

void start_kernel(void)
{
	// sys_MyPrintf("Kernel test: %s\n", "hello"); 
	uart_init();
	uart_puts("Hello, RVOS!\n");
	printf("== Kernel started ==\n");

	page_init();

	trap_init();

	plic_init();

	timer_init();

	sched_init();

    load_elf_image(__user_elf_start);
	// os_main();
	

	schedule();

	uart_puts("Would not go here!\n");
	while (1) {}; // stop here!
}

