#include "assets.h"

uint8_t palette[256][3] = {
    {0, 0, 0}, {0, 0, 168}, {0, 168, 0}, {0, 168, 168}, {168, 0, 0},
    {168, 0,168}, {168, 84, 0}, {168, 168, 168}, {84, 84, 84},
    {84, 84, 252}, {84, 252, 84}, {84, 252, 252}, {252, 84, 84},
    {252, 84, 252}, {252, 252, 84}, {252, 252, 252}, {236, 236, 236},
    {220, 220, 220}, {208, 208, 208}, {192, 192, 192}, {180, 180, 180},
    {168, 168, 168}, {152, 152, 152}, {140, 140, 140}, {124, 124, 124},
    {112, 112, 112}, {100, 100, 100}, {84, 84, 84}, {72, 72, 72}, {56, 56, 56},
    {44, 44, 44}, {32, 32, 32}, {252, 0, 0}, {236, 0, 0}, {224, 0, 0},
    {212, 0, 0}, {200, 0, 0}, {188, 0, 0}, {176, 0, 0}, {164, 0, 0},
    {152, 0, 0}, {136, 0, 0}, {124, 0, 0}, {112, 0, 0}, {100, 0, 0},
    {88, 0, 0}, {76, 0, 0}, {64, 0, 0}, {252, 216, 216}, {252, 184, 184},
    {252, 156, 156}, {252, 124, 124}, {252, 92, 92}, {252, 64, 64},
    {252, 32, 32}, {252, 0, 0}, {252, 168, 92}, {252, 152, 64}, {252, 136, 32},
    {252, 120, 0}, {228, 108, 0}, {204, 96, 0}, {180, 84, 0}, {156, 76, 0},
    {252, 252, 216}, {252, 252, 184}, {252, 252, 156}, {252, 252, 124},
    {252, 248, 92}, {252, 244, 64}, {252, 244, 32}, {252, 244, 0},
    {228, 216, 0}, {204, 196, 0}, {180, 172, 0}, {156, 156, 0},
    {132, 132, 0}, {112, 108, 0}, {88, 84, 0}, {64, 64, 0}, {208, 252, 92},
    {196, 252, 64}, {180, 252, 32}, {160, 252, 0}, {144, 228, 0},
    {128, 204, 0}, {116, 180, 0}, {96, 156, 0}, {216, 252, 216},
    {188, 252, 184}, {156, 252, 156}, {128, 252, 124}, {96, 252, 92},
    {64, 252, 64}, {32, 252, 32}, {0, 252, 0}, {0, 252, 0}, {0, 236, 0},
    {0, 224, 0}, {0, 212, 0}, {4, 200, 0}, {4, 188, 0}, {4, 176, 0},
    {4, 164, 0}, {4, 152, 0}, {4, 136, 0}, {4, 124, 0}, {4, 112, 0},
    {4, 100, 0}, {4, 88, 0}, {4, 76, 0}, {4, 64, 0}, {216, 252, 252},
    {184, 252, 252}, {156, 252, 252}, {124, 252, 248}, {92, 252, 252},
    {64, 252, 252}, {32, 252, 252}, {0, 252, 252}, {0, 228, 228},
    {0, 204, 204}, {0, 180, 180}, {0, 156, 156}, {0, 132, 132}, {0, 112, 112},
    {0, 88, 88}, {0, 64, 64}, {92, 188, 252}, {64, 176, 252}, {32, 168, 252},
    {0, 156, 252}, {0, 140, 228}, {0, 124, 204}, {0, 108, 180}, {0, 92, 156},
    {216, 216, 252}, {184, 188, 252}, {156, 156, 252}, {124, 128, 252},
    {92, 96, 252}, {64, 64, 252}, {32, 36, 252}, {0, 4, 252}, {0, 0, 252},
    {0, 0, 236}, {0, 0, 224}, {0, 0, 212}, {0, 0, 200}, {0, 0, 188},
    {0, 0, 176}, {0, 0, 164}, {0, 0, 152}, {0, 0, 136}, {0, 0, 124},
    {0, 0, 112}, {0, 0, 100}, {0, 0, 88}, {0, 0, 76}, {0, 0, 64},
    {40, 40, 40}, {252, 224, 52}, {252, 212, 36}, {252, 204, 24},
    {252, 192, 8}, {252, 180, 0}, {180, 32, 252}, {168, 0, 252},
    {152, 0, 228}, {128, 0, 204}, {116, 0, 180}, {96, 0, 156},
    {80, 0, 132}, {68, 0, 112}, {52, 0, 88}, {40, 0, 64}, {252, 216, 252},
    {252, 184, 252}, {252, 156, 252}, {252, 124, 252}, {252, 92, 252},
    {252, 64, 252}, {252, 32, 252}, {252, 0, 252}, {224, 0, 228},
    {200, 0, 204}, {180, 0, 180}, {156, 0, 156}, {132, 0, 132},
    {108, 0, 112}, {88, 0, 88}, {64, 0, 64}, {252, 232, 220},
    {252, 224, 208}, {252, 216, 196}, {252, 212, 188}, {252, 204, 176},
    {252, 196, 164}, {252, 188, 156}, {252, 184, 144}, {252, 176, 128},
    {252, 164, 112}, {252, 156, 96}, {240, 148, 92}, {232, 140, 88},
    {220, 136, 84}, {208, 128, 80}, {200, 124, 76}, {188, 120, 72},
    {180, 112, 68}, {168, 104, 64}, {160, 100, 60}, {156, 96, 56},
    {144, 92, 52}, {136, 88, 48}, {128, 80, 44}, {116, 76, 40},
    {108, 72, 36}, {92, 64, 32}, {84, 60, 28}, {72, 56, 24}, {64, 48, 24},
    {56, 44, 20}, {40, 32, 12}, {96, 0, 100}, {0, 100, 100}, {0, 96, 96},
    {0, 0, 28}, {0, 0, 44}, {48, 36, 16}, {72, 0, 72}, {80, 0, 80},
    {0, 0, 52}, {28, 28, 28}, {76, 76, 76}, {92, 92, 92}, {64, 64, 64},
    {48, 48, 48}, {52, 52, 52}, {216, 244, 244}, {184, 232, 232},
    {156, 220, 220}, {116, 200, 200}, {72, 192, 192}, {32, 180, 180},
    {32, 176, 176}, {0, 164, 164}, {0, 152, 152}, {0, 140, 140}, {0, 132, 132},
    {0, 124, 124}, {0, 120, 120}, {0, 116, 116}, {0, 112, 112}, {0, 108, 108},
    {152, 0, 136}};

bool read_asset_header(const Data mem, AssetHeader * header)
{
    header->number = read_uint16(mem, 0); // 538
    header->first_sprite = read_uint16(mem, 2);
    header->first_sound = read_uint16(mem, 4);
    header->pointers = malloc(sizeof(uint32_t) * header->number);
    header->lengths = malloc(sizeof(uint16_t) * header->number);
    for (uint32_t i = 0; i < header->number; i++)
    {
        uint32_t at = 6 + i * 4; // last is 2154, so first length should be 2154+4 = 2158
        header->pointers[i] = read_uint32(mem, at);
        uint32_t length_at = 6 + i * 2 + header->number * 4;
        header->lengths[i] = read_uint16(mem, length_at);
        //printf("%u. %u [%u] @%u\n", at, header->pointers[i], header->lengths[i], length_at);
    }
    return true;
}

Image * wall_to_image(const Data assetDataFile, AssetHeader assetHeader, uint16_t index)
{
    uint32_t offset = assetHeader.pointers[index];
    uint32_t length = assetHeader.lengths[index];
    if (length != 4096)
    {
        return NULL;
    }
    int32_t width = 64;
    int32_t height = 64;
    Image * img = image_new(width, height);
    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        uint8_t pixel = assetDataFile.data[offset + i]; // rotation horizontale i + 64 * y ---
        Pixel p = {.r=palette[pixel][0], .g =palette[pixel][1], .b=palette[pixel][2]};
        image_set_pixel(img, y, x, p);
        x += 1;
        if (x == 64)
        {
            x = 0;
            y += 1;
        }
    }
    return img;
}

Image * sprite_to_image(const Data assetDataFile, AssetHeader assetHeader, uint16_t index)
{
//#define DEBUG
#ifdef DEBUG
    printf("-- Sprite to image --------------------------\n");
#endif
    uint32_t sprite_offset = assetHeader.pointers[index];
    uint32_t sprite_length = assetHeader.lengths[index];
#ifdef DEBUG
    printf("index = %u, offset = %u, length = %u\n", index, sprite_offset, sprite_length);
#endif
    if (sprite_offset == 0 || sprite_length == 0)
    {
        return NULL;
    }
    int32_t width = 64;
    int32_t height = 64;
    Image * img = image_new(width, height);
    image_fill(img, MAGENTA);

    uint16_t first_column = read_uint16(assetDataFile, sprite_offset);
    uint16_t last_column = read_uint16(assetDataFile, sprite_offset + 2);
    uint16_t nb_columns = last_column - first_column + 1;
    uint16_t * column_first_post_offset = malloc(sizeof(uint16_t) * sprite_length); // beginning of the first post of each column described
    for (uint16_t i = 0; i < nb_columns; i++)
    {
        column_first_post_offset[i] = read_uint16(assetDataFile, sprite_offset + 4 + i * 2);
    }
    uint32_t pixel_pool_offset = 4 + nb_columns * 2; // + offset
    uint32_t pixel_pool_length = column_first_post_offset[0] - pixel_pool_offset;
    uint32_t post_offset = column_first_post_offset[0];
    uint32_t pixel_count = 0;

#ifdef DEBUG
    printf("#%u col1 = %u, col2 = %u, length = %u pixels, pixel pool offset = %u, length = %u, post offset = %u\n", index, first_column,
        last_column, nb_columns, pixel_pool_offset, pixel_pool_length, post_offset);
#endif

    // Iterating the columns
    for (uint16_t icol = 0; icol < nb_columns; icol++)
    {
        uint32_t post_start = column_first_post_offset[icol];
        uint16_t post_count = 0;
        // Iterating the posts
#ifdef DEBUG
        printf("Col %2u/%2u(+%2u) posts : ## | start |  end | next (@%u)\n", icol, nb_columns, first_column, column_first_post_offset[icol] + post_count * 6);
#endif
        while (pixel_count < pixel_pool_length)
        {
            uint32_t offset = sprite_offset  + column_first_post_offset[icol] + post_count * 6;
            uint16_t post_end = read_uint16(assetDataFile, offset) / 2;
            uint16_t post_info2 = read_uint16(assetDataFile, offset + 2);
            uint16_t post_start = read_uint16(assetDataFile, offset + 4) / 2;
            uint16_t post_next = read_uint16(assetDataFile, offset + 6);
            if (post_end == 0) // nothing in that column
            {
                break;
            }
#ifdef DEBUG
            if (post_next == 0) {
                printf("                       %02u |  %4u | %4u | %4u (end)\n", post_count, post_start, post_end, post_next);
            } else {
                printf("                       %02u |  %4u | %4u | %4u\n", post_count, post_start, post_end, post_next);
            }
#endif
            //printf("col:%u(+%u) post:%u: chunkoffset:%u start(1): %u (2): %u end(3): %u next(4): %u\n",
            //    icol, first_column, post_count, chunk_offset[icol], post_start, post_info2, post_end, post_next);
            for (uint16_t irow = 0; irow < post_end - post_start; irow++)
            {
                uint8_t pixel = assetDataFile.data[sprite_offset + pixel_pool_offset + pixel_count];
#ifdef DEBUG
                printf("                                      pixel %u / %u = %u\n", pixel_count, pixel_pool_length, pixel);
#endif
                Pixel p = {.r=palette[pixel][0], .g =palette[pixel][1], .b=palette[pixel][2]};
                image_set_pixel(img, icol + first_column, irow + post_start, p);
                pixel_count += 1;
            }
            post_count += 1;
            if (post_next == 0)
            {
                break;
            }
        }
    }
    return img;
}
