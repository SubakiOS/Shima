#include "idt.h"
#include "../../../kernel/log/klog.h"

#define IDT_ENTRIES 256

static struct InterruptDescriptor64 idt[IDT_ENTRIES];

_Static_assert(sizeof(struct InterruptDescriptor64) == 16,
               "IDT entry must be 16 bytes");

_Static_assert(sizeof(idt) == 4096,
               "IDT must be 4096 bytes");


extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq32(void);
extern void irq33(void);
extern void irq34(void);
extern void irq35(void);
extern void irq36(void);
extern void irq37(void);
extern void irq38(void);
extern void irq39(void);
extern void irq40(void);
extern void irq41(void);
extern void irq42(void);
extern void irq43(void);
extern void irq44(void);
extern void irq45(void);
extern void irq46(void);
extern void irq47(void);




static void (*const exception_handlers[32])(void) = {
    isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7,
    isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31
};

static void (*const irq_stubs[16])(void) = {
    irq32, irq33, irq34, irq35,
    irq36, irq37, irq38, irq39,
    irq40, irq41, irq42, irq43,
    irq44, irq45, irq46, irq47
};

static void idt_set_gate(
    uint8_t vector,
    uint64_t handler,
    uint16_t selector,
    uint8_t type_attributes,
    uint8_t ist
) {
    idt[vector].offset_1 = handler & 0xFFFF;
    idt[vector].selector = selector;
    idt[vector].ist = ist & 0x07;
    idt[vector].type_attributes = type_attributes;
    idt[vector].offset_2 = (handler >> 16) & 0xFFFF;
    idt[vector].offset_3 = (handler >> 32) & 0xFFFFFFFF;
    idt[vector].zero = 0;
}

__attribute__((noreturn))
void idt_test_double_fault(void) {
    asm volatile ("cli");

    idt[13].selector = 0x28;

    asm volatile ("int $13");

    for (;;) {
        asm volatile ("cli; hlt");
    }
}
void idt_init(void) {


    for (uint8_t i = 0; i < 32; i++) {
        idt_set_gate( 
            i,
            (uint64_t)(uintptr_t)exception_handlers[i],
            0x08,
            0x8E,
            (i == 8) ? 1 : 0
        );
    }
      for (uint8_t i = 0; i < 16; i++) {
        idt_set_gate( 
            32 + i,
            (uint64_t)(uintptr_t)irq_stubs[i],
            0x08,
            0x8E,
            0
        );
    }

    uint64_t irq0_address =
        (uint64_t)idt[32].offset_1 |
        ((uint64_t)idt[32].offset_2 << 16) |
        ((uint64_t)idt[32].offset_3 << 32);

    klog_info(irq0_address == (uint64_t)(uintptr_t)irq32
        ? "IRQ0 IDT address OK"
        : "IRQ0 IDT address WRONG");

        struct IDTPointer idtr = {
            .limit = sizeof(idt) - 1,
            .base = (uint64_t)(uintptr_t)&idt
        };

    
        idt_load(&idtr);

        struct IDTPointer loaded_idtr;

asm volatile("sidt %0" : "=m"(loaded_idtr));

klog_info(loaded_idtr.base == idtr.base
    ? "LOADED IDT BASE OK"
    : "LOADED IDT BASE WRONG");

klog_info(loaded_idtr.limit == idtr.limit
    ? "LOADED IDT LIMIT OK"
    : "LOADED IDT LIMIT WRONG");
};
