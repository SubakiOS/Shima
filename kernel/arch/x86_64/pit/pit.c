#include "pit.h"
#include "../io/io.h"
#include "../interrupts/irq/irq.h"


#define PIT_CHANNEL_0 0x40
#define PIT_COMMAND 0x43
#define PIT_FREQUENCY 1193182

static volatile uint64_t ticks = 0;

static void pit_handler(void) {
    ticks++;
}

void pit_init(uint32_t frequency) {
    if (frequency == 0 || frequency > PIT_FREQUENCY) {
        return;
    }

    uint16_t divisor = PIT_FREQUENCY / frequency;

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL_0, divisor & 0xFF);
    outb(PIT_CHANNEL_0, (divisor >> 8) & 0xFF);

    irq_register_handler(0, pit_handler);
}

uint64_t pit_get_ticks(void) {
    return ticks;
}