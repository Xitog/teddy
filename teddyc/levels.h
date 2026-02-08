#ifndef LEVELS_H
#define LEVELS_H

#include <stdint.h> // int32_t
#include <stdbool.h> // bool
#include "common.h"
#include "image.h"

typedef struct HeaderStructure {
    uint16_t magic_word;
    uint32_t level_header_ptr[100];
    uint8_t number;
} Header;

typedef struct LevelHeaderStructure {
    int32_t offsetPlane0;  // @00 offset in GAMEMAPS file where the compressed plane 0 data begins (or <= 0 if plane not present)
    int32_t offsetPlane1;  // @04 same for 1
    int32_t offsetPlane2;  // @08 same for 2
    uint16_t lenghtPlane0; // @12 length of compressed plane 0 data in bytes
    uint16_t lenghtPlane1; // @14 length of compressed plane 0 data in bytes
    uint16_t lenghtPlane2; // @16 length of compressed plane 0 data in bytes
    uint16_t width;        // @18 width of the level in tiles
    uint16_t height;       // @20 height of the level in tiles
    uint16_t * dataPlane0;
    uint16_t * dataPlane1;
    char name[16];         // @22 internal name fo the level, null-terminated string
} LevelHeader;

typedef struct LevelStructure {
    uint8_t number;
    uint16_t width;
    uint16_t height;
    uint8_t planeNumber;
    uint16_t *** plane; // a table of 2d tables
} Level;

void display_header(const Header * header);
bool read_header(const Data mem, Header * header);
bool read_level_header(const Data mem, uint32_t offset, LevelHeader * lh);
void display_level_header(const LevelHeader * lh);
bool uncompress(const Data mem, uint32_t offset, uint16_t length, uint16_t * output);
bool read_plane(const LevelHeader * lhs, uint8_t level, uint8_t plane, const Data mem, uint16_t * output);
bool export_plane_to_txt_old(const LevelHeader * level_headers, uint8_t level, uint8_t plane);
bool read_level_data(const Data levelDataFile, LevelHeader * level_headers, uint8_t level);

Level create_level(uint8_t number, uint16_t width, uint16_t height, uint8_t planeNumber);
Level create_level_from_files(const Data levelDataFile, LevelHeader * level_headers, uint8_t level);
void level_stat(Level lvl, uint8_t plane, bool order_by_count);
bool export_plane_to_txt(Level lvl, uint8_t plane);
Image * level_to_image(Level lvl, uint8_t plane, Image * textures[], Image * sprites[]);

#endif
