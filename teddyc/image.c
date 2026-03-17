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
const Pixel ORANGE = {.r = 255, .g = 165, .b = 0};

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
        image_draw_line(dest, x, y, x + 5, y, color);         // down
        image_draw_line(dest, x, y, x, y - 7, color);         // left
        image_draw_line(dest, x, y - 7, x + 5, y - 7, color); // up
        image_draw_line(dest, x + 5, y, x + 5, y - 7, color); // right
        image_draw_line(dest, x, y, x + 5, y - 7, color);     // diag
    }
    else if (digit == 1)
    {
        image_draw_line(dest, x, y, x + 5, y, color);         // base
        image_draw_line(dest, x + 3, y, x + 3, y - 7, color); // hamp
        image_draw_line(dest, x + 3, y - 7, x, y - 3, color); // diag
    }
}

void image_draw_line(Image *dest, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, Pixel color)
{
    // printf("x1=%u y1=%u x2=%u y2=%u\n", x1, y1, x2, y2);
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
        uint32_t diff_x = (x1 < x2) ? x2 - x1 : x1 - x2;
        uint32_t diff_y = (y1 < y2) ? y2 - y1 : y1 - y2;
        uint32_t count = 0;
        if (diff_x > diff_y)
        {
            uint32_t mod_x = (x1 < x2) ? 1 : -1;
            double y = y1;
            double factor = (y1 < y2) ? 1 : -1;
            double mod_y = ((double)diff_y / (double)diff_x) * factor;
            for (uint32_t x = x1; count <= diff_x; x += mod_x, count++)
            {
                if (x >= 0 && x < dest->width && y >= 0 && y < dest->height)
                {
                    // printf("%u %f (as %u) %f\n", x, y, (uint32_t) round(y), mod_y);
                    dest->rows[(uint32_t)round(y)][x] = color;
                    y += mod_y;
                }
            }
        }
        else
        {
            uint32_t mod_y = (y1 < y2) ? 1 : -1;
            double x = x1;
            double factor = (x1 < x2) ? 1 : -1;
            double mod_x = ((double)diff_x / (double)diff_y) * factor;
            for (uint32_t y = y1; count <= diff_y; y += mod_y, count++)
            {
                if (x >= 0 && x < dest->width && y >= 0 && y < dest->height)
                {
                    // printf("%f (as %u) %u %f\n", x, (uint32_t) round(x), y, mod_x);
                    dest->rows[y][(uint32_t)round(x)] = color;
                    x += mod_x;
                }
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
    // Writing file
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

// from https://www.w3.org/TR/png/#D-CRCAppendix
uint32_t crc32(unsigned char * buffer, uint32_t length)
{
    // Init table
    uint32_t c, crc_table[256];
    uint16_t n, k;

    for (n = 0; n < 256; n++)
    {
        c = (uint32_t)n;
        for (k = 0; k < 8; k++)
        {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    // Compute
    c = 0xffffffffu;
    for (n = 0; n < length; n++) {
        c = crc_table[(c ^ buffer[n]) & 0xff] ^ (c >> 8);
    }
    printf("crc32 = %u\n", c ^ 0xffffffffu);
    return c ^ 0xffffffffu;
}

void image_save_to_png(Image *img, const char *file_path)
{
    // PNG signature
    uint8_t signature[8] = {0};
    signature[0] = 0x89;
    signature[1] = 0x50; // P in ASCII
    signature[2] = 0x4E; // N in ASCII
    signature[3] = 0x47; // G in ASCII
    signature[4] = 0x0D; // CR
    signature[5] = 0x0A; // LF (CRLF = DOS line ending)
    signature[6] = 0x1A; // DOS end of file
    signature[7] = 0x0A; // LF (UNIX line ending)
    printf("Signature OK\n");
    // IHDR = image header
    // IHDR - LENGTH (4)
    uint32_t length = 13;
    uint32_t swapped_length = _byteswap_ulong(length); // 00 00 00 0D;
    // IHDR - CHUNK TYPE (4)
    uint8_t type[4] = {'I', 'H', 'D', 'R'}; // 49 48 44 52
    uint32_t width = _byteswap_ulong(1);
    uint32_t height = _byteswap_ulong(1);
    uint8_t bits_per_channel = 8;
    uint8_t color_type = 2;
    uint8_t compression_method = 0;
    uint8_t filter_method = 0;
    uint8_t interlaced = 0;
    // Writing to buffer
    uint32_t idhr_length = 4 + 4 + length + 4;
    uint8_t idhr[4 + 4 + 13 + 4] = {0};
    memcpy(idhr, &swapped_length, 4);
    memcpy(idhr + 4, &type, 4);
    memcpy(idhr + 8, &width, 4);
    memcpy(idhr + 12, &height, 4);
    memcpy(idhr + 16, &bits_per_channel, 1);
    memcpy(idhr + 17, &color_type, 1);
    memcpy(idhr + 18, &compression_method, 1);
    memcpy(idhr + 19, &filter_method, 1);
    memcpy(idhr + 20, &interlaced, 1);
    // IHDR - CRC (4) on type & content but not length
    //uint32_t crc = _byteswap_ulong(0x907753DE);
    uint32_t crc = _byteswap_ulong(crc32(idhr + 4, length + 4)); // ignore length, add chunck type length
    memcpy(idhr + 21, &crc, 4);
    printf("IHDR OK\n");
    // IDAT
    // IDAT - LENGTH (4)
    length = 12;
    swapped_length = _byteswap_ulong(length); // 00 00 00 0C
    // IDAT - CHUNK TYPE (4)
    type[0] = 'I'; // 49
    type[1] = 'D'; // 44
    type[2] = 'A'; // 41
    type[3] = 'T'; // 54
    // IDAT - DATA (12)
    compression_method = 0x08;
    uint8_t *data = malloc(sizeof(uint8_t) * 6);
    data[0] = 0x63;
    data[1] = 0xF8;
    data[2] = 0xCF;
    data[3] = 0xC0;
    data[4] = 0x00;
    data[5] = 0x00;
    uint8_t fcheck = 0xD7;
    uint32_t adler32 = _byteswap_ulong(0x03010100);
    // Writing to buffer
    uint32_t idat_length = 4 + 4 + length + 4;
    uint8_t idat[4 + 4 + 12 + 4] = {0};
    memcpy(idat, &length, 4);
    memcpy(idat + 4, &type, 4);
    memcpy(idat + 8, &compression_method, 1);
    memcpy(idat + 9, &fcheck, 1);
    memcpy(idat + 10, data, 6);
    memcpy(idat + 16, &adler32, 4);
    // IDAT - CRC (4) on type & content but not length
    //crc = _byteswap_ulong(0x18DD8DB0);
    crc = _byteswap_ulong(crc32(idat + 4, length + 4)); /// ignore length, add chunck type length
    memcpy(idat + 20, &crc, 4);
    printf("IDAT OK\n");
    // IEND
    // IEND - LENGTH (4)
    length = 0;
    swapped_length = _byteswap_ulong(length);
    // IEND - CHUNK TYPE (4)
    type[0] = 'I'; // 49
    type[1] = 'E'; // 45
    type[2] = 'N'; // 4E
    type[3] = 'D'; // 44
    // Writing to buffer
    uint32_t iend_length = 4 + 4 + length + 4;
    uint8_t iend[4 + 4 + 0 + 4] = {0};
    memcpy(iend, &swapped_length, 4);
    memcpy(iend + 4, &type, 4);
    // IEND - CRC (4) on type & content but not length
    // crc = _byteswap_ulong(0xAE426082);
    crc = _byteswap_ulong(crc32(iend + 4, length + 4)); // ignore length, add chunck type length
    memcpy(iend + 8, &crc, 4);
    printf("IDEND OK\n");
    // Cleaning
    free(data);
    // Writing file
    FILE *f = fopen(file_path, "wb");
    if (f == NULL)
    {
        printf("ERROR: Unable to open file %s for writing.\n", file_path);
        return;
    }
    fwrite(signature, 1, 8, f);
    fwrite(idhr, 1, idhr_length, f);
    fwrite(idat, 1, idat_length, f);
    fwrite(iend, 1, iend_length, f);
    fclose(f);
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

uint32_t image_get_size(Image *img)
{
    return img->width * img->width * sizeof(Pixel);
}
