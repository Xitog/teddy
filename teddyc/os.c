#include "os.h"

void get_build_info()
{
    #ifdef _WIN32
    printf("Compiled for Win32\n"); // Win32 1937
    #endif
    #ifdef _WIN64
    printf("Compiled for Win64\n");
    #endif
    printf("Version of the compiler: %u\n", _MSC_VER);
}

char * get_current_dir()
{
    DWORD size = GetCurrentDirectory(0, NULL);
    char * dirname = malloc(sizeof(char) * size);
    GetCurrentDirectory(size, dirname);
    return dirname;
}

void get_files(const char * dir_path)
{
    // m y d i r \0 strlen = 5 +1 = 6
    // _ _ _ _ _ _  _ _ buffer = 8
    // m y d i r \\ * \0

    size_t s = strlen(dir_path) + 1; // for \0
    char * buffer = malloc(sizeof(char) * s + 2);
    strcpy(buffer, dir_path);
    buffer[s - 1] = '\\';
    buffer[s] = '*';
    buffer[s + 1] = '\0';
    printf("DEBUG : get_files for : %s\n", dir_path);
    printf("DEBUG : buffer is :    %s\n", buffer);
    WIN32_FIND_DATAA file_info;
    HANDLE search_handle = FindFirstFileA(buffer, &file_info);
    if (search_handle == NULL)
    {
        return;
    }
    bool res = true;
    while (res)
    {
        if (file_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if (strcmp(file_info.cFileName, ".") != 0 && strcmp(file_info.cFileName, "..") != 0)
            {
                printf("DIR : %s\n", file_info.cFileName);
            }
        } else {
            LARGE_INTEGER file_size;
            file_size.HighPart = file_info.nFileSizeHigh;
            file_size.LowPart = file_info.nFileSizeLow;
            SYSTEMTIME utc;
            FileTimeToSystemTime(&file_info.ftLastWriteTime, &utc);
            //printf("FILE: %s (%ld bytes) %02d/%02d/%d %02d:%02d\n",
            //    file_info.cFileName,
            //    file_size.QuadPart,
            //    utc.wMonth, utc.wDay, utc.wYear, utc.wHour, utc.wMinute);
            printf("FILE: %s (%lld bytes) %u/%02u/%02u %02u:%02u\n", file_info.cFileName, file_size.QuadPart, utc.wYear, utc.wMonth, utc.wDay, utc.wHour, utc.wMinute);
        }
        res = FindNextFile(search_handle, &file_info);
    }
    FindClose(search_handle);
    free(buffer);
}

void set_console_color(ConsoleColor cc)
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dw;
    if (cc == CONSOLE_GREEN)
    {
        SetConsoleTextAttribute(console, FOREGROUND_GREEN);
    }
    else if (cc == CONSOLE_RED)
    {
        SetConsoleTextAttribute(console, FOREGROUND_RED);
    }
    else if (cc == CONSOLE_BLUE)
    {
        SetConsoleTextAttribute(console, FOREGROUND_BLUE);
    }
    else if (cc = CONSOLE_WHITE)
    {
        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
}

void set_console_color_default()
{
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}
