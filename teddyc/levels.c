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
    printf("%-16s : %ux%u | %06d %4x [%4u] | %06d %4x [%4u] | %06d %4x [%4u] |\n",
        lh->name,
        lh->width, lh->height,
        lh->offsetPlane0, lh->offsetPlane0, lh->lenghtPlane0,
        lh->offsetPlane1, lh->offsetPlane1, lh->lenghtPlane1,
        lh->offsetPlane2, lh->offsetPlane2, lh->lenghtPlane2
    );
}

void display_level_header2(const LevelHeader * lh)
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
    uint16_t size = read_uint16(mem, offset);
#ifdef DEBUG
    printf("Compressed data length   = %u\n", length);
    printf("Uncompressed data length = %u\n", size);
    printf("Previsionned data length = %d\n", 64*64*2); // 2 bytes by case
    printf("Compressed ratio         = %d%%\n", (int) round(((double) length / size) * 100));
    printf("Starting offset          = %u\n", offset);
#endif
    uint32_t tile_index = 0;
    uint32_t offset_read = offset + 2; // in bytes, +2 for size
    while (offset_read < offset + length)
    {
        uint16_t elem = read_uint16(mem, offset_read);
        if (elem != 0xABCD)
        {
            if (tile_index < 1000)
            {
                #ifdef DEBUG
                printf("@%u/%u Uncompressed data : %u (%x)\n", offset_read, length, elem, elem);
                #endif
            }
            output[tile_index] = elem;
            tile_index += 1;
            offset_read += 2;
        }
        else
        {
            uint16_t num = read_uint16(mem, offset_read + 2);
            uint16_t val = read_uint16(mem, offset_read + 4);
            if (tile_index < 1000)
            {
                #ifdef DEBUG
                printf("@%u/%u Compressed data detected : repeat %u times the value %u\n", offset_read, length, num, val);
                #endif
            }
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

void level_info(Level lvl)
{
    uint32_t static_objects = 0;
    uint32_t tile_objects = 0;
    uint32_t doors =  0;
    uint32_t elevators = 0;
    uint32_t secrets = 0;
    uint32_t baddies1 = 0;
    uint32_t baddies3 = 0;
    uint32_t baddies4 = 0;
    uint32_t bosses = 0;
    uint32_t starts = 0;
    uint32_t total = 0;
    for (uint16_t line = 0 ; line < lvl.width ; line++)
    {
        for (uint16_t col = 0 ; col < lvl.height ; col++)
        {
            uint8_t val0 = lvl.plane[0][line][col];
            if (val0 == 90 || val0 == 91)
            {
                doors += 1;
            }
            else if (val0 == 100 || val0 == 101)
            {
                elevators += 1;
            }
            uint8_t val1 = lvl.plane[1][line][col];
            if (val1 == 98)
            {
                secrets += 1;
            }
            else if (val1 >= 23 && val1 <= 62)
            {
                static_objects += 1;
            }
            else if (
                (val1 >= 108 && val1 <= 115) || // guard any standing or patrolling
                (val1 >= 138 && val1 <= 141))
            {
                baddies1 += 1;
            }
            else if (
                (val1 >= 144 && val1 <= 151) || // guard med standing or patrolling
                (val1 >= 174 && val1 <= 177))
            {
                baddies3 +=1 ;
            }
            else if (
                (val1 >= 180 && val1 <= 187) || // guard hard standing or patrolling
                (val1 >= 210 && val1 <= 213))
            {
                baddies4 += 1;
            }
            else if (val1 >= 19 && val1 <= 22)
            {
                starts += 1;
            }
        }
    }
    total = baddies1 + baddies3 + baddies4 + starts;
    printf("Information for level %u:\n", lvl.number);
    printf("Static Objects:       %u\n", static_objects); // 121
    printf("Tile Objects:         \n"); // 24 ???
    printf("Doors:                %u\n", doors); // 22 doors & elevators
    printf("Elevators:            %u\n", elevators);
    printf("Secrets:              %u\n", secrets); // 5 push wall
    printf("Level any enemies:    %u\n", baddies1); // 11
    printf("Level medium enemies: %u\n", baddies3); // 9
    printf("Level hard enemies:   %u\n", baddies4); // 17
    printf("Bosses:               \n"); // 0
    printf("Start position:       %u\n", starts); // 1 = others. Dead guard or start position ? A voir...
    printf("Total Actors:         %u\n", total); // 38
}

void level_stat(Level lvl, uint8_t plane, bool order_by_count)
{
    uint16_t counts[256]; // MAX 256 different values
    for (uint16_t i = 0 ; i < 256 ; i++)
    {
        counts[i] = 0;
    }
    uint16_t values = 0;
    for (uint16_t line = 0 ; line < lvl.width ; line++)
    {
        for (uint16_t col = 0 ; col < lvl.height ; col++)
        {
            uint8_t val = lvl.plane[plane][line][col];
            if (counts[val] == 0)
            {
                values += 1;
            }
            counts[val] += 1;
        }
    }
    printf("There are %u values:\n", values);
    if (!order_by_count)
    {
        for (uint16_t i = 0 ; i < 256 ; i++)
        {
            if (counts[i] != 0)
            {
                printf("%u : %u\n", i, counts[i]);
            }
        }
    } else {
        // Order
        uint16_t index_last_max = 0;
        uint16_t last_max = 0;
        for (uint16_t count = 0 ; count < values ; count++)
        {
            for (uint16_t i = 0 ; i < 256 ; i++)
            {
                if (counts[i] > last_max)
                {
                    last_max = counts[i];
                    index_last_max = i;
                }
            }
            printf("%u : %u\n", index_last_max, last_max);
            counts[index_last_max] = 0; // we delete step by step
            last_max = 0;
            count += 1;
        }
    }
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

bool wall_is_not_enclosed(Level lvl, uint16_t pline, uint16_t pcol)
{
    for (int16_t line = pline - 1; line < pline + 2; line++)
    {
        for (int16_t col = pcol - 1; col < pcol + 2; col++)
        {
            if (line >= 0 && line < lvl.height && col >= 0 && col < lvl.width && lvl.plane[0][line][col] > 64)
            {
                return true;
            }
        }
    }
    return false;
}

Image * level_to_image(Level lvl, uint8_t plane, Image * textures[], Image * sprites[], bool grid, bool thin_wall)
{
    Image * img = image_new(4096, 4096); // 64*64
    uint16_t unhandled_plane1 = 0;
    if (thin_wall)
    {
        image_fill(img, BLACK);
    }

    // We go through the plane of the level
    for (uint16_t line = 0 ; line < lvl.width ; line++)
    {
        for (uint16_t col = 0 ; col < lvl.height ; col++)
        {
            // -- Plane 0 --
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
                    tile = 56; // Steel door
                }
                else if (raw_tile == 92 || raw_tile == 93)
                {
                    tile = 62; // Grey door
                }
                else if (raw_tile == 100)
                {
                    tile = 60; // Elevator door
                }
                //printf("at=(%d, %d) tile = %u, raw_tile = %u, ptr = %p\n", col*64, line*64, tile, raw_tile, textures[tile]);
                if (textures[tile] != NULL)
                {
                    if (thin_wall && wall_is_not_enclosed(lvl, line, col))
                    {
                        image_draw_image(img, col * 64, line * 64, textures[tile]);
                    }
                }
            }
            else if (raw_tile >= 106 && raw_tile <= 143)
            {
                image_draw_rect(img, col * 64, line * 64, 64, 64, GREY, true);
                if (grid)
                {
                    image_draw_rect(img, col * 64, line * 64, 64, 64, NEAR_WHITE, false);
                }
            } else {
                printf("Plane 0 unhandled value : %u at %u,%u \n", raw_tile, line, col);
            }
            // -- Plane 1 --
            // Magical number... don't ask why
            uint16_t raw_sprite = lvl.plane[1][line][col];
            // 65 -42 = 23 et 104 -42 = 62
            uint32_t sprite = 0;
            // -- Depart point ----------------------------------------------------------
            if (raw_sprite == 19) // Depart north
            {
                image_draw_arrow(img, col * 64, line * 64, 64, NORTH, GREEN);
            } else if (raw_sprite == 20) { // Depart east
                image_draw_arrow(img, col * 64, line * 64, 64, EAST, GREEN);
            } else if (raw_sprite == 21) { // Depart south
                image_draw_arrow(img, col * 64, line * 64, 64, SOUTH, GREEN);
            } else if (raw_sprite == 22) { // Depart west
                image_draw_arrow(img, col * 64, line * 64, 64, WEST, GREEN);
            } else if (raw_sprite == 90) { // turning east
                image_draw_arrow(img, col * 64, line * 64, 64, EAST, WHITE);
            // -- Turning point ---------------------------------------------------------
            } else if (raw_sprite == 91) { // turning north east
                image_draw_arrow(img, col * 64, line * 64, 64, NORTH_EAST, WHITE);
            } else if (raw_sprite == 92) { // turning north
                image_draw_arrow(img, col * 64, line * 64, 64, NORTH, WHITE);
            } else if (raw_sprite == 93) { // turning north west
                image_draw_arrow(img, col * 64, line * 64, 64, NORTH_WEST, WHITE);
            } else if (raw_sprite == 94) { // turning west
                image_draw_arrow(img, col * 64, line * 64, 64, WEST, WHITE);
            } else if (raw_sprite == 95) { // turning south west
                image_draw_arrow(img, col * 64, line * 64, 64, SOUTH_WEST, WHITE);
            } else if (raw_sprite == 96) { // turning south
                image_draw_arrow(img, col * 64, line * 64, 64, SOUTH, WHITE);
            } else if (raw_sprite == 97) { // turning south east
                image_draw_arrow(img, col * 64, line * 64, 64, SOUTH_EAST, WHITE);
            // -- Objects ---------------------------------------------------------------
            } else if (raw_sprite >= 23 && raw_sprite <= 62 && sprites[raw_sprite + 42 - 64] != NULL) {
                // remove the first 64 walls with -64
                image_draw_image(img, col * 64, line * 64, sprites[raw_sprite + 42 - 64]);
            // -- Push wall -------------------------------------------------------------
            } else if (raw_sprite == 98) {
                image_draw_rect(img, col * 64, line * 64, 64, 64, GREEN, false);
            // -- Dead Guard ------------------------------------------------------------
            } else if (raw_sprite == 124) {
                image_draw_image(img, col * 64, line * 64, sprites[158 - 64]);
            // -- Guard -----------------------------------------------------------------
            } else if (raw_sprite == 108 || raw_sprite == 144 || raw_sprite == 180) { // guard any/med/hard standing east
                sprite = 119 - 64;
            } else if (raw_sprite == 109 || raw_sprite == 145 || raw_sprite == 181) { // guard any/med/hard standing north
                sprite = 117 - 64;
            } else if (raw_sprite == 110 || raw_sprite == 146 || raw_sprite == 182) { // guard any/med/hard standing west
                sprite = 115 - 64;
            } else if (raw_sprite == 111 || raw_sprite == 147 || raw_sprite == 183) { // guard any/med/hard standing south
                sprite = 113 - 64;
            } else if (raw_sprite == 112 || raw_sprite == 148 || raw_sprite == 184) { // guard any/med/hard patrolling east
                sprite = 127 - 64;
            } else if (raw_sprite == 113 || raw_sprite == 149 || raw_sprite == 185) { // guard any/med/hard patrolling north
                sprite = 125 - 64;
            } else if (raw_sprite == 114 || raw_sprite == 150 || raw_sprite == 186) { // guard any/med/hard patrolling west
                sprite = 123 - 64;
            } else if (raw_sprite == 115 || raw_sprite == 151 || raw_sprite == 187) { // guard any/med/hard patrolling south
                sprite = 121 - 64;
            // -- Dog ---------------------------------------------------------
            } else if (raw_sprite == 138 || raw_sprite == 174 || raw_sprite == 210) { // dog any/med/hard patrolling east
                sprite = 168 - 64;
            } else if (raw_sprite == 139 || raw_sprite == 175 || raw_sprite == 211) { // dog any/med/hard patrolling north
                sprite = 166 - 64;
            } else if (raw_sprite == 140 || raw_sprite == 176 || raw_sprite == 212) { // dog any/med/hard patrolling west
                sprite = 164 - 64;
            } else if (raw_sprite == 141 || raw_sprite == 177 || raw_sprite == 213) { // dog any/med/hard patrolling south
                sprite = 162 - 64;
            } else if (raw_sprite != 0) { // 0 is for empty
                printf("Plane 1 unhandled value : %u at %u,%u \n", raw_sprite, line, col);
                unhandled_plane1 += 1;
            }
            if (sprite != 0)
            {
                image_draw_image(img, col * 64, line * 64 - 3, sprites[sprite]); // -3 to center vertically the sprite
            }

            // Difficulty indication
            if ((raw_sprite >= 144 && raw_sprite <= 151) ||
                (raw_sprite >= 174 && raw_sprite <= 177)) // guard standing and patrolling
            {
                Rect rect = image_get_visible_rectangle(sprites[sprite], MAGENTA, 2);
                image_draw_rect(img, col * 64 + rect.x, line * 64 + rect.y - 3, rect.width, rect.height, ORANGE, false);
                //image_draw_rect(img, col * 64 + 18, line * 64 + 13, 29, 54, ORANGE, false);
            } else if ((raw_sprite >= 180 && raw_sprite <= 187) ||
                       (raw_sprite >= 210 && raw_sprite <= 213)) // guard standing and patrolling
            {
                Rect rect = image_get_visible_rectangle(sprites[sprite], MAGENTA, 2);
                image_draw_rect(img, col * 64 + rect.x, line * 64 + rect.y - 3, rect.width, rect.height, RED, false);
                //image_draw_rect(img, col * 64 + 18, line * 64 + 13, 29, 54, RED, false);
            }
        }
    }
    printf("Number of tiles unhandled: %u\n", unhandled_plane1);
    return img;
}
