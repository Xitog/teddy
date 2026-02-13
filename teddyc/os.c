#include "os.h"

char * getCurrentDir()
{
    DWORD size = GetCurrentDirectory(0, NULL);
    char * dirname = malloc(sizeof(char) * size);
    GetCurrentDirectory(size, dirname);
    return dirname;
}
