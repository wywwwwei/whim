// Refrence Link: 
// A Cross-Platform Memory Leak Detector  http://wyw.dcweb.cn/leakage.htm
// adah1972/nvwa  https://github.com/adah1972/nvwa/blob/master/nvwa/debug_new.cpp
#pragma once

#define _DEBUG 1

#include <cstddef>

void *operator new(std::size_t size, const char *file, int line);
void *operator new[](std::size_t size, const char *file, int line);

// Debug_new https://en.wikipedia.org/wiki/Debug_new
// Replacing the keyword with a macro is
// actually not condoned by the standard.
//
// Must put after the declaration of operator new.

#ifdef _DEBUG
#define DEBUG_NEW new (__FILE__, __LINE__)
#define new DEBUG_NEW
#endif

int check_leaks();