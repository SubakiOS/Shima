#include "terminal.h"
#include "../arch/x86_64/framebuffer/framebuffer.h"

static struct terminal term;

int terminal_init(struct limine_framebuffer *fb,
                  struct limine_file *font) {
                    if (fb == NULL || font == NULL || font->address == NULL) {
                        return -1;
                    }

                     if (font->size < sizeof(struct psf1_header) + (256 * 16)) {
                            return -1; 
                        }

                    const struct psf1_header *header = font->address;

                    if (header->magic[0] != 0x36 ||
                        header->magic[1] != 0x04 ||
                        header->charsize != 16) {
                            return -1;
                        }
                        term.fb = fb;
                        term.glyph_buffer = (const uint8_t *)font->address + sizeof(struct psf1_header);



                        term.cursor_x = 0;
                        term.cursor_y = 0;

                        term.columns = fb->width / 8;
                        term.rows = fb->height / 16;

                      
                        return 0;
                  }

void terminal_putchar(char c) {

    if (term.fb ==  NULL || term.glyph_buffer == NULL) {
        return;
    }

     if (term.cursor_x >= term.columns) {
        term.cursor_x = 0;
        term.cursor_y++;
     }

     if (term.cursor_y >= term.rows) {
        return;
     }

    uint8_t index = (uint8_t)c;
    const uint8_t *glyph = term.glyph_buffer + (index * 16);


    for (uint32_t y = 0;  y < 16; y++ ) {
        for (uint32_t x = 0; x < 8; x++)  {
            if (glyph[y] & (0x80 >> x)) {
                fb_put_pixel(term.fb, 
                   term.cursor_x * 8 + x,
                   term.cursor_y * 16 + y,
                   255, 255, 255);
            } else  {
                fb_put_pixel(term.fb,
                             term.cursor_x * 8 + x,
                             term.cursor_y * 16 + y,
                             0, 0, 0);
            }
        }
    }
    term.cursor_x++;
}

void terminal_write(const char *str) {
    if  (str == NULL) {
        return;
    }

    while (*str != '\0') {
        if (*str == '\n') {
            term.cursor_x = 0;
            term.cursor_y++;
        } else {
        terminal_putchar(*str);
    }
        str++;
    }
}