OUTPUT_FORMAT(elf64-x86-64)

ENTRY(Kmain)

PHDRS
{
    requests PT_LOAD FLAGS(6);
    kernel PT_LOAD FLAGS(7);

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

    .data : ALIGN(CONSTANT(MAXPAGESIZE)) {
        *(.data .data.*)
    } :kernel

    .bss (NOLOAD) : ALIGN(16) {
        *(.bss .bss.*)
        *(COMMON)
    } :kernel

    /DISCARD/ : {
        *(.eh_frame*)
        *(.note .note.*)
    }
}
