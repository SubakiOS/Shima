#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../arch/x86_64/framebuffer/framebuffer.h"
#include "limine_boot.h"
#include "../arch/x86_64/serial/serial.h"
#include "log/klog.h"
#include "panic/panic.h"
#include "panic/kassert.h"
#include "../arch/x86_64/gdt/gdt.h"
#include "../arch/x86_64/idt/idt.h"
#include <string.h>



static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// kern entry pint

void Kmain(void) {

    struct limine_framebuffer *fb = limine_get_framebuffer();

    if (init_serial() != 0) {
        hcf();

    }
    klog_info("GDT & IDT START BEGIN");
    gdt_init();
    idt_init();
    klog_info("GDT & IDT INTIALIZED");
    // panic("PWEEEE PWEEE A KERNUL PANICCCCCCC");

    klog_warn("test gpf init");
    asm volatile (
        "mov $0xFFFF, %%ax\n\t"
        "mov %%ax, %%ds"
        :
        :
        : "rax", "memory"
    );

    klog_info("SubakiOS: Kernel intialized. \r\n");
    klog_info("Frambuffy is UPPY");
    klog_error("HI, I AM AN ERROR");
    klog_warn("HI, THERE A LITTLE CUTESY WARNING... WHICH MIGHT NUKE THIS LATER");
    kassert(fb != NULL, "Framebuffer is NULL");
    fb_pattern(fb);
 
    hcf();
}