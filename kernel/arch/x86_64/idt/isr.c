#include "idt.h"
#include "../../../kernel/log/klog.h"
#include "../../../kernel/panic/panic.h"
#include "../../arch/x86_64/serial/serial.h"

static const char *exception_name(uint64_t vector) {
    switch (vector) {
        case 0: return "Divison by 0";
        case 1: return "Debug";
        case 2: return "Non-mask interrupt";
        case 3: return "BreakhhhPOint";
        case 4: return "OverFlowww";
        case 5: return "Bound Range Exceeded";
        case 6: return "invalid Opcode";
        case 7: return "Dih-vice not Abhilable";
        case 8: return "Dauble Fealut";
        case 9: return "CoProc Segm Ran OVer";
        case 10: return "Inbhaild TSS (THis Shit Shit)?";
        case 11: return "Segm not there bruh";
        case 12: return "Stack-Segm Fault";
        case 13: return "General Protection Fault";
        case 14: return "Page Failed";
        case 16: return "x87 Floating Point Exception";
        case 17: return "Alignment Check";
        case 18: return "Mac-hine check";
        case 19: return "SIDm Floating-Point Exception";
        case 20: return "Bhirtualization Exception";
        case 21: return  "Kontrol Protecktion Exception";
        case 29: return "VMM Kammunication Exception";
        case 30: return "Security Exception";
        default: return "Unknown CPU Exception"; 
    }
};

__attribute__((noreturn))
void isr_handler(struct InterruptFrame *frame) {

    uint64_t current_rsp;
    asm volatile ("mov %%rsp, %0" : "=r"(current_rsp));

    klog_error(exception_name(frame->vector));
    if (frame->vector == 8) {
        serial_write_string("Double fault: ist1 handler reached\r\n");
        serial_write_string("RSP:");
        serial_write_hex(current_rsp);
        serial_write_string("\r\n");
    }
    
    serial_write_string("Exception vector: ");
    serial_write_hex(frame->vector);
    serial_write_string("\r\n");

    serial_write_string("Weerror code: ");
    serial_write_hex(frame->error_code);
    serial_write_string("\r\n");

    serial_write_string("RIP: ");
    serial_write_hex(frame->rip);
    serial_write_string("\r\n");


    if (frame->vector == 14) {
    uint64_t cr2;

    asm volatile("mov %%cr2, %0" : "=r"(cr2));

    serial_write_string("Page fault address (CR2): ");
    serial_write_hex(cr2);
    serial_write_string("\r\n");

    serial_write_string("Page fauly error code: ");
    serial_write_hex(frame->error_code);
    serial_write_string("\r\n");

    if (frame->error_code & (1ULL << 0))
        serial_write_string("Protection violation\r\n");
    else
        serial_write_string("Page not present\r\n");

    if (frame->error_code & (1ULL << 1))
    serial_write_string("Write access\r\n");
    else
    serial_write_string("Read access\r\n");

    if (frame->error_code & (1ULL << 2))
    serial_write_string("User-mode access\r\n");
    else 
    serial_write_string("Supervisor-mode access\r\n");

    if (frame->error_code & (1ULL << 3))
        serial_write_string("Reserved-bit violation\r\n");

    if (frame->error_code & (1ULL << 4))
    serial_write_string("Instruction fetch\r\n");

}
  panic("Unhandeled CPU exception");
}

