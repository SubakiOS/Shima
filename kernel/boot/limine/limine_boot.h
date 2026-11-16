#ifndef LIMINE_BOOT_H
#define LIMINE_BOOT_H

#include <limine.h>
#include <stdint.h>

struct limine_framebuffer *limine_get_framebuffer(void);
uint64_t limine_get_hhdm_offset(void);
uint64_t limine_get_paging_mode(void);
struct limine_file *limine_get_font(void);
struct limine_memmap_response *limine_get_memmap(void);

#endif