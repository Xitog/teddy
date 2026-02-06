#include "common.h"

int32_t get_file_size(FILE *f)
{
    int32_t size = -1;
    if (f != NULL)
    {
        fseek(f, 0L, SEEK_END); // go to the end of file
        size = ftell(f);        // current byte is the size
        rewind(f);              // reset to the start of the file
    }
    return size;
}

bool load_file(const char * file_path, Data * mem)
{
    FILE *f = fopen(file_path, "rb");
    if (f == NULL)
    {
        printf("ERROR: Unable to file: %s.\n", file_path);
        return false;
    }
    mem->size = get_file_size(f);
    printf("------------------------------------------------------------\n");
    printf("Loading file...\n");
    printf("File path           : %s\n", file_path);
    printf("File size           : %u bytes\n", mem->size);
    printf("------------------------------------------------------------\n");
    mem->data = malloc(sizeof(uint8_t) * mem->size);
    fread(mem->data, sizeof(uint8_t), mem->size, f);
    fclose(f);
    return true;
}

uint32_t read_uint32(const Data mem, const uint32_t at)
{
    if (at + 3 >= mem.size)
    {
        printf("ERROR : memory overflow : %u >= size = %u\n", at, mem.size);
    }
    return mem.data[at] + mem.data[at + 1] * 256 + mem.data[at + 2] * 256 * 256 + mem.data[at + 3] * 256 * 256 * 256;
}

int32_t read_int32(const Data mem, const uint32_t at)
{
    if (at + 4 >= mem.size)
    {
        printf("ERROR : memory overflow : %u >= size = %u\n", at, mem.size);
    }
    int32_t i = 0;
    memcpy(&i, mem.data + at, 4);
    return i;
}

uint16_t read_uint16(const Data mem, const uint32_t at)
{
    if (at + 1 >= mem.size)
    {
        printf("ERROR : memory overflow : %u >= size = %u\n", at, mem.size);
    }
    return mem.data[at] + mem.data[at+1] * 256;
}
