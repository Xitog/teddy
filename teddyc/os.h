#ifndef OS_H
#define OS_H

#include <string.h> // strcpy
#include <stdbool.h> // bool
#include <stdio.h> // printf

#include <Windows.h>
#include <WinBase.h>
#include <fileapi.h>

typedef enum ConsoleColorEnum
{
    CONSOLE_RED,
    CONSOLE_GREEN,
    CONSOLE_BLUE,
    CONSOLE_WHITE
} ConsoleColor;

void get_build_info();
char * get_current_dir();
void get_files(const char * dir_path);

void set_console_color(ConsoleColor cc);
void set_console_color_default();

#endif
