#include "tss.h"


_Static_assert(sizeof(struct tss) == 104, "TSS size must be 104 bytes");
static struct tss kernel_tss;

static uint8_t double_fault_stack[16384]
       __attribute__((aligned(16)));

void tss_init(void) {
    kernel_tss.rsp0 = 0;

    kernel_tss.ist1 = (uint64_t)(double_fault_stack + sizeof(double_fault_stack));

    kernel_tss.iomap_base = sizeof(struct tss);
}

struct tss *tss_get(void) {
    return &kernel_tss;
}


uint64_t tss_get_ist1(void) {
     return kernel_tss.ist1;
}