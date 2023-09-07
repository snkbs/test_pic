FLASH_SIZE = 64k ;
RAM_SIZE = 20k ;
RAM_ISR_SIZE = 512 ;


MEMORY
{
	ram_isr (rwx) : ORIGIN = 0x20000000, LENGTH = RAM_ISR_SIZE
	ram (rwx) : ORIGIN = ORIGIN(ram_isr) + LENGTH(ram_isr), LENGTH = RAM_SIZE - LENGTH(ram_isr)
	rom (rx) : ORIGIN = 0x08000000, LENGTH = FLASH_SIZE	
}


EXTERN(vector_table)
ENTRY(reset_handler)

PROVIDE(_stack = ORIGIN(ram) + LENGTH(ram));
PROVIDE(_flash_begin = ORIGIN(rom));
PROVIDE(_flash_end = ORIGIN(rom) + LENGTH(rom));

SECTIONS
{
	.vectors : {
		. = ALIGN(4);
		_vectors_ram_begin = .;
		KEEP(*(.vectors))
		. = ALIGN(4);
		_vectors_ram_end = .;
	} >ram_isr AT >rom
	_vectors_rom_begin = LOADADDR(.vectors) ;
	_vectors_rom_end = _vectors_rom_begin + SIZEOF(.vectors) ;

	.text : {
		. = ALIGN(4);
		*(.text*)
		. = ALIGN(4);
		*(.rodata*)
		. = ALIGN(4);
	} >rom

	.preinit_array : {
		. = ALIGN(4);
		__preinit_array_start = .;
		KEEP (*(.preinit_array))
		__preinit_array_end = .;
	} >rom
	.init_array : {
		. = ALIGN(4);
		__init_array_start = .;
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array))
		__init_array_end = .;
	} >rom
	.fini_array : {
		. = ALIGN(4);
		__fini_array_start = .;
		KEEP (*(.fini_array))
		KEEP (*(SORT(.fini_array.*)))
		__fini_array_end = .;
	} >rom

	.ARM.extab : {
		*(.ARM.extab*)
	} >rom
	.ARM.exidx : {
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >rom

	. = ALIGN(4);
	_rom_end = .;


	.noinit (NOLOAD) : {
		*(.noinit*)
	} >ram
	. = ALIGN(4);

	.got : {
		. = ALIGN(4);
		_got_ram_begin = .;
		*(.got*)
		. = ALIGN(4);
		_got_ram_end = .;
	} >ram AT >rom
	_got_rom_begin = LOADADDR(.got) ;
	_got_rom_end = _got_rom_begin + SIZEOF(.got) ;
	
	.data : {
		_data_ram_begin = .;
		*(.data*)
		*(.ramtext*)
		. = ALIGN(4);
		_data_ram_end = .;
	} >ram AT >rom
	_data_rom_begin = LOADADDR(.data) ;
	_data_rom_end = _data_rom_begin + SIZEOF(.data) ;

	.bss : {
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_bss_ram_end = .;
	} >ram

	/DISCARD/ : { *(.eh_frame) }

	. = ALIGN(4);
	_ram_end = .;
}
