#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../arch/x86_64/framebuffer/framebuffer.h"
#include "limine_boot.h"
#include "../arch/x86_64/serial/serial.h"
#include "log/klog.h"
#include <string.h>



static void hcf(void) {
    for (;;) {
        asm ("hlt");
    }
}

// kern entry pint

void Kmain(void) {

    struct limine_framebuffer *fb = limine_get_framebuffer();

    if  (fb == NULL) {
        hcf();
    }

    if (init_serial() != 0) {
        hcf();

    }

    klog("SubakiOS: Kernel intialized. \r\n");
    klog("Frambuffy is UPPY");
    fb_pattern(fb);
    hcf();
}