#include "levels.h"

void display_header(const Header * header)
{
    printf("------------------------------------------------------------\n");
    printf("Magic number (ABCD) : %d (%X)\n", header->magic_word, header->magic_word);
    printf("Number of levels    : %d\n", header->number);
    for (uint8_t i = 0 ; i < header->number; i++)
    {
        printf("    %03d. %06u %x\n", i + 1, header->level_header_ptr[i], header->level_header_ptr[i]);
    }
    printf("------------------------------------------------------------\n");
}

bool read_header(const Data mem, Header * header)
{
    if (header == NULL)
    {
        header = malloc(sizeof(Header));
        if (header == NULL)
        {
            printf("ERROR: Impossible to allocate a header structure.\n");
            return false;
        }
    }
    // 1 byte = 2**8 = 256. 1 char hexa = 2**4 = 16.
    // Donc Il faut 2 chars hexa pour décrire un byte = 8 bits = 1 char.
    header->magic_word = read_uint16(mem, 0);
    header->number = 0;
    for (uint8_t i = 0; i < 100; i++) {
        int32_t ptr = read_uint32(mem, 2 + i * 4);
        if (ptr >= 0)
        {
            header->level_header_ptr[i] = (uint32_t) ptr;
            header->number++;
        }
        else
        {
            break;
        }
    }
    return true;
}

bool read_level_header(const Data mem, uint32_t offset, LevelHeader * lh)
{
    if (lh == NULL)
    {
        lh = malloc(sizeof(LevelHeader));
        if (lh == NULL)
        {
            printf("ERROR: Impossible to allocate a level header structure.\n");
            return false;
        }
    }
    lh->offsetPlane0 = read_int32(mem, offset + 0);
    lh->offsetPlane1 = read_int32(mem, offset + 4);
    lh->offsetPlane2 = read_int32(mem, offset + 8);
    if (lh->offsetPlane0 >= 0) {
        lh->lenghtPlane0 = read_uint16(mem, offset + 12);
    }
    if (lh->offsetPlane1 >= 0) {
        lh->lenghtPlane1 = read_uint16(mem, offset + 14);
    }
    if (lh->offsetPlane2 >= 0) {
        lh->lenghtPlane2 = read_uint16(mem, offset + 16);
    }
    lh->width = read_uint16(mem, offset + 18);
    lh->height = read_uint16(mem, offset + 20);
    int i = 0;
    while ((char) mem.data[offset + 22 + i] != '\0')
    {
        lh->name[i] = (char) mem.data[offset + 22 + i];
        i++;
    }
    lh->name[i] = '\0';
    return true;
}

void display_level_header(const LevelHeader * lh)
{
    printf("------------------------------------------------------------\n");
    printf("Level name         : %s\n", lh->name);
    printf("Width x Height     : %d x %d\n", lh->width, lh->height);
    printf("Offset for plane 0 : %06d %x\n", lh->offsetPlane0, lh->offsetPlane0);
    printf("Length of plane 0  : %d\n", lh->lenghtPlane0);
    printf("Offset for plane 1 : %06d %x\n", lh->offsetPlane1, lh->offsetPlane1);
    printf("Length of plane 1  : %d\n", lh->lenghtPlane1);
    printf("Offset for plane 2 : %06d %x\n", lh->offsetPlane2, lh->offsetPlane2);
    printf("Length of plane 2  : %d\n", lh->lenghtPlane2);
    printf("------------------------------------------------------------\n");
}

bool uncompress(const Data mem, uint32_t offset, uint16_t length, uint16_t * output)
{
    printf("Compressed data length   = %u\n", length);
    uint16_t size = read_uint16(mem, offset);
    printf("Uncompressed data length = %u\n", size);
    printf("Previsionned data length = %d\n", 64*64*2); // 2 bytes by case
    printf("Compressed ratio         = %d%%\n", (int) round(((double) length / size) * 100));
    printf("Starting offset          = %u\n", offset);
    uint32_t tile_index = 0;
    uint32_t offset_read = offset + 2; // in bytes, +2 for size
    while (offset_read < offset + length)
    {
        uint16_t elem = read_uint16(mem, offset_read);
        if (elem != 0xABCD)
        {
            if (tile_index < 1000)
                printf("@%u/%u Uncompressed data : %u (%x)\n", offset_read, length, elem, elem);
            output[tile_index] = elem;
            tile_index += 1;
            offset_read += 2;
        }
        else
        {
            uint16_t num = read_uint16(mem, offset_read + 2);
            uint16_t val = read_uint16(mem, offset_read + 4);
            if (tile_index < 1000)
                printf("@%u/%u Compressed data detected : repeat %u times the value %u\n", offset_read, length, num, val);
            for (uint16_t i = 0; i < num; i++)
            {
                output[tile_index] = val;
                tile_index += 1;
            }
            //tile_index += num;
            offset_read += 6; // magic number, num, val
        }
    }
    return true;
}

bool read_plane(const LevelHeader * lhs, uint8_t level, uint8_t plane, const Data mem, uint16_t * output)
{
    uint32_t offset;
    uint16_t length;
    if (plane == 0)
    {
        offset = lhs[level].offsetPlane0;
        length = lhs[level].lenghtPlane0;
    }
    else if (plane == 1)
    {
        offset = lhs[level].offsetPlane1;
        length = lhs[level].lenghtPlane1;
    }
    else
    {
        printf("[ERROR] Plane unknown: %u\n", plane);
        return false;
    }
    return uncompress(mem, offset, length, output);
}

bool export_plane_to_txt_old(const LevelHeader * level_headers, uint8_t level, uint8_t plane)
{
    char file_name[64];
    sprintf(file_name, "level%uplane%u.txt", level, plane);
    FILE *f = fopen(file_name, "w");
    if (f == NULL)
    {
        printf("ERROR: Unable to open file %s for writing.\n", file_name);
        return false;
    }
    uint16_t * source = NULL;
    if (plane == 0)
    {
        source = level_headers[level].dataPlane0;
    }
    else if (plane == 1)
    {
        source = level_headers[level].dataPlane1;
    }
    else
    {
        printf("[ERROR] Plane unknown: %u\n", plane);
        return false;
    }
    for (uint16_t line = 0 ; line < level_headers[level].width ; line++)
    {
        for (uint16_t col = 0 ; col < level_headers[level].height ; col++)
        {
            printf("%3u ", source[line * 64 + col]);
            fprintf(f, "%3u ", source[line * 64 + col]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    return true;
}

bool export_plane_to_txt(Level lvl, uint8_t plane)
{
    char file_name[64];
    sprintf(file_name, "level%uplane%u.txt", lvl.number, plane);
    FILE *f = fopen(file_name, "w");
    if (f == NULL)
    {
        printf("ERROR: Unable to open file %s for writing.\n", file_name);
        return false;
    }
    for (uint16_t line = 0 ; line < lvl.width ; line++)
    {
        for (uint16_t col = 0 ; col < lvl.height ; col++)
        {
            printf("%3u ", lvl.plane[plane][line][col]);
            fprintf(f, "%3u ", lvl.plane[plane][line][col]);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    printf("\n");
    return true;
}

bool read_level_data(const Data levelDataFile, LevelHeader * level_headers, uint8_t level)
{
    // Read level data
    // Plane 0
    level_headers[level].dataPlane0 = malloc(sizeof(uint16_t) * (level_headers[level].width * level_headers[level].height));
    if (level_headers[level].dataPlane0 == NULL)
    {
        printf("[ERROR] Impossible to allocate for a new plane\n");
        return false;
    }
    read_plane(level_headers, level, 0, levelDataFile, level_headers[level].dataPlane0);
    // Plane 1
    level_headers[level].dataPlane1 = malloc(sizeof(uint16_t) * (level_headers[level].width * level_headers[level].height));
    if (level_headers[level].dataPlane1 == NULL)
    {
        printf("[ERROR] Impossible to allocate for a new plane\n");
        return false;
    }
    read_plane(level_headers, level, 1, levelDataFile, level_headers[level].dataPlane1);
    return true;
}

Level create_level_from_files(const Data levelDataFile, LevelHeader * level_headers, uint8_t level)
{
    Level lvl = create_level(level, level_headers[level].width, level_headers[level].height, 2);
    for (uint8_t planeNumber = 0 ; planeNumber < 2 ; planeNumber++)
    {
        uint16_t * source = NULL;
        switch(planeNumber)
        {
            case 0:
                source = level_headers[level].dataPlane0;
                break;
            case 1:
                source = level_headers[level].dataPlane1;
                break;
        }
        for (uint16_t line = 0 ; line < level_headers[level].width ; line++)
        {
            for (uint16_t col = 0 ; col < level_headers[level].height ; col++)
            {
                lvl.plane[planeNumber][line][col] = source[line * 64 + col];
            }
        }
    }
    return lvl;
}

Level create_level(uint8_t number, uint16_t width, uint16_t height, uint8_t planeNumber)
{
    Level lvl = {.number = number, .width = width, .height = height, .planeNumber = planeNumber};
    lvl.plane = malloc(sizeof(uint16_t *) * planeNumber);
    for (uint8_t planeIndex = 0 ; planeIndex < planeNumber ; planeIndex++)
    {
        lvl.plane[planeIndex] = malloc(sizeof(uint16_t *) * height);
        for (uint8_t line = 0 ; line < height ; line++)
        {
            lvl.plane[planeIndex][line] = malloc(sizeof(uint16_t *) * width);
            for (uint8_t column = 0 ; column < width ; column++)
            {
                lvl.plane[planeIndex][line][column] = 0;
            }
        }
    }
    return lvl;
}

Image * level_to_image(Level lvl, uint8_t plane, Image * textures[], Image * sprites[])
{
    Image * img = image_new(4096, 4096); // 64*64

    // We go through the plane of the level
    for (uint16_t line = 0 ; line < lvl.width ; line++)
    {
        for (uint16_t col = 0 ; col < lvl.height ; col++)
        {
            uint16_t raw_tile = lvl.plane[0][line][col];
            // La plupart des textures sont stockées sous deux formes :
            // claire ou sombre
            // C'est pour ça que pour la chercher dans notre base de textures
            // il faut multiplier la valeur du tile par 2 !
            // Le -1 c'est parce que 1 = 0.
            uint16_t tile = (raw_tile - 1) * 2;
            if (raw_tile <= 64 || raw_tile == 90 || raw_tile == 91 || raw_tile == 92 || raw_tile == 93 || raw_tile == 100)
            {
                if (raw_tile == 90 || raw_tile == 91)
                {
                    tile = 56; // Green door
                }
                else if (raw_tile == 92 || raw_tile == 93)
                {
                    tile = 62; // Grey door
                }
                else if (raw_tile == 100)
                {
                    tile = 60; // Elevator door
                }
                printf("at=(%d, %d) tile = %u, raw_tile = %u, ptr = %p\n", col*64, line*64, tile, raw_tile, textures[tile]);
                if (textures[tile] != NULL)
                {
                    //if (line > 0) return img; // DEBUG
                    image_draw_image(img, col * 64, line * 64, textures[tile]);
                }
            }
            else if (raw_tile >= 106 && raw_tile <= 143)
            {
                image_draw_rect(img, col * 64, line * 64, 64, 64, GREY, true);
                image_draw_rect(img, col * 64, line * 64, 64, 64, NEAR_WHITE, false);
            }
            // Magical number... don't ask why
            uint16_t raw_sprite = lvl.plane[1][line][col];
            // 65 -42 = 23 et 104 -42 = 62
            if (raw_sprite >= 23 && raw_sprite <= 62 && sprites[raw_sprite + 42 - 64] != NULL)
            {
                // remove the first 64 walls with -64
                image_draw_image(img, col * 64, line * 64, sprites[raw_sprite + 42 - 64]);
            }
            else if (raw_sprite == 98)
            {
                image_draw_rect(img, col * 64, line * 64, 64, 64, GREEN, false);
            }
        }
    }
    return img;
}
