#include <cstdio>
#include <cassert>
#include <cstring>

#include "Detector.h"

#ifndef _DEBUG_NEW_ALIGNMENT
#define _DEBUG_NEW_ALIGNMENT 16
#endif

#define ALIGN(n) (((n) + _DEBUG_NEW_ALIGNMENT - 1) & ~(_DEBUG_NEW_ALIGNMENT - 1))

// The caller address can be converted to a source position
// by atos (on MacOS) or addr2line (on Linux)
#ifndef _DEBUG_NEW_CALLER_ADDRESS
#ifdef __GNUC__
#define _DEBUG_NEW_CALLER_ADDRESS __builtin_return_address(0)
#else
#define _DEBUG_NEW_CALLER_ADDRESS nullptr
#endif
#endif

// why 44? Makes sizeof(new_ptr_list_t) = 64 on non-Windows 32-bit platforms
#ifndef _DEBUG_NEW_FILENAME_LEN
#define _DEBUG_NEW_FILENAME_LEN 44
#endif

struct new_ptr_list_t
{
    new_ptr_list_t *prev;
    new_ptr_list_t *next;
    std::size_t size;
    union
    {
        char filename[_DEBUG_NEW_FILENAME_LEN]; // filename of caller
        void *caller_address;                   // address of caller
    };
    unsigned line : 31;    // line number of the caller
    unsigned is_array : 1; // non-zero iff new[] is used
    unsigned magic;        // magic number for error detection
};
static const unsigned DEBUG_NEW_MAGIC = 0x4442474E;
static const size_t ALIGNED_LIST_ITEM_SIZE = ALIGN(sizeof(new_ptr_list_t));

static new_ptr_list_t new_ptr_list = {&new_ptr_list, &new_ptr_list, 0, {.filename = ""}, 0, 0, DEBUG_NEW_MAGIC};

static void print_position(const void *ptr, int line)
{
    if (line != 0) // file/line information exists
    {
        printf("In file %s:%d", ptr, line);
    }
    else if (ptr != nullptr) // caller address exists
    {
        printf("At address %p", ptr);
    }
    else // no infomation exists
    {
        printf("<Unknown>");
    }
}

static void *alloc_mem(std::size_t size, const char *file, int line, bool is_array)
{
    assert(line > 0);

    size_t require_size = size + ALIGNED_LIST_ITEM_SIZE;
    new_ptr_list_t *ptr = (new_ptr_list_t *)malloc(require_size);
    if (ptr == nullptr)
    {
        printf("Out of memory when allocating %lu bytes\n", (unsigned long)size);
        abort();
    }

    if (line > 0)
    {
        strncpy(ptr->filename, file, _DEBUG_NEW_FILENAME_LEN - 1);
        ptr->filename[_DEBUG_NEW_FILENAME_LEN - 1] = '\0';
    }
    else
    {
        ptr->caller_address = (void *)file;
    }

    ptr->line = line;
    ptr->is_array = is_array;
    ptr->size = size;
    ptr->magic = DEBUG_NEW_MAGIC;

    
}