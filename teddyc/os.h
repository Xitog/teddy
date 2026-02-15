#ifndef OS_H
#define OS_H

#include <string.h> // strcpy
#include <stdbool.h> // bool
#include <stdio.h> // printf

#include <Windows.h>
#include <WinBase.h>
#include <fileapi.h>

void getBuildInfo();
char * getCurrentDir();
void getFiles(const char * dir_path);

#endif
