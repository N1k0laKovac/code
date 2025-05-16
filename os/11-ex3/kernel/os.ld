#include "platform.h"

OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 256K
}

SECTIONS
{
    .text : {
        PROVIDE(_text_start = .);
        *(.text .text.*)
        PROVIDE(_text_end = .);
    } > ram

    .rodata : {
        . = ALIGN(8);
        PROVIDE(_rodata_start = .);
        *(.rodata .rodata.*)
        PROVIDE(_rodata_end = .);
    } > ram

    .data : {
        . = ALIGN(8);
        PROVIDE(_data_start = .);
        *(.sdata .sdata.*)
        *(.data .data.*)
        PROVIDE(_data_end = .);
    } > ram

    .bss : {
        . = ALIGN(8);
        PROVIDE(_bss_start = .);
        *(.sbss .sbss.*)
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    } > ram

    /* 如果你确实需要 .user_elf，可以仅包含自定义段 */
    .user_elf : {
        . = ALIGN(8);
        __user_elf_start = .;
        *(.user_elf*)
        __user_elf_end = .;
    } > ram

    /* 提供整个内存范围信息 */
    PROVIDE(_memory_start = ORIGIN(ram));
    PROVIDE(_memory_end = ORIGIN(ram) + LENGTH(ram));

    /* 堆从 BSS 段之后开始 */
    PROVIDE(_heap_start = _bss_end);
    PROVIDE(_heap_size = _memory_end - _heap_start);
}
