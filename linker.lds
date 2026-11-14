OUTPUT_FORMAT(elf64-x86-64)

ENTRY(Kmain)

PHDRS
{
    requests PT_LOAD FLAGS(6);
    kernel PT_LOAD FLAGS(5);
    writable PT_LOAD FLAGS(6);

}

SECTIONS
{
    . = 0xffffffff80000000;

    .limine_requests : {
        KEEP(*(.limine_requests_start))
        KEEP(*(.limine_requests))
        KEEP(*(.limine_requests_end))
    } :requests

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .text : {
        *(.text .text.*)
    } :kernel

    .rodata : {
        *(.rodata .rodata.*)
    } :kernel

    . = ALIGN(CONSTANT(MAXPAGESIZE));

    .data :  {
        *(.data .data.*)
        *(.got .got.*)
        *(.got.plt)
        *(.igot.plt)
        BYTE(0)
    } :writable

    .bss (NOLOAD) : {
        *(.bss .bss.*)
        *(COMMON)
    } :writable

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
