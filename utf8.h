/*
  UTF-8 handling for Gust (Koei/Tecmo) PC games tools
  Copyright © 2019 VitaSmith

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DUMP_LEAKS _CrtDumpMemoryLeaks()
#else
#define DUMP_LEAKS
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>

#pragma once

#if defined(_WIN32)
#include <windows.h>
#define stat64 _stat64

static __inline char* utf16_to_utf8(const wchar_t* str16)
{
    char* str8 = NULL;
    int str8_size = 0;

    if (str16[0] == 0)
        return calloc(1, 1);

    if (((str8_size = str8_size = WideCharToMultiByte(CP_UTF8, 0, str16, -1, NULL, 0, NULL, NULL)) <= 1) ||
        ((str8 = (char*)calloc(str8_size, 1)) == NULL))
        return NULL;

    if (WideCharToMultiByte(CP_UTF8, 0, str16, -1, str8, str8_size, NULL, NULL) != str8_size) {
        free(str8);
        return NULL;
    }

    return str8;
}

static __inline wchar_t* utf8_to_utf16(const char* str8)
{
    wchar_t* str16 = NULL;
    int str16_size = 0;

    if (str8 == NULL)
        return NULL;

    if (str8[0] == 0)
        return calloc(1, sizeof(wchar_t));

    if (((str16_size = MultiByteToWideChar(CP_UTF8, 0, str8, -1, NULL, 0)) <= 1) ||
        ((str16 = (wchar_t*)calloc(str16_size, sizeof(wchar_t))) == NULL))
        return NULL;

    if (MultiByteToWideChar(CP_UTF8, 0, str8, -1, str16, str16_size) != str16_size) {
        free(str16);
        return NULL;
    }

    return str16;
}

static __inline FILE* fopen_utf8(const char* filename, const char* mode)
{
    FILE* r = NULL;
    wchar_t* filename16 = utf8_to_utf16(filename);
    wchar_t* mode16 = utf8_to_utf16(mode);
    _wfopen_s(&r, filename16, mode16);
    free(filename16);
    free(mode16);
    return r;
}

static __inline int stat64_utf8(const char* path, struct stat64* buffer)
{
    int r;
    wchar_t* path16 = utf8_to_utf16(path);
    r = _wstat64(path16, buffer);
    free(path16);
    return r;
}

#define CALL_MAIN int wmain(int argc, wchar_t** argv16) {   \
    SetConsoleOutputCP(CP_UTF8);                            \
    char** argv = calloc(argc, sizeof(char*));              \
    for (int i = 0; i < argc; i++)                          \
        argv[i] = utf16_to_utf8(argv16[i]);                 \
    int r = main_utf8(argc, argv);                          \
    for (int i = 0; i < argc; i++)                          \
        free(argv[i]);                                      \
    free(argv);                                             \
    DUMP_LEAKS;                                             \
    return r;                                               \
}
#else
#define fopen_utf8 fopen
#define stat64_utf8 stat64
#define CALL_MAIN int main(int argc, char** argv) {         \
    return main_utf8(argc, argv);                           \
}
#endif
