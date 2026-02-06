#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h> // malloc
#include <stdint.h> // int32_t
#include <stdio.h>  // printf, fopen, fread, fclose, fseek, ftell, rewind, SEEK_END
#include <stdbool.h> // bool

typedef struct DataStructure {
    uint8_t * data;
    uint32_t size;
} Data;

int32_t get_file_size(FILE *f);
bool load_file(const char * file_path, Data * mem);
uint32_t read_uint32(const Data mem, const uint32_t at);
int32_t read_int32(const Data mem, const uint32_t at);
uint16_t read_uint16(const Data mem, const uint32_t at);

#endif
