#include "idt.h"
#include "../../kernel/log/klog.h"
#include "../../kernel/panic/panic.h"

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
    klog_error(exception_name(frame->vector));
    panic("Unhandeled CPU exception");
}