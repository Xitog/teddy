#ifndef ASSETS_H
#define ASSETS_H

#include "stdlib.h"
#include "stdbool.h"
#include "common.h"
#include "image.h"

typedef struct AssetHeaderStructure
{
    uint16_t number;        // number of chunks (wall textures + sprites + sounds)
    uint16_t first_sprite;  // index of the first sprinte = number of wall textures
    uint16_t first_sound;   // index of the first sound = number of wall tex + sprites
    uint32_t * pointers;    // offsets of the each chunk (the table has number elements)
    uint16_t * lengths;     // byte length of each chunk (the table has number elements)
} AssetHeader;

extern uint8_t palette[256][3];

bool read_asset_header();
Image * wall_to_image(const Data assetDataFile, AssetHeader assetHeader, uint16_t index);
Image * sprite_to_image(const Data assetDataFile, AssetHeader assetHeader, uint16_t index);

#endif
