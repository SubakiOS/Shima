#include "klog.h"
#include "../../arch/x86_64/serial/serial.h"

static void klog_level(const char *level, const char *message) {
    serial_write_string("[");
    serial_write_string(level);
    serial_write_string("] [SubakiOS] ");
    serial_write_string(message);
    serial_write_string("\r\n");
}
//
// void klog(const char *message) {
//    serial_write_string("[SubakiOS] ");
//    serial_write_string(message);
//    serial_write_string("\r\n");
// }
//
void klog_info(const char *message) {
    klog_level("INFO", message);
}

void klog_warn(const char *message) {
    klog_level("WARN", message);
}

void klog_error(const char *message) {
    klog_level("ERROR", message);
}