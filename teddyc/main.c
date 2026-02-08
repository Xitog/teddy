// cl .\main.c /Fe:neguev.exe
// cl .\main.c .\libattopng.c .\common.c .\assets.c .\levels.c  /Fe:neguev.exe
// With the help of:
// - https://moddingwiki.shikadi.net/wiki/GameMaps_Format
// - https://moddingwiki.shikadi.net/wiki/Id_Software_RLEW_compression
// - https://vpoupet.github.io/wolfenstein/docs/files

// Number of assets : 538
// First sprite (= number of walls) : 64
// First sound (= number of sprites + wall) : 452 (452 - 64 = 388 sprites)

#include <stdlib.h> // EXIT_SUCCESS, malloc
#include <stdio.h>  // printf, fopen, fread, fclose, fseek, ftell, rewind, SEEK_END
#include <string.h> // memcpy
#include <stdint.h> // int32_t
#include <stdbool.h> // bool
#include <math.h> // round
#include <ctype.h> // isdigit

#include "common.h"
#include "levels.h"
#include "assets.h"
#include "libattopng.h"
#include "image.h"

const char * HEADER_FILE = "D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0\\MAPHEAD.WL1";
const char * LEVEL_FILE = "D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0\\MAPTEMP.WL1";
const char * ASSET_FILE = "D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0\\VSWAP.WL1";

const uint8_t ALL_PLANES = 3;

void first_five_pixel(const Data assetDataFile, AssetHeader assetHeader, uint16_t index)
{
    uint32_t ptr = assetHeader.pointers[index];
    uint32_t len = assetHeader.lengths[index];
    printf("Wall texture %u : %u ptr %u len\n", index, ptr, len);
    uint8_t count = 0;
    for (uint32_t i = ptr; i < ptr + 5; i++)
    {
        uint8_t pixel = assetDataFile.data[i];
        uint8_t r = palette[pixel][0];
        uint8_t g = palette[pixel][1];
        uint8_t b = palette[pixel][2];
        uint32_t rgb = palette[pixel][0] * 255 * 255 + palette[pixel][1] * 255 + palette[pixel][2];
        printf("%d. in palette : %u (%u, %u, %u) or %u\n", count, pixel, r, g, b, rgb);
        count += 1;
    }
}

void to_matrix(const Data assetDataFile, uint32_t offset, uint16_t length, uint8_t ** rotated_matrix)
{
    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        //printf("Access to: x=%u y=%u put in i=%u (len = %u)\n", x, y, i, len);
        uint8_t pixel = assetDataFile.data[offset + i]; // rotation horizontale i + 64 * y ---
        rotated_matrix[x][y] = pixel;
        x += 1;
        if (x == 64)
        {
            x = 0;
            y += 1;
        }
    }
}

bool wall_to_ppm(const Data assetDataFile, AssetHeader assetHeader, uint16_t index)
{
    char file_name[255];
    sprintf(file_name, "wall%u.ppm", index);
    FILE * file = fopen(file_name, "w");
    fprintf(file, "P3\n# ASCII PPM file\n64 64\n255\n\n");
    uint32_t offset = assetHeader.pointers[index];
    uint16_t length = assetHeader.lengths[index];
    if (length != 4096)
    {
        return true;
    }
    printf("Wall texture %u : %u ptr %u len\n", index, offset, length);
    uint8_t rotated_matrix[64][64];
    //to_matrix(assetDataFile, ptr, len, (uint8_t **)rotated_matrix);

    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        //printf("Access to: x=%u y=%u put in i=%u (len = %u)\n", x, y, i, len);
        uint8_t pixel = assetDataFile.data[offset + i]; // rotation horizontale i + 64 * y ---
        rotated_matrix[x][y] = pixel;
        x += 1;
        if (x == 64)
        {
            x = 0;
            y += 1;
        }
    }

    for (uint32_t i = 0; i < 64; i++)
    {
        for (uint32_t j = 0; j < 64; j++)
        {
            uint8_t pixel = rotated_matrix[i][j];
            uint8_t r = palette[pixel][0];
            uint8_t g = palette[pixel][1];
            uint8_t b = palette[pixel][2];
            fprintf(file, "%d %d %d\n", r, g, b);
        }
    }
    fprintf(file, "\n\n");
    fclose(file);
    return true;
}

bool wall_to_png(const Data assetDataFile, AssetHeader assetHeader, uint16_t index)
{
    uint32_t ptr = assetHeader.pointers[index];
    uint32_t len = assetHeader.lengths[index];
    if (len != 4096)
    {
        return true;
    }
    printf("Wall texture %u : %u ptr %u len\n", index, ptr, len);
    libattopng_t *wall = libattopng_new(64, 64, PNG_RGB);
    uint32_t count = 0;
    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = ptr; i < ptr + len; i++)
    {
        uint8_t pixel = assetDataFile.data[i];
        count += 1;
        //printf("%u. %u\n", count, pixel);
        //uint32_t rgb = (palette[pixel][0] << 24) | (palette[pixel][1] << 16) | (palette[pixel][2] << 8);
        uint32_t rgb = palette[pixel][0] + palette[pixel][1] * 255 + palette[pixel][2] * 255 * 255;
        libattopng_set_pixel(wall, y, x, rgb);
        x += 1;
        if (x == 64)
        {
            x = 0;
            y += 1;
        }
    }
    char file_name[255];
    sprintf(file_name, "wall%u.png", index);
    libattopng_save(wall, file_name);
    libattopng_destroy(wall);
    return true;
}

bool is_str_digit(const char * s)
{
    for (int32_t i=0; i < strlen(s); i++)
    {
        if (isdigit(s[i]) == 0)
        {
            return false;
        }
    }
    return true;
}

int32_t str_to_digit(const char * s)
{
    int32_t res = 0;
    int32_t dec = 1;
    for (int32_t i=strlen(s)-1; i >= 0; i--)
    {
        res += dec * (s[i] - '0');
        dec *= 10;
    }
    return res;
}

int main(int argc, const char *argv[])
{
    //-------------------------------------------------------------------------
    // Switchs
    //-------------------------------------------------------------------------

    const bool EXPORT_PLANE_TO_TEXT = false;
    const bool EXPORT_ALL_SPRITES = false;
    const bool EXPORT_ALL_WALLS = false;
    const bool EXPORT_PLANE_TO_BMP = false;
    bool RUN_STAT = false;
    uint32_t TARGET = 0;
    bool ORDER_BY_COUNT = false;

    //-------------------------------------------------------------------------
    // Commands
    //-------------------------------------------------------------------------
    // id : identify game and version
    // list : list all recognized files
    // asset X : extract asset at index X (or all) in VSWAP in the correct format (bmp)
    // map X F : extract map number X (or all) in F format (txt, csv or bmp)
    // stat X : run statistics on map X

    bool good = false;
    printf("Arguments:\n");
    for (uint8_t i = 0; i < argc; i++)
    {
        printf("    %02u. %s\n", i, argv[i]);
    }
    if (argc >= 2)
    {
        if (strcmp(argv[1], "stats") == 0)
        {
            if (argc == 2 || !is_str_digit(argv[2]))
            {
                printf("You must indicate a level number to run the statistics.\n");
                good = false;
            }
            else
            {
                TARGET = str_to_digit(argv[2]);
                printf("Running statistics on level %u\n", TARGET);
                RUN_STAT = true;
                good = true;
                if (argc == 4 && strcmp(argv[3], "count_order") == 0)
                {
                    ORDER_BY_COUNT = true;
                }
            }
        }
        else if (strcmp(argv[1], "list") == 0)
        {
            printf("List all files\n");
        }
    }
    if (!good)
    {
        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------------
    // Reading Level files
    //-------------------------------------------------------------------------

    Data headerDataFile = {.data = NULL, .size = 0};
    bool ok = load_file(HEADER_FILE, &headerDataFile);
    if (!ok)
    {
        return EXIT_FAILURE;
    }
    Header header;
    ok = read_header(headerDataFile, &header);
    if (!ok)
    {
        return EXIT_FAILURE;
    }
    display_header(&header);
    Data levelDataFile = {.data = NULL, .size = 0};
    ok = load_file(LEVEL_FILE, &levelDataFile);
    if (!ok)
    {
        return EXIT_FAILURE;
    }
    // Read level headers
    LevelHeader * level_headers = malloc(sizeof(LevelHeader) * header.number);
    for (uint8_t i = 0; i < header.number; i++)
    {
        ok = read_level_header(levelDataFile, header.level_header_ptr[i], &level_headers[i]);
        if (!ok)
        {
            return EXIT_FAILURE;
        }
        display_level_header(&level_headers[i]);
        ok = read_level_data(levelDataFile, level_headers, i);
        if (!ok)
        {
            return EXIT_FAILURE;
        }
    }

    // Exports
    if (EXPORT_PLANE_TO_TEXT)
    {
        ok = export_plane_to_txt_old(level_headers, 0, 0);
        if (!ok)
        {
            return EXIT_FAILURE;
        }
        ok = export_plane_to_txt_old(level_headers, 0, 1);
        if (!ok)
        {
            return EXIT_FAILURE;
        }
    }

    // Stats
    if (RUN_STAT)
    {
        printf("Start stat on %u\n", TARGET);
        Level lvl = create_level_from_files(levelDataFile, level_headers, TARGET);
        level_stat(lvl, 0, ORDER_BY_COUNT);
    }

    //-------------------------------------------------------------------------
    // Reading Asset Data File
    //-------------------------------------------------------------------------

    Data assetDataFile = {.data = NULL, .size = 0};
    ok = load_file(ASSET_FILE, &assetDataFile);
    AssetHeader assetHeader;
    ok = read_asset_header(assetDataFile, &assetHeader);
    printf("Number of assets : %u\n", assetHeader.number); // 538
    printf("First sprite (= number of wall) : %d\n", assetHeader.first_sprite); // 64
    printf("First sound (= number of sprites + wall) : %d\n", assetHeader.first_sound); // 452 (- 64 = 388)
    printf("Number of walls : %u\n", assetHeader.first_sprite);
    printf("Number of sprites : %u\n", assetHeader.first_sound - assetHeader.first_sprite);

    //-------------------------------------------------------------------------
    // Walls
    //-------------------------------------------------------------------------

    // Making images for all wall textures and [save them to bmp]
    Image ** textures;
    textures = malloc(sizeof(Image *) * assetHeader.first_sprite);
    char file_name[255];
    for (uint32_t i = 0; i < assetHeader.first_sprite; i++)
    {
        Image * img = wall_to_image(assetDataFile, assetHeader, i);
        textures[i] = NULL;
        if (img != NULL)
        {
            if (EXPORT_ALL_WALLS)
            {
                sprintf(file_name, ".\\walls\\%02u_wall.bmp", i);
                image_save_to_bmp(img, file_name);
            }
            textures[i] = img;
            // wall_to_ppm...
        }
    }

    //-------------------------------------------------------------------------
    // Sprites
    //-------------------------------------------------------------------------

    const char * name[] = {
        "demo",             //  64
        "puddle",           //  65
        "green_barrel",     //  66
        "table_and_chairs", //  67
        "floor_lamp",       //  68
        "chandelier",       //  69
        "hanged_skeleton",  //  70
        "dog_food",         //  71
        "while_pillar",     //  72
        "tree",             //  73
        "skeleton",         //  74
        "sink",             //  75
        "potted_plant",     //  76
        "urn",              //  77
        "bare_table",       //  78
        "ceiling_lamp",     //  79
        "pans",             //  80
        "suit_of_armor",    //  81
        "hanging_cage",     //  82
        "skeleton_in_cage", //  83
        "bones",            //  84
        "silver_key",       //  85
        "gold_key",         //  86
        "bed",              //  87
        "bucket",           //  88
        "food",             //  89
        "medkit",           //  90
        "ammo_clip",        //  91
        "machine_gun",      //  92
        "chaingun",         //  93
        "cross_treasure",   //  94
        "chalice_treasure", //  95
        "chest_treasure",   //  96
        "crown_treasure",   //  97
        "life_up",          //  98
        "bones_and_blood",  //  99
        "barrel",           // 100
        "well",             // 101
        "empty_well",       // 102
        "blood",            // 103
        "flag",             // 104
        "x_call_apogee",    // 105 x
        "x_m1",             // 106 x
        "x_m2",             // 107 x
        "x_m3",             // 108 x
        "x_m4",             // 109 x
        "x_stove",          // 110 x
        "x_spears",         // 111 x
        "x_m5",             // 112 x
        "x_m6"              // 113
    };
    const uint8_t max_named_sprites = 113;
    Image ** sprites;
    sprites = malloc(sizeof(Image *) * assetHeader.first_sound - assetHeader.first_sprite);

    for (uint16_t i = assetHeader.first_sprite; i < assetHeader.first_sound; i++)
    {
#ifdef DEBUG
        printf("Exporting sprite #%u named %s\n", i, name[i - assetHeader.first_sprite]);
#endif
        Image * img = sprite_to_image(assetDataFile, assetHeader, i);
        sprites[i - assetHeader.first_sprite] = img;
        if (img != NULL)
        {
            if (EXPORT_ALL_SPRITES)
            {
                if (i <= max_named_sprites)
                {
                    sprintf(file_name, ".\\sprites\\%03u_%s.bmp", i, name[i - assetHeader.first_sprite]);
                }
                else
                {
                    sprintf(file_name, ".\\sprites\\%03u.bmp", i);
                }
                image_save_to_bmp(img, file_name);
            }
        }
    }

    //-------------------------------------------------------------------------
    // Level export to bmp
    //-------------------------------------------------------------------------

    if (EXPORT_PLANE_TO_BMP)
    {
        Level lvl = create_level_from_files(levelDataFile, level_headers, 0);
        Image * img = level_to_image(lvl, ALL_PLANES, textures, sprites);
        //export_plane_to_txt(lvl, 0);
        image_save_to_bmp(img, "level0.bmp");
        image_free(img);
    }

    return EXIT_SUCCESS;
}
