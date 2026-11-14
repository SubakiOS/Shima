#include "irq.h"
#include "../../io/io.h"

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0

static irq_handler_t irq_handlers[16] = {0};

void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if (irq >=16) {
        return;
    }

    irq_handlers[irq] = handler;
}

volatile uint64_t irq_debug_count = 0;
  void irq_dispatch(uint64_t irq) {
    irq_debug_count++;
    if (irq >= 16) {
        return;
    }

    if (irq_handlers[irq] != 0) {
        irq_handlers[irq]();
    }

    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }

    outb(PIC1_COMMAND, 0x20);
}