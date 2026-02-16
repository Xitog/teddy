#include "image.h"

const Pixel RED = {.r = 255, .g = 0, .b = 0};
;
const Pixel GREEN = {.r = 0, .g = 255, .b = 0};
const Pixel BLUE = {.r = 0, .g = 0, .b = 255};
const Pixel BLACK = {.r = 0, .g = 0, .b = 0};
const Pixel WHITE = {.r = 255, .g = 255, .b = 255};
const Pixel GREY = {.r = 112, .g = 112, .b = 112};
const Pixel NEAR_WHITE = {.r = 224, .g = 224, .b = 224};
const Pixel MAGENTA = {.r = 255, .g = 0, .b = 255};

Image *image_new(uint32_t width, uint32_t height)
{
    Image *img = malloc(sizeof(Image));
    if (img == NULL)
    {
        return NULL;
    }
    img->width = width;
    img->height = height;
    img->rows = malloc(sizeof(Pixel *) * height);
    for (int i = 0; i < height; i++)
    {
        img->rows[i] = malloc(sizeof(Pixel) * width);
    }
    image_fill(img, BLACK);
    return img;
}

void image_draw_digit(Image *dest, uint32_t x, uint32_t y, uint8_t digit, Pixel color)
{
    if (digit == 0)
    {
        image_draw_line(dest, x, y, x + 5, y, color);          // down
        image_draw_line(dest, x, y, x, y - 7, color);          // left
        image_draw_line(dest, x, y - 7, x + 5, y - 7, color);  // up
        image_draw_line(dest, x + 5, y, x + 5, y - 7, color);  // right
        image_draw_line(dest, x, y, x + 5, y - 7, color);      // diag
    } else if (digit == 1) {
        image_draw_line(dest, x, y, x + 5, y, color);          // base
        image_draw_line(dest, x + 3, y, x + 3, y - 7, color);  // hamp
        image_draw_line(dest, x + 3, y - 7, x, y - 3, color);  // diag
    }
}

void image_draw_line(Image *dest, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Pixel color)
{
    if (y2 == y1)
    {
        if (y1 >= 0 && y1 < dest->height)
        {
            int64_t min = (x1 < x2) ? x1 : x2;
            int64_t max = (x1 < x2) ? x2 : x1;
            for (int64_t x = min; x <= max; x += 1)
            {
                if (x >= 0 && x < dest->width)
                {
                    dest->rows[y1][x] = color;
                }
            }
        }
    }
    else if (x2 == x1)
    {
        if (x1 >= 0 && x1 < dest->width)
        {
            int64_t min = (y1 < y2) ? y1 : y2;
            int64_t max = (y1 < y2) ? y2 : y1;
            for (int64_t y = min; y <= max; y += 1)
            {
                if (y >= 0 && y < dest->width)
                {
                    dest->rows[y][x1] = color;
                }
            }
        }
    }
    else
    {
        // Bresenham's line algorithm as explained by Wikipedia
        // Do not handle thickness other than 1
        int64_t dy = y2 - y1;
        int64_t dx = x2 - x1;
        int64_t y = y1;
        double e = 0.0;
        double e10 = dy / dx;
        double e01 = -1.0;
        int64_t xmin = (x1 < x2) ? x1 : x2;
        int64_t xmax = (x1 < x2) ? x2 : x1;
        int64_t yincrement = (y1 < y2) ? 1 : -1;
        for (int64_t x = xmin; x <= xmax; x += 1)
        {
            if (x >= 0 && x < dest->width && y >= 0 && y < dest->height)
            {
                dest->rows[y][x] = color;
            }
            e = e + e10;
            if (e >= 0.5)
            {
                y += yincrement;
                e = e + e01;
            }
        }
    }
}

void image_draw_image(Image *dest, uint32_t x, uint32_t y, Image *source)
{
    for (int row = 0; row < source->height; row++)
    {
        for (int col = 0; col < source->width; col++)
        {
            // Making MAGENTA transparent
            if (source->rows[row][col].b != MAGENTA.b || source->rows[row][col].r != MAGENTA.r)
            {
                dest->rows[row + y][col + x] = source->rows[row][col];
            }
        }
    }
}

void image_draw_rect(Image *img, uint32_t x, uint32_t y, uint32_t width, uint32_t height, Pixel color, bool filled)
{
    for (int row = y; row < y + height; row++)
    {
        for (int col = x; col < x + width; col++)
        {
            if (filled || (!filled && ((row == y || row == y + height - 1) || (col == x || col == x + width - 1))))
            {
                img->rows[row][col] = color;
            }
        }
    }
}

void image_safe_set_pixel(Image *img, int32_t x, int32_t y, Pixel color)
{
    if (x >= 0 && x < img->width && y >= 0 && y <= img->height)
    {
        img->rows[y][x] = color;
    }
}

void image_set_pixel(Image *img, uint32_t x, uint32_t y, Pixel color)
{
    img->rows[y][x] = color;
}

void image_fill(Image *img, Pixel color)
{
    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            img->rows[i][j] = color;
        }
    }
}

void image_save_to_bmp(Image *img, const char *file_path)
{
    // Header
    int32_t width = img->width;
    int32_t height = img->height;
    uint16_t bitcount = 24;
    int width_in_bytes = ((width * bitcount + 31) / 32) * 4;
    uint32_t imagesize = width_in_bytes * height;
    unsigned char *buf = malloc(imagesize);
    // for (int row = img->height - 1; row >= 0; row--)
    for (uint32_t row = 0; row < img->height; row++)
    {
        for (uint32_t col = 0; col < img->width; col++)
        {
            // Invert only reading, not writing : we read the bottom first (higher y/line) and write it first
            Pixel pixel = img->rows[img->height - 1 - row][col];
            // printf("x=col=%u, y=lin=%u (%u, %u, %u)\n", col, row, pixel.r, pixel.g, pixel.b);
            buf[row * width_in_bytes + col * 3 + 0] = pixel.b; // blue
            buf[row * width_in_bytes + col * 3 + 1] = pixel.g; // green
            buf[row * width_in_bytes + col * 3 + 2] = pixel.r; // red
        }
    }
    const uint32_t biSize = 40;
    const uint32_t bfOffBits = 54;
    uint32_t filesize = 54 + imagesize;
    const uint16_t biPlanes = 1;
    unsigned char header[54] = {0};
    memcpy(header, "BM", 2);
    memcpy(header + 2, &filesize, 4);
    memcpy(header + 10, &bfOffBits, 4);
    memcpy(header + 14, &biSize, 4);
    memcpy(header + 18, &width, 4);
    memcpy(header + 22, &height, 4);
    memcpy(header + 26, &biPlanes, 2);
    memcpy(header + 28, &bitcount, 2);
    memcpy(header + 34, &imagesize, 4);
    // Write
    FILE *f = fopen(file_path, "wb");
    if (f == NULL)
    {
        printf("ERROR: Unable to open file %s for writing.\n", file_path);
        return;
    }
    fwrite(header, 1, 54, f);
    fwrite((char *)buf, 1, imagesize, f);
    fclose(f);
    free(buf);
    printf("Saved at : %s\n", file_path);
}

void image_free(Image *img)
{
    for (int i = 0; i < img->height; i++)
    {
        free(img->rows[i]);
    }
    free(img->rows);
}
