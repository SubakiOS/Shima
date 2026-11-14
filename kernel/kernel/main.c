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
#include "../arch/x86_64/interrupts/pic/pic.h"
#include "../arch/x86_64/pit/pit.h"
#include "../arch/x86_64/interrupts/irq/irq.h"
#include "../arch/x86_64/io/io.h"
#include <string.h>



static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// kern entry pint

void Kmain(void) {

    asm volatile ("cli");

    struct limine_framebuffer *fb = limine_get_framebuffer();

    if (init_serial() != 0) {
        hcf();

    }
    klog_info("GDT & IDT START BEGIN");
    gdt_init();
    idt_init();
    klog_info("PIC REMAP INCOMING");
    pic_remap();
    pit_init(100);
    pic_clear_mask(0);
    inb(0x21);
    klog_info("PIC master mask:");
klog_info(inb(0x21) == 0xFE ? "IRQ0 unmasked" : "IRQ0 still masked");
    klog_info("The pic remap came");
    klog_info("GDT & IDT INTIALIZED");
    // panic("PWEEEE PWEEE A KERNUL PANICCCCCCC");

    klog_info("SubakiOS: Kernel intialized. \r\n");
    klog_info("Frambuffy is UPPY");
    klog_error("HI, I AM AN ERROR");
    klog_warn("HI, THERE A LITTLE CUTESY WARNING... WHICH MIGHT NUKE THIS LATER");
    kassert(fb != NULL, "Framebuffer is NULL");
    fb_pattern(fb);
    
    klog_info("PIT initzd. Enabling intups");
    outb(0x20, 0x0A);
uint8_t irr = inb(0x20);

klog_info(irr & 1 ? "PIT IRQ pending" : "No PIT IRQ pending");
    asm volatile ("sti");

    for (;;) {
        if (irq_debug_count >= 100) {
            klog_info("IRQ dispatcher recd 100 int");
        }
        asm volatile ("hlt");
    }
    uint64_t last_tick = 0;

    for (;;) {
        uint64_t current_tick = pit_get_ticks();

        if (current_tick >= last_tick + 100) {
            last_tick = current_tick;
            klog_info("PIT: 1 second elapsed");
        }
        asm volatile ("hlt");
    }
}