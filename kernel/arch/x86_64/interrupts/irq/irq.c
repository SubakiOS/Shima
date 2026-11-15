#include "irq.h"
#include "../../io/io.h"
#include "../pic/pic.h"

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0

static irq_handler_t irq_handlers[16] = {0};

void irq_register_handler(uint8_t irq, irq_handler_t handler) {
    if (irq >=16) {
        return;
    }

    irq_handlers[irq] = handler;
}

void irq_unregister_handler(uint8_t irq) {
    if (irq >= 16) {
        return;
    }

    irq_handlers[irq] = 0;
}


void irq_mask(uint8_t irq) {
    pic_set_mask(irq);
}

void irq_unmask(uint8_t irq) {
    pic_clear_mask(irq);
}

volatile uint64_t irq_debug_count = 0;
volatile uint64_t irq_entry_count = 0;
  void irq_dispatch(uint64_t irq) {
    
    if (irq >= 16) {
        return;
    }

    if (irq == 0) {
        irq_debug_count++;
    }

    if (irq_handlers[irq] != 0) {
        irq_handlers[irq]();
    }

    if (irq >= 8) {
        outb(PIC2_COMMAND, 0x20);
    }

    outb(PIC1_COMMAND, 0x20);
}
