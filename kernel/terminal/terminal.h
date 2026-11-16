#ifndef TERMINAL_H
#define TERMINAL_H


#include <stdint.h>
#include <stddef.h>
#include <limine.h>

struct psf1_header { 
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
};

struct terminal {
    struct limine_framebuffer *fb;
    const uint8_t *glyph_buffer;

    uint32_t cursor_x;
    uint32_t cursor_y;

    uint32_t columns;
    uint32_t rows;
};

int terminal_init(struct limine_framebuffer *fb, 
                  struct limine_file *font);

void terminal_putchar(char c);

void terminal_write(const char *str);
#endif