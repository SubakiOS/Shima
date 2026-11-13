#ifndef PANIC_H
#define PANIC_H

__attribute__((noreturn))
void kernel_panic(const char *reason);

__attribute__((noreturn))
void kernel_panic_at(const char *reason, const char *file, int line);

#define panic(reason) kernel_panic_at(reason, __FILE__, __LINE__)

#endif