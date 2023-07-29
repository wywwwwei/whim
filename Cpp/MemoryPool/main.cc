#include "CustomStack.h"
#include "MemoryPool.h"
#include <iostream>
#include <assert.h>

// The number of repeats
#define REPS 100
// The number of elements to be inserted
#define ELEMS 1000000

int main()
{
    clock_t start;

    // STL default allocator
    CustomStack<int, std::allocator<int>> defaultStack;
    start = clock();
    for (int j = 0; j < REPS; j++)
    {
        assert(defaultStack.empty());
        for (int i = 0; i < ELEMS; i++)
            defaultStack.push(i);
        for (int i = 0; i < ELEMS; i++)
            defaultStack.pop();
    }
    std::cout << "Default Allocator Time: ";
    std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n";

    // Custom memory pool
    CustomStack<int, MemoryPool<int>> customStack;
    start = clock();
    for (int j = 0; j < REPS; j++)
    {
        assert(customStack.empty());
        for (int i = 0; i < ELEMS; i++)
            customStack.push(i);
        for (int i = 0; i < ELEMS; i++)
            customStack.pop();
    }
    std::cout << "MemoryPool Allocator Time: ";
    std::cout << (((double)clock() - start) / CLOCKS_PER_SEC) << "\n";

    return 0;
}

// test result:
//
// REPS = 100 ELEMS = 100000
// Default Allocator Time: 1.309
// MemoryPool Allocator Time: 0.289
//
// REPS = 100 ELEMS = 1000000
// Default Allocator Time: 12.02
// MemoryPool Allocator Time: 2.639