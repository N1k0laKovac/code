#include "types.h"
#pragma once



// 32 位 ELF Header
typedef struct {
    unsigned char e_ident[16];
    uint16_t      e_type;
    uint16_t      e_machine;
    uint32_t      e_version;
    uint32_t      e_entry;    // 入口点
    uint32_t      e_phoff;    // Program header 偏移
    uint32_t      e_shoff;
    uint32_t      e_flags;
    uint16_t      e_ehsize;
    uint16_t      e_phentsize;
    uint16_t      e_phnum;    // Program header 数量
    uint16_t      e_shentsize;
    uint16_t      e_shnum;
    uint16_t      e_shstrndx;
} Elf32_Ehdr;

// 32 位 Program Header
typedef struct {
    uint32_t p_type;    // 段类型
    uint32_t p_offset;  // 文件内偏移
    uint32_t p_vaddr;
    uint32_t p_paddr;   // 物理地址
    uint32_t p_filesz;  // 文件中大小
    uint32_t p_memsz;   // 内存中大小
    uint32_t p_flags;
    uint32_t p_align;
} Elf32_Phdr;


#define PT_LOAD 1

int load_elf_image(uint8_t *elf);
