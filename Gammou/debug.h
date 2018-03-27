#pragma once

#define TESTING

#if !defined(NDEBUG) || defined(TESTING)

#ifdef _WIN32
#include <windows.h>
#undef min
#undef max
#define DEBUG_PRINT(...) {char str[256];snprintf(str, 256, __VA_ARGS__);OutputDebugStringA(str);}
#else
#include <stdio.h>
#define DEBUG_PRINT(...) std::fprintf(stderr, __VA_ARGS__)
#endif
#else
#define DEBUG_PRINT(...) ((void)__VA_ARGS__)
#endif
