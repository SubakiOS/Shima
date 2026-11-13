#include "panic.h"
#include "../log/klog.h"
#include "../../arch/x86_64/serial/serial.h"

static void panic_print_number(int number) {
    char buffer[12];
    int i = 0;

    if (number == 0) {
        write_serial('0');
        return;
    }

    while (number > 0) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }

    while (i > 0) {
        write_serial(buffer[--i]);

    }
}
__attribute__((noreturn))
void kernel_panic(const char *reason) {
    klog_error("KERNEL PANIC");
    klog_error(reason);

    for (;;) {
        asm volatile ("cli; hlt");
    }
}
__attribute__((noreturn))
void kernel_panic_at(const char *reason, const char *file, int line) {
    klog_error("KERNEL PANIC");
    klog_error(reason);

    serial_write_string(" [FILE] ");
    serial_write_string(file);

    serial_write_string(" [LINE] ");
    panic_print_number(line);
    serial_write_string("\r\n");



    for (;;) {
        asm volatile ("cli; hlt");
    }

}