

#include "platform.h"


OUTPUT_ARCH( "riscv" )


ENTRY(_user_start)


MEMORY
{
	user_ram   (wxa!ri) : ORIGIN = 0x80400000, LENGTH = 128K
}


SECTIONS
{
	
	.text : {
        *(.text.entry)  # ????????
        *(.text .text.*)
    } > user_ram

	.rodata : {
		PROVIDE(_rodata_start = .);
		*(.rodata .rodata.*)
		PROVIDE(_rodata_end = .);
	} >user_ram

	.data : {
		
		. = ALIGN(4096);
		PROVIDE(_data_start = .);
		
		*(.sdata .sdata.*)
		*(.data .data.*)
		PROVIDE(_data_end = .);
	} >user_ram

	.bss :{
		
		PROVIDE(_bss_start = .);
		*(.sbss .sbss.*)
		*(.bss .bss.*)
		*(COMMON)
		PROVIDE(_bss_end = .);
	} >user_ram

	PROVIDE(_memory_start = ORIGIN(user_ram));
	PROVIDE(_memory_end = ORIGIN(user_ram) + LENGTH(user_ram));

	PROVIDE(_heap_start = _bss_end);
	PROVIDE(_heap_size = _memory_end - _heap_start);
}
