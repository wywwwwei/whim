#include <cstdio>
#include <cassert>
#include <cstring>
#include <mutex>

#include "Detector.h"

#ifdef new
#undef new
#endif
#ifdef DEBUG_NEW
#undef DEBUG_NEW
#endif

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

static std::size_t total_mem_alloc = 0;
// the lock used to visit linklist
static std::mutex new_ptr_lock;
// the lock used for output
static std::mutex output_lock;

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

static void print_caller_position(const new_ptr_list_t *ptr)
{
    if (ptr->line != 0)
    {
        print_position(ptr->filename, ptr->line);
    }
    else
    {
        print_position(ptr->caller_address, ptr->line);
    }
}

static void *alloc_mem(std::size_t size, const char *file, int line, bool is_array)
{
    assert(line > 0);

    size_t require_size = size + ALIGNED_LIST_ITEM_SIZE;
    new_ptr_list_t *ptr = (new_ptr_list_t *)malloc(require_size);
    if (ptr == nullptr)
    {
        std::unique_lock<std::mutex> lock(output_lock);
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
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        ptr->prev = new_ptr_list.prev;
        ptr->next = &new_ptr_list;
        new_ptr_list.prev->next = ptr;
        new_ptr_list.prev = ptr;
        total_mem_alloc += size;
    }
    void *usr_ptr = (char *)ptr + ALIGNED_LIST_ITEM_SIZE;
    {
        std::unique_lock<std::mutex> lock(output_lock);
        printf("new%s: allocated %p (size %zu, ", is_array ? "[]" : "", usr_ptr, size);
        print_caller_position(ptr);
        printf(")\n");
    }
    return usr_ptr;
}

static void free_pointer(void *usr_ptr, void *addr, bool is_array)
{
    if (usr_ptr == nullptr)
    {
        return;
    }
    new_ptr_list_t *ptr = reinterpret_cast<new_ptr_list_t *>(static_cast<char *>(usr_ptr) - ALIGNED_LIST_ITEM_SIZE);
    if (ptr == nullptr || ptr->magic != DEBUG_NEW_MAGIC)
    {
        std::unique_lock<std::mutex> lock(output_lock);
        printf("delete%s: invalid pointer %p (", is_array ? "[]" : "", usr_ptr);
        print_position(addr, 0); // print delete caller addr
        printf("\n");
        return;
    }
    if (is_array != ptr->is_array)
    {
        const char *msg;
        if (is_array)
        {
            msg = "delete[] after new";
        }
        else
        {
            msg = "delete after new[]";
        }
        std::unique_lock<std::mutex> lock(output_lock);
        printf("%s: pointer %p (size %zu)\n\tat ", msg, usr_ptr, ptr->size);
        print_position(addr, 0); // print delete caller addr
        print_caller_position(ptr);
        printf("\n");
    }
    {
        std::unique_lock<std::mutex> lock(new_ptr_lock);
        total_mem_alloc -= ptr->size;
        ptr->magic = 0;
        ptr->prev->next = ptr->next;
        ptr->next->prev = ptr->prev;
    }
    free(ptr);
}

int check_leaks()
{
    int leak_count = 0;
    std::unique_lock<std::mutex> lock(output_lock);
    std::unique_lock<std::mutex> lock(new_ptr_lock);
    new_ptr_list_t *ptr = new_ptr_list.next;
    while (ptr != &new_ptr_list)
    {
        const char *usr_ptr = reinterpret_cast<const char *>(ptr) + ALIGNED_LIST_ITEM_SIZE;
        if (ptr->magic != DEBUG_NEW_MAGIC)
        {
            printf("warning: heap data corrupt near %p\n", usr_ptr);
        }
        printf("Leaked object at %p (size %lu, ", usr_ptr, (unsigned long)ptr->size);
        print_caller_position(ptr);
        printf(")\n");
        ptr = ptr->next;
        ++leak_count;
    }
    return leak_count;
}

void *operator new(std::size_t size, const char *file, int line)
{
    void *ptr = alloc_mem(size, file, line, false);
    return ptr;
}
void *operator new[](std::size_t size, const char *file, int line)
{
    void *ptr = alloc_mem(size, file, line, true);
    return ptr;
}
void *operator new(std::size_t size) { return operator new(size, (char *)_DEBUG_NEW_CALLER_ADDRESS, 0); }
void *operator new[](std::size_t size) { return operator new[](size, (char *)_DEBUG_NEW_CALLER_ADDRESS, 0); }
void operator delete(void *ptr) noexcept { free_pointer(ptr, _DEBUG_NEW_CALLER_ADDRESS, false); }
void operator delete[](void *ptr) noexcept { free_pointer(ptr, _DEBUG_NEW_CALLER_ADDRESS, true); }