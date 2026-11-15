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

volatile uint64_t irq0_address = 0;
volatile uint64_t irq0_callback_count = 0;

static void timer_irq_handler(void) {
    irq0_callback_count++;
}
// kern entry pint

void Kmain(void) {

    asm volatile ("cli");

    struct limine_framebuffer *fb = limine_get_framebuffer();

    if (init_serial() != 0) {
        hcf();

    }
    uint64_t hhdm_offset = limine_get_hhdm_offset();

    klog_info(hhdm_offset
              ? "HHDM response received"
              : "HHDM response missing");


    uint64_t paging_mode = limine_get_paging_mode();

    klog_info(paging_mode == LIMINE_PAGING_MODE_X86_64_4LVL
              ? "Four-level paging confirmed"
              : "Unexpected paging mode, missing response");

    uint32_t apic_lo, apic_hi;

     asm volatile(
         "rdmsr"
         : "=a"(apic_lo), "=d"(apic_hi)
         : "c"(0x1B)
     );

    uint64_t apic_msr = ((uint64_t)apic_hi << 32) | apic_lo;

    klog_info((apic_msr & (1ULL << 11))
               ? "Local APIC enabled"
               : "Local APIC disabled");

    klog_info((apic_msr & (1ULL << 10))
               ? "x2APIC mode enabled"
               : "xAPIC MMIO mode");
    

    uint32_t eax, ebx, ecx, edx;

    asm volatile(
        "cpuid"
        : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
        : "a"(1), "c"(0)
    );
    if ((ecx & (1U << 21)) && (apic_msr & (1ULL << 11))) {
    apic_msr |= (1ULL << 10);

    asm volatile(
        "wrmsr"
        :
        : "c"(0x1B),
          "a"((uint32_t)apic_msr),
          "d"((uint32_t)(apic_msr >> 32))
    );

    uint32_t lo, hi;
    asm volatile(
        "rdmsr"
        : "=a"(lo), "=d"(hi)
        : "c"(0x1B)
    );

    uint64_t check = ((uint64_t)hi << 32) | lo;

    klog_info((check & (1ULL << 10))
        ? "x2APIC mode successfully enabled"
        : "x2APIC mode enable failed");
}
    
    uint32_t lint0_lo, lint0_hi;

    asm volatile(
        "rdmsr"
        : "=a"(lint0_lo), "=d"(lint0_hi)
        : "c"(0x835)
    );

    uint64_t lint0 = ((uint64_t)lint0_hi << 32) | lint0_lo;

    serial_write_string("LINT0: ");
    serial_write_hex(lint0);
    serial_write_string("\n");
    klog_info((ecx & (1U << 21))
    ? "CPU supports x2APIC"
    : "CPU does not support x2APIC");

    uint64_t lint0_new = lint0 & ~(1ULL << 16);

    asm volatile(
        "wrmsr"
        :
        : "c"(0x835),
          "a"((uint32_t)lint0_new),
          "d"((uint32_t)(lint0_new >> 32))

    );

    // uint64_t apic_phys = apic_msr & 0x0000000FFFFFF000ULL;
    // uint64_t apic_virt = limine_get_hhdm_offset() + apic_phys;

    // volatile uint32_t *lapic = (volatile uint32_t *)apic_virt;

    // uint32_t apic_id = lapic[0x20 / 4];

    // uint32_t lvt0 = lapic[0x350 / 4];

    // klog_info("LAPIC MMIO is accessible");

    // klog_info((lvt0 & (1U << 16))
    //          ? "LVT0 is masked"
    //          : "LVT0 is Unmasked");
            
    
    //klog_info((lvt0 & 0x700) == 0x700
    //? "LVT0 delivery mode is ExtINT"
    //: "LVT0 delivery mode is NOT ExtINT");
    klog_info("GDT & IDT START BEGIN");
    gdt_init();
    idt_init();
    struct limine_memmap_response  *memmap = limine_get_memmap();

    if (memmap == NULL) {
        klog_error("Memory map resp missing");

    } else {
        klog_info("MEM map resp grapsed");
        klog_info("Starting mem map scam");
        serial_write_string("Entery count: ");
        serial_write_hex(memmap->entry_count);
        serial_write_string("\n");

    for (uint64_t i = 0; i < memmap->entry_count; i++) {
        struct limine_memmap_entry *entry = memmap->entries[i];
         
        serial_write_string("Base: ");
        serial_write_hex(entry->base);

        serial_write_string(" Length: ");
        serial_write_hex(entry->length);

        serial_write_string("Type: ");
        serial_write_hex(entry->type);

        serial_write_string("\n");

        if (0xFEE00000ULL >= entry->base &&
            0xFEE00000ULL - entry->base < entry->length) {
                klog_info("Lapic adduy found");

                if (entry->type == LIMINE_MEMMAP_RESERVED)
                klog_info("LAPIC region is reserved");

                else if (entry->type == LIMINE_MEMMAP_RESERVED_MAPPED)
                klog_info("LAPIC region is reserved map");

                else if (entry->type == LIMINE_MEMMAP_USABLE)
                klog_info("LAPIC region is marked USABLE");

                else 
                   klog_info("LAPIC region has another mem type");

                break;
            }
            
    }
    klog_info("MEM map scan complete");
}
    klog_info("PIC REMAP INCOMING");
    pic_remap();
    pit_init(100);
    pic_clear_mask(0);
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
 

irq_register_handler(0, timer_irq_handler);
irq_unregister_handler(0);


    asm volatile ("sti");

    uint64_t flags;
asm volatile("pushfq; popq %0" : "=r"(flags));

klog_info((flags & (1ULL << 9))
    ? "CPU interrupts ENABLED"
    : "CPU interrupts DISABLED");

    

for (;;) {
    if (irq_entry_count >= 10000) {
        klog_info("IRQ assembly entry received 10000 interrupts!");
        serial_write_string("Asssembly enteries");
        serial_write_hex(irq_entry_count);

        serial_write_string("\nC dispatches: ");
        serial_write_hex(irq_debug_count);

        serial_write_string("\nC callbacks: ");
        serial_write_hex(irq0_callback_count);

        serial_write_string("\n");
        hcf();
    }

    asm volatile("hlt");
}
}