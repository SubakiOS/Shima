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
#include "../arch/x86_64/tss/tss.h"
#include "../arch/x86_64/io/io.h"
#include "../terminal/terminal.h"
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
__attribute__((noinline))
static void test_divide_by_zero(void) {
    volatile int a = 10;
    volatile int b = 0;
    volatile int result;

    asm volatile(
        "idivl %2"
        : "=a"(result)
        : "a"(a), "r"(b), "d"(0)
        : "cc"
    );
}

__attribute__((noinline))
static void test_page_fault(void) {
    volatile uint64_t *ptr = 
    (volatile uint64_t *)0x0000400000000000ULL;

    volatile uint64_t value = *ptr;
    (void)value;
}
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
    serial_write_string("IST1 stack top:");
    serial_write_hex(tss_get_ist1());
    serial_write_string("\r\n");
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
    for (uint32_t y = 0; y < fb->height; y++) {
        for (uint32_t x = 0; x < fb->width; x++) {
            fb_put_pixel(fb, x ,y, 0, 0, 0);
        }
    }
    
    struct limine_file *font = limine_get_font();

    if (terminal_init(fb, font) != 0) {
        klog_error("termianl init failed");
    } else {
        klog_info("terminal init success");
        terminal_write("WELCOME TO SUBAKI OS~~~!\n");
        terminal_write("version 0.1\n");
        terminal_write("More coming soon!");
      
    }
    
    
    if (font == NULL) {
        klog_error("Font module not found");
    } else {
        klog_error("Font module found");

        serial_write_string("Font size: ");
        serial_write_hex(font->size);
        serial_write_string("\r\n");

        const uint8_t *magic = font->address;

        if (magic[0] == 0x36 && magic[1] == 0x04) {
            klog_info("PSF1 magic validated");
        } else {
            klog_error("Invalid PSF1 magic");
        }
    }
    
    serial_write_string("Frmbuf: ");
    serial_write_hex(fb->width);
    serial_write_string(" x ");
    serial_write_hex(fb->height);
    serial_write_string("\r\n");

    serial_write_string("Pitch ");
    serial_write_hex(fb->pitch);
    serial_write_string("\r\n");

    serial_write_string("BPP: ");
    serial_write_hex(fb->bpp);
    serial_write_string("\r\n");

    klog_info("PIT initzd. Enabling intups");
    outb(0x20, 0x0A);
uint8_t irr = inb(0x20);

klog_info(irr & 1 ? "PIT IRQ pending" : "No PIT IRQ pending");
 

irq_register_handler(0, timer_irq_handler);
irq_unregister_handler(0);


    irq_unmask(0);
    serial_write_string("PIC mask: ");
    serial_write_hex(inb(0x21));
    serial_write_string("\n");

    asm volatile ("sti");

    uint64_t flags;
asm volatile("pushfq; popq %0" : "=r"(flags));

klog_info((flags & (1ULL << 9))
    ? "CPU interrupts ENABLED"
    : "CPU interrupts DISABLED");

    

for (;;) {
    if (irq_entry_count >= 100) {
        klog_info("IRQ assembly entry received 100 interrupts!");
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