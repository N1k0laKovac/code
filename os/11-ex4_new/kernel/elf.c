#include "elf.h"
#include "os.h"
#include "string.h"

#define EI_MAG0   0
#define EI_MAG1   1
#define EI_MAG2   2
#define EI_MAG3   3

extern void enter_user_mode(uint32_t entry);  // ��Ϊ uint32_t

// �ж� ELF32 magic
static inline int is_valid_elf32(const unsigned char *e_ident) {
    return e_ident[EI_MAG0] == 0x7F
        && e_ident[EI_MAG1] == 'E'
        && e_ident[EI_MAG2] == 'L'
        && e_ident[EI_MAG3] == 'F';
}

// �� ELF32 ������ص������ڴ棬����ת
int load_elf_image(uint8_t *elf_buf) {
    uart_puts(">>> load_elf_image called\n");

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)elf_buf;
    if (!is_valid_elf32(ehdr->e_ident)) {
        uart_puts("Invalid ELF magic!\n");
        // ���У��ʧ�ܣ��ͱ�����ˣ�ֱ��ͣ������
        panic("ELF load failed");
        return -1; 
    }

    uart_puts("ELF magic OK. Loading program segments...\n");

    // ���� program headers
    Elf32_Phdr *ph = (Elf32_Phdr *)(elf_buf + ehdr->e_phoff);
    for (int i = 0; i < ehdr->e_phnum; i++, ph++) {
        if (ph->p_type != PT_LOAD) 
            continue;
        void *src = elf_buf + ph->p_offset;
        void *dst = (void *)(ph->p_paddr);
        memcpy(dst, src, ph->p_filesz);
        if (ph->p_memsz > ph->p_filesz) {
            memset(dst + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        }
    }

    uart_puts("Jumping to user entry...\n");
    enter_user_mode(ehdr->e_entry);
    return 0;
}
