#include "pic.h"
#include "../../io/io.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

void pic_remap(void) {
    outb(PIC1_COMMAND, 0x11);
    io_wait();

    outb(PIC2_COMMAND, 0x11);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 0x04);
    io_wait();

    outb(PIC2_DATA, 0x02);
    io_wait();

    outb(PIC1_DATA, 0x01);
    io_wait();
    
    outb(PIC2_DATA, 0x01);
    io_wait();

    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_clear_mask(uint8_t irq) {
    if (irq >=16) {
        return;
    }

    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t line = irq % 8;

    uint8_t mask = inb(port);
    outb(port, mask & ~(1 << line));

    if (irq >= 8) {
        mask = inb(PIC1_DATA);
        outb(PIC1_DATA, mask & ~(1 << 2));
    }
}

void pic_set_mask(uint8_t irq) {
    if (irq >= 16) {
        return;
    }

    uint16_t port = (irq < 8) ? PIC1_DATA : PIC2_DATA;
    uint8_t line = irq % 8;

    uint8_t mask = inb(port);
    outb(port, mask | (1U << line));
}