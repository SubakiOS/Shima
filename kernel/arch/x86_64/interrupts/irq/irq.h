#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

typedef void (*irq_handler_t)(void);

void irq_register_handler(uint8_t irq, irq_handler_t handler);
void irq_dispatch(uint64_t irq);
extern volatile uint64_t irq_debug_count;

#endif
