#ifndef OS_H
#define OS_H

#include <string.h>  // strcpy
#include <stdbool.h> // bool
#include <stdio.h>   // printf

#include <Windows.h>
#include <WinBase.h>
#include <fileapi.h>

typedef enum ConsoleColorEnum
{
    CONSOLE_RED,
    CONSOLE_GREEN,
    CONSOLE_BLUE,
    CONSOLE_WHITE,
    CONSOLE_YELLOW
} ConsoleColor;

void get_build_info();
char *get_current_dir();
void get_files(const char *dir_path);
bool is_dir(const wchar_t *dir_path);
bool in_dir(const wchar_t *dir_path, const wchar_t *filename);

void set_console_text_color(ConsoleColor cc);
void set_console_text_color_default();

void log_error(const char *msg);

#endif
