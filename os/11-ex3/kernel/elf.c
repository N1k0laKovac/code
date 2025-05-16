#include "elf.h"
#include "os.h"
#include "string.h"

extern void enter_user_mode(uint64_t entry);

int load_elf_image(uint8_t *elf_buf) {
    uart_puts(">>> load_elf_image called\n");
    Elf64_Ehdr *ehdr = (Elf64_Ehdr *)elf_buf;

    if (ehdr->magic != ELF_MAGIC) {
        uart_puts("Invalid ELF magic!\n");
        return -1;
    }

    uart_puts("ELF magic OK. Loading program segments...\n");

    Elf64_Phdr *ph = (Elf64_Phdr *)(elf_buf + ehdr->phoff);
    for (int i = 0; i < ehdr->phnum; ++i, ++ph) {
        if (ph->type != PT_LOAD)
            continue;

        void *src = elf_buf + ph->offset;
        void *dst = (void *)ph->paddr;

        memcpy(dst, src, ph->filesz);
        if (ph->memsz > ph->filesz)
            memset(dst + ph->filesz, 0, ph->memsz - ph->filesz);
    }

    uart_puts("Jumping to user entry...\n");
    enter_user_mode(ehdr->entry);
    return 0;
}
