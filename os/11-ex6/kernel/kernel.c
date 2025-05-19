#include "os.h"
#include "file.h"
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
struct filesystem fs;

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

	fs_init(&fs);
	
	uart_puts("ELF magic check: ");
    uart_put_hex(__user_elf_start[0]); uart_puts(" ");
    uart_put_hex(__user_elf_start[1]); uart_puts(" ");
    uart_put_hex(__user_elf_start[2]); uart_puts(" ");
    uart_put_hex(__user_elf_start[3]); uart_puts("\n");

    load_elf_image(__user_elf_start);

	// main();
	

	schedule();

	uart_puts("Would not go here!\n");
	while (1) {}; // stop here!
}

