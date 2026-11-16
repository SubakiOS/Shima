#include "terminal.h"

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
