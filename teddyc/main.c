// cl .\main.c /Fe:neguev.exe
// cl .\main.c .\libattopng.c .\common.c .\assets.c .\levels.c  /Fe:neguev.exe
// With the help of:
// - https://moddingwiki.shikadi.net/wiki/GameMaps_Format
// - https://moddingwiki.shikadi.net/wiki/Id_Software_RLEW_compression
// - https://vpoupet.github.io/wolfenstein/docs/files

// Number of assets : 538
// First sprite (= number of walls) : 64
// First sound (= number of sprites + wall) : 452 (452 - 64 = 388 sprites)

#include <stdlib.h>  // EXIT_SUCCESS, malloc
#include <stdio.h>   // printf, fopen, fread, fclose, fseek, ftell, rewind, SEEK_END
#include <string.h>  // memcpy
#include <stdint.h>  // int32_t
#include <stdbool.h> // bool
#include <math.h>    // round
#include <ctype.h>   // isdigit

#include ".\libraries\miniz.h"
#include ".\libraries\spng.h"

#include "common.h"
#include "levels.h"
#include "assets.h"
#include "image.h"
#include "os.h"

const char *TEDDY_VERSION = "0.1a";

const char *HEADER_FILE = "D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0\\MAPHEAD.WL1";
const char *LEVEL_FILE = "D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0\\MAPTEMP.WL1";
const char *ASSET_FILE = "D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0\\VSWAP.WL1";

const uint8_t ALL_PLANES = 3;

void setConsoleColorGreen()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, FOREGROUND_GREEN);
}

void setConsoleColorRed()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, FOREGROUND_RED);
}

void setConsoleColorDefault()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

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

void to_matrix(const Data assetDataFile, uint32_t offset, uint16_t length, uint8_t **rotated_matrix)
{
    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        // printf("Access to: x=%u y=%u put in i=%u (len = %u)\n", x, y, i, len);
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
    FILE *file = fopen(file_name, "w");
    fprintf(file, "P3\n# ASCII PPM file\n64 64\n255\n\n");
    uint32_t offset = assetHeader.pointers[index];
    uint16_t length = assetHeader.lengths[index];
    if (length != 4096)
    {
        return true;
    }
    printf("Wall texture %u : %u ptr %u len\n", index, offset, length);
    uint8_t rotated_matrix[64][64];
    // to_matrix(assetDataFile, ptr, len, (uint8_t **)rotated_matrix);

    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        // printf("Access to: x=%u y=%u put in i=%u (len = %u)\n", x, y, i, len);
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
    // libattopng_t *wall = libattopng_new(64, 64, PNG_RGB);
    uint32_t count = 0;
    uint32_t x = 0;
    uint32_t y = 0;
    for (uint32_t i = ptr; i < ptr + len; i++)
    {
        uint8_t pixel = assetDataFile.data[i];
        count += 1;
        // printf("%u. %u\n", count, pixel);
        // uint32_t rgb = (palette[pixel][0] << 24) | (palette[pixel][1] << 16) | (palette[pixel][2] << 8);
        uint32_t rgb = palette[pixel][0] + palette[pixel][1] * 255 + palette[pixel][2] * 255 * 255;
        // libattopng_set_pixel(wall, y, x, rgb);
        x += 1;
        if (x == 64)
        {
            x = 0;
            y += 1;
        }
    }
    char file_name[255];
    sprintf(file_name, "wall%u.png", index);
    // libattopng_save(wall, file_name);
    // libattopng_destroy(wall);
    return true;
}

bool is_str_digit(const char *s)
{
    for (int32_t i = 0; i < strlen(s); i++)
    {
        if (isdigit(s[i]) == 0)
        {
            return false;
        }
    }
    return true;
}

int32_t str_to_digit(const char *s)
{
    int32_t res = 0;
    int32_t dec = 1;
    for (int32_t i = strlen(s) - 1; i >= 0; i--)
    {
        res += dec * (s[i] - '0');
        dec *= 10;
    }
    return res;
}

bool str_is(const char *value, const char *expected)
{
    return strcmp(value, expected) == 0;
}

typedef struct LevelIdentifierStruct
{
    uint8_t episode;        // E1
    uint8_t level;          // M1
    uint8_t index;          // = 0
} LevelIdentifier;

bool is_valid_level(const char *value)
{
    bool ok = false;
    if (strlen(value) == 4)
    {
        if (
            (value[0] == 'E' || value[0] == 'e') &&
            isdigit(value[1]) &&
            (value[2] == 'M' || value[2] == 'm') &&
            isdigit(value[3]))
        {
            ok = true;
        }
    }
    else if (strlen(value) == 5)
    {
        if (strcmp(value, "E1M10") == 0 || strcmp(value, "e1m10"))
        {
            ok = true;
        }
    }
    return ok;
}

LevelIdentifier str_to_level_identifier(const char *value)
{
    uint8_t episode = 0;
    uint8_t level = 0;
    if (strlen(value) == 4)
    {
        if (
            (value[0] == 'E' || value[0] == 'e') && // TODO: Handle max E
            isdigit(value[1]) &&
            (value[2] == 'M' || value[2] == 'm') &&
            isdigit(value[3]))
        {
            episode = value[1] - '0';
            level = value[3] - '0';
        }
    }
    else if (strlen(value) == 5)
    {
        if (strcmp(value, "E1M10") == 0 || strcmp(value, "e1m10")) // TODO: Handle E>1
        {
            episode = 1;
            level = 10;
        }
    }
    LevelIdentifier li = { .episode = episode, .level = level, .index = level - 1}; // TODO: Handle E>1
    return li;
}

const bool EXPORT_PLANE_TO_TEXT = false;
const bool EXPORT_ALL_SPRITES = false;

int main(int argc, const char *argv[])
{
    // -- Variables ---------------------------------------------------------------------

    uint32_t level_index = 0;
    bool order_by_count = false;

    // -- Load data (reading level files) -----------------------------------------------

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
    LevelHeader *level_headers = malloc(sizeof(LevelHeader) * header.number);
    printf("Level name : W x H | Plane 0 [Size] | Plane 1 [Size] | Plane 2 [Size] |\n");
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

    // -- Reading Asset Data File -------------------------------------------------------

    Data assetDataFile = {.data = NULL, .size = 0};
    ok = load_file(ASSET_FILE, &assetDataFile);
    AssetHeader assetHeader;
    ok = read_asset_header(assetDataFile, &assetHeader);
    printf("Number of assets : %u\n", assetHeader.number);                              // 538
    printf("First sprite (= number of wall) : %d\n", assetHeader.first_sprite);         // 64
    printf("First sound (= number of sprites + wall) : %d\n", assetHeader.first_sound); // 452 (- 64 = 388)
    printf("Number of walls : %u\n", assetHeader.first_sprite);
    printf("Number of sprites : %u\n", assetHeader.first_sound - assetHeader.first_sprite);

    // -- Walls -------------------------------------------------------------------------

    // Making images for all wall textures
    Image **textures;
    uint16_t number_of_walls = assetHeader.first_sprite;
    textures = malloc(sizeof(Image *) * number_of_walls);
    char file_name[255];
    for (uint32_t i = 0; i < number_of_walls; i++)
    {
        Image *img = wall_to_image(assetDataFile, assetHeader, i);
        textures[i] = NULL;
        if (img != NULL)
        {
            textures[i] = img;
        }
    }

    // -- Sprites -----------------------------------------------------------------------

    const char *name[] = {
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
        "gold_key",         //  85
        "silver_key",       //  86
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
    };
    const uint8_t max_named_sprites = 112; // 113 est le premier guard
    Image **sprites;
    sprites = malloc(sizeof(Image *) * assetHeader.first_sound - assetHeader.first_sprite);

    for (uint16_t i = assetHeader.first_sprite; i < assetHeader.first_sound; i++)
    {
#ifdef DEBUG
        printf("Exporting sprite #%u named %s\n", i, name[i - assetHeader.first_sprite]);
#endif
        Image *img = sprite_to_image(assetDataFile, assetHeader, i);
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

    // -- Commands ----------------------------------------------------------------------

    // id : identify game and version
    // list : list all recognized files
    // asset X : extract asset at index X (or all) in VSWAP in the correct format (bmp)
    // map X F : extract map number X (or all) in F format (txt, csv or bmp)

    bool good = false;
    if (argc == 1 || (argc == 2 && str_is(argv[1], "help")))
    {
        printf("---------------------------------\n");
        printf("- Teddy v%s\n", TEDDY_VERSION);
        printf("---------------------------------\n");
        printf("Help menu:\n");
        printf("info <level>          : map information\n");
        printf("count <level>         : count of map objects\n"); // ex stats
        printf("extract walls         : extract all walls\n");
        printf("export <level> <type> : export a <level> to <type> (png, bmp or all)\n");
        printf("check <level>         : check if everything is known for <level>\n");
        printf("Every level must be in the format EXMY or eXmY\n");
        good = true;
    }
    else if (argc >= 2)
    {
        if (str_is(argv[1], "info"))
        {
            if (argc == 2 || !is_str_digit(argv[2]))
            {
                printf("You must indicate a level number to get information.\n");
                good = false;
            }
            else
            {
                level_index = str_to_digit(argv[2]);
                good = true;
                printf("Getting information on level %u\n", level_index);
                Level lvl = create_level_from_files(levelDataFile, level_headers, level_index);
                level_info(lvl);
            }
        }
        else if (str_is(argv[1], "count"))
        {
            if (argc == 2 || !is_str_digit(argv[2]))
            {
                printf("You must indicate a level number to run the statistics.\n");
                good = false;
            }
            else
            {
                level_index = str_to_digit(argv[2]);
                good = true;
                if (argc == 4 && str_is(argv[3], "count_order"))
                {
                    order_by_count = true;
                }
                printf("Running statistics on level %u\n", level_index);
                Level lvl = create_level_from_files(levelDataFile, level_headers, level_index);
                level_stat(lvl, 0, order_by_count);
            }
        }
        else if (str_is(argv[1], "extract"))
        {
            if (argc == 2)
            {
                printf("You must indicate which asset to extract :\n-walls for all wall.\n");
                good = false;
            }
            else if (str_is(argv[2], "walls"))
            {
                for (uint16_t i = 0; i < number_of_walls; i++)
                {
                    if (textures[i] != NULL)
                    {
                        sprintf(file_name, ".\\walls\\%02u_wall.bmp", i);
                        // image_save_to_bmp(textures[i], file_name);
                        printf("[%u / %u] Saving to %s\n", i, number_of_walls, file_name);
                    }
                }
            }
        }
        else if (str_is(argv[1], "export"))
        {
            if (argc == 2 || !is_valid_level(argv[2]))
            {
                printf("You must indicate a level with format EXMY or eXmY to export.\n");
            }
            else
            {
                bool png = false;
                bool bmp = true;
                if (argc == 4)
                {
                    if (str_is(argv[3], "png"))
                    {
                        png = true;
                        bmp = false;
                    }
                    else if (str_is(argv[3], "bmp"))
                    {
                        png = false;
                        bmp = true;
                    }
                    else if (str_is(argv[3], "all"))
                    {
                        png = true;
                        bmp = true;
                    }
                }
                LevelIdentifier identifier = str_to_level_identifier(argv[2]);
                good = true;
                printf("Exporting level e%um%u\n", identifier.episode, identifier.level);
                Image *level_image = NULL;
                bool NO_GRID = false;
                bool THIN_WALL = true;
                Level lvl = create_level_from_files(levelDataFile, level_headers, identifier.index);
                level_image = level_to_image(lvl, ALL_PLANES, textures, sprites, NO_GRID, THIN_WALL);
                if (level_image != NULL)
                {
                    if (bmp)
                    {
                        sprintf(file_name, "e%um%u.bmp", identifier.episode, identifier.level);
                        image_save_to_bmp(level_image, file_name);
                    }
                    if (png)
                    {
                        sprintf(file_name, "e%um%u.png", identifier.episode, identifier.level);
                        image_save_to_png(level_image, file_name);
                    }
                    image_free(level_image);
                }
            }
        }
        else if (str_is(argv[1], "check"))
        {
            if (argc == 2 || !is_valid_level(argv[2]))
            {
                printf("You must indicate a level with format EXMY or eXmY to check.\n");
            }
            else
            {
                LevelIdentifier identifier = str_to_level_identifier(argv[2]);
                Level lvl = create_level_from_files(levelDataFile, level_headers, identifier.index);
                uint32_t unknown = 0;
                for (uint16_t line = 0; line < lvl.width; line++)
                {
                    for (uint16_t col = 0; col < lvl.height; col++)
                    {
                        uint16_t val0 = lvl.plane[0][line][col];
                        uint16_t val1 = lvl.plane[1][line][col];

                        if (val0 <= 64 && textures[val0] == NULL)
                        {
                            printf(">>> [Plane 0] Unloaded texture at line=%u col=%u: %u\n", line, col, val0);
                            unknown += 1;
                        }
                        else if (val0 != 90 && val0 != 91 && val0 != 92 && val0 != 93 && val0 != 100 && val0 < 106 && val0 > 143)
                        {
                            printf(">>> [Plane 0] Unknown value at line=%u col=%u: %u\n", line, col, val0);
                            unknown += 1;
                        }

                        if (!is_empty(val1) && !is_guard(val1) && !is_dog(val1) && !is_ss(val1) &&
                            !is_starting_point(val1) && !is_turning_point(val1) && !is_object(val1) &&
                            !is_pushwall(val1) && !is_dead_guard(val1))
                        {
                            printf(">>> [Plane 1] Unknown value at line=%u col=%u: %u\n", line, col, val1);
                            unknown += 1;
                        }
                    }
                }
                good = true;
                if (unknown == 0)
                {
                    setConsoleColorGreen();
                    printf("Level e%um%u : all values handled.\n", identifier.episode, identifier.level);
                    setConsoleColorDefault();
                }
                else
                {
                    setConsoleColorRed();
                    printf("Level e%um%u : %u unknown values.\n", identifier.episode, identifier.level, unknown);
                    setConsoleColorDefault();
                }
            }
        }
        else if (str_is(argv[1], "list"))
        {
            printf("List all files\n");
        }
    }
    // Console
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!good)
    {
        SetConsoleTextAttribute(console, FOREGROUND_RED);
        printf("Goodbye\n");
        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return EXIT_FAILURE;
    }
    else
    {
        SetConsoleTextAttribute(console, FOREGROUND_GREEN); // | FOREGROUND_GREEN | FOREGROUND_BLUE); // white
        printf("Goodbye\n");
        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        return EXIT_SUCCESS;
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

    getBuildInfo();
    char *cwd = getCurrentDir();
    printf("Current dir is : %s\n", cwd);
    getFiles(cwd);
    printf("\n-----------------------------------------\n");
    getFiles("D:\\Perso\\Projets\\git\\teddy\\data\\Wolfenstein 3D\\Shareware\\wolfenstein-3d-1.0");

    Image *img = image_new(100, 100);
    image_draw_line(img, 0, 0, 99, 99, RED);
    image_draw_line(img, 0, 99, 99, 0, GREEN);
    image_draw_digit(img, 70, 50, 0, BLUE);
    image_draw_digit(img, 80, 50, 1, MAGENTA);
    image_save_to_bmp(img, "textX.bmp");

    // MiniZ

    static const char *source = "Blood and money. Blood and money. Blood and money. Blood and money. Blood and money. Blood and money.";
    printf("Version of miniz.c : %s\n", MZ_VERSION);
    uint64_t src_len = (uint64_t)strlen(source);
    uint64_t cmp_len = compressBound(src_len);
    uint8_t *cmp_data = (uint8_t *)malloc((size_t)cmp_len);
    int cmp_status = compress(cmp_data, (mz_ulong *)&cmp_len, (const unsigned char *)source, src_len);
    if (cmp_status != Z_OK)
    {
        printf("compress() failed!\n");
        free(cmp_data);
    }
    printf("Compressed from %llu to %llu bytes\n", (uint64_t)src_len, (uint64_t)cmp_len);

    // Visible rect
    Image *guard = sprites[119 - 64];
    Image *dog = sprites[168 - 64];
    Image *dg1 = image_new(128, 128);
    image_fill(dg1, WHITE);
    image_draw_image(dg1, 0, 0, dog);

    Rect rect = image_get_visible_rectangle(guard, MAGENTA, 0);
    printf("Rect guard, no spaces = %u %u %u %u\n", rect.x, rect.y, rect.width, rect.height);
    rect = image_get_visible_rectangle(guard, MAGENTA, 2);
    printf("Rect guard, 02 spaces = %u %u %u %u\n", rect.x, rect.y, rect.width, rect.height);
    rect = image_get_visible_rectangle(dog, MAGENTA, 0);
    printf("Rect dog,   no spaces = %u %u %u %u\n", rect.x, rect.y, rect.width, rect.height);
    rect = image_get_visible_rectangle(dog, MAGENTA, 1);
    printf("Rect dog,   01 spaces = %u %u %u %u\n", rect.x, rect.y, rect.width, rect.height);
    image_draw_rect(dg1, rect.x, rect.y, rect.width, rect.height, ORANGE, false);
    rect = image_get_visible_rectangle(dog, MAGENTA, 2);
    printf("Rect dog,   02 spaces = %u %u %u %u\n", rect.x, rect.y, rect.width, rect.height);
    image_draw_rect(dg1, rect.x, rect.y, rect.width, rect.height, RED, false);
    image_save_to_bmp(dg1, "dg1.bmp");
    image_free(dg1);

    return EXIT_SUCCESS;
}
