#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <limine.h>

void fb_pattern(struct limine_framebuffer *fb);
void fb_put_pixel(struct limine_framebuffer *fb,
                  uint32_t x, uint32_t y,
                  uint8_t red, uint8_t green, uint8_t blue);
#endif

