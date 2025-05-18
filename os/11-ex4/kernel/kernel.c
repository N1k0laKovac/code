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

extern uint8_t __user_elf_user1_start[];  // user1的ELF数据起始地址
extern uint8_t __user_elf_user2_start[];  // user2的ELF数据起始地址

void uart_put_hex(uint32_t num) {
    char hex_chars[] = "0123456789abcdef";
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex_chars[(num >> i) & 0xf]);
    }
}

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
	
	uart_puts("ELF magic check: ");
    uart_put_hex(__user_elf_start[0]); uart_puts(" ");
    uart_put_hex(__user_elf_start[1]); uart_puts(" ");
    uart_put_hex(__user_elf_start[2]); uart_puts(" ");
    uart_put_hex(__user_elf_start[3]); uart_puts("\n");

    

	 // 1. 加载用户程序并获取入口地址
    uint32_t user1_entry = load_elf_image(__user_elf_user1_start);
    uint32_t user2_entry = load_elf_image(__user_elf_user2_start);

    // 2. 创建任务
    if (task_create((void (*)(void))user1_entry) != 0) {
        panic("Failed to create user1 task");
    }
    if (task_create((void (*)(void))user2_entry) != 0) {
        panic("Failed to create user2 task");
    }
	

	// main();
	
	schedule();

	uart_puts("Would not go here!\n");
	while (1) {}; // stop here!
}

