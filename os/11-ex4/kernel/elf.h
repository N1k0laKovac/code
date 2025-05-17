#include "types.h"
#pragma once



// 32 λ ELF Header
typedef struct {
    unsigned char e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint32_t      e_entry;    // ��ڵ�
    uint32_t      e_phoff;    // Program header ƫ��
    uint32_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;    // Program header ����
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf32_Ehdr;

// 32 λ Program Header
typedef struct {
    uint32_t p_type;    // ������
    uint32_t p_offset;  // �ļ���ƫ��
    uint32_t p_vaddr;
    uint32_t p_paddr;   // �����ַ
    uint32_t p_filesz;  // �ļ��д�С
    uint32_t p_memsz;   // �ڴ��д�С
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;


#define PT_LOAD 1

int load_elf_image(uint8_t *elf);
