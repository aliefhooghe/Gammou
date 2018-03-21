#ifndef CAIRO_DEFINITION_H_
#define CAIRO_DEFINITION_H_

#define CAIRO_WIN32_STATIC_BUILD
#include <cairo.h>

#ifdef _WIN32
#include <cairo-win32.h>
#elif defined __linux__
#include <cairo-xlib.h>
#endif


#endif
