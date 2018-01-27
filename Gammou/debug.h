#pragma once

//#ifndef NDEBUG

#ifdef _WIN32
#include <windows.h>
#undef min
#define DEBUG_PRINT(...) {char str[256];sprintf(str, __VA_ARGS__);OutputDebugStringA(str);}
#else
#include <iostream>
#define DEBUG_PRINT(...) std::fprintf(stderr, __VA_ARGS__)
#endif
//#else
//#define DEBUG_PRINT(...)
//#endif