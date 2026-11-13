#include "klog.h"
#include "../../arch/x86_64/serial/serial.h"

void klog(const char *message) {
    serial_write_string("[SubakiOS] ");
    serial_write_string(message);
    serial_write_string("\r\n");
}