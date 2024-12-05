#define PAGE_SIZE 0x1000
#define STACK_SIZE PAGE_SIZE

#define LOAD_BASE_ADDR 0x80000000
#define LOAD_BASE_OFFSET 0x00200000
#define LOAD_OFFSET (LOAD_BASE_ADDR + LOAD_BASE_OFFSET)

OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS {
	. = LOAD_OFFSET;
	_start = .;
	. = ALIGN(PAGE_SIZE);

	.text : {
		_text = .;
		KEEP(*(.head.text))
		. = ALIGN(PAGE_SIZE);
    	*(.text)
	}
	. = ALIGN(PAGE_SIZE);

	.data : {
    	*(.data)
	}
	. = ALIGN(8);

	.rodata : {
    	*(.rodata)
	}
	. = ALIGN(8);

	.sdata : {
		*(.sdata*)
	}
	. = ALIGN(8);

	.bss : {
    	*(.bss)
	}

	. = ALIGN(8);
	. = . + STACK_SIZE;
	_STACK_PTR = .;

	_end = .;
}
