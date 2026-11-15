#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

int init_serial(void);
int serial_reveived(void);
char read_serial(void);
int is_transmit_empty(void);
void write_serial(char a);
void serial_write_string(const char *str);
void serial_write_hex(uint64_t value);


#endif
