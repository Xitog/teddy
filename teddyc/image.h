#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h> // int32_t
#include <stdio.h> // FILE, fopen, fwrite, fclose
#include <stdlib.h> // malloc, memcpy, free
#include <stdbool.h> // bool
#include <math.h>

typedef struct PixelStructure {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Pixel;

extern const Pixel RED;
extern const Pixel BLUE;
extern const Pixel GREEN;
extern const Pixel BLACK;
extern const Pixel WHITE;
extern const Pixel GREY;
extern const Pixel NEAR_WHITE;
extern const Pixel MAGENTA;

typedef struct ImageStructure {
    uint32_t width;
    uint32_t height;
    Pixel ** rows;
} Image;

Image * image_new(uint32_t width, uint32_t height);
void image_draw_digit(Image * dest, uint32_t x, uint32_t y, uint8_t digit, Pixel color);
void image_draw_line(Image * dest, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Pixel color);
void image_draw_image(Image * dest, uint32_t x, uint32_t y, Image * source);
void image_draw_rect(Image * img, uint32_t x, uint32_t y, uint32_t width, uint32_t height, Pixel color, bool filled);
void image_safe_set_pixel(Image * img, int32_t x, int32_t y, Pixel color);
void image_set_pixel(Image * img, uint32_t x, uint32_t y, Pixel color);
void image_fill(Image * img, Pixel color);
void image_save_to_bmp(Image * img, const char * file_path);
void image_free(Image * img);

#endif
