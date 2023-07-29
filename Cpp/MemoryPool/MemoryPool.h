#pragma once

#include <cstddef>
#include <new>
#include <utility>

template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
public:
    template <typename U>
    struct rebind
    {
        using other = MemoryPool<U>;
    };

    MemoryPool() noexcept;
    ~MemoryPool() noexcept;

    T *allocate(size_t n = 1, const T *hint = 0);
    void deallocate(T *p, size_t n = 1);

    template <typename U, typename... Args>
    void construct(U *p, Args &&...args);

    template <typename U>
    void destroy(U *p);

private:
    union Slot
    {
        T element;
        Slot *next;
    };

    // Current memory block
    Slot *currentBlock_;

    // Next free slot in current block
    Slot *currentSlot_;

    // Last slot in current block
    Slot *lastSlot_;

    // Recycled free slots
    Slot *freeSlots_;

    // Verify block size
    static_assert(BlockSize >= 2 * sizeof(Slot), "BlockSize too small.");
};

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool() noexcept
    : currentBlock_(nullptr), currentSlot_(nullptr), lastSlot_(nullptr), freeSlots_(nullptr)
{
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool() noexcept
{
    Slot *block = currentBlock_;
    while (block != nullptr)
    {
        Slot *nextBlock = block->next;
        operator delete(reinterpret_cast<void *>(block));
        block = nextBlock;
    }
}

template <typename T, size_t BlockSize>
T *MemoryPool<T, BlockSize>::allocate(size_t n, const T *hint)
{
    if (freeSlots_ != nullptr)
    {
        Slot *slot = freeSlots_;
        freeSlots_ = slot->next;
        return reinterpret_cast<T *>(slot);
    }

    if (currentSlot_ >= lastSlot_)
    {
        char *newBlock = reinterpret_cast<char *>(operator new(BlockSize));
        reinterpret_cast<Slot *>(newBlock)->next = currentBlock_;
        currentBlock_ = reinterpret_cast<Slot *>(newBlock);

        // Block is consist of a Slot pointer (point to last Block)
        // and a lot of free slots;
        char *body = newBlock + sizeof(Slot *);
        uintptr_t result = reinterpret_cast<uintptr_t>(body);
        size_t bodyPadding = (alignof(Slot) - result) % alignof(Slot);
        currentSlot_ = reinterpret_cast<Slot *>(body + bodyPadding);

        // https://www.lanqiao.cn/questions/5475/
        // last slot |<---newBlock + BlockSize - sizeof(Slot)   end---->|
        // so when current slot >= last slot + 1, need to alloc new block
        lastSlot_ = reinterpret_cast<Slot *>(newBlock + BlockSize - sizeof(Slot) + 1);
    }

    return reinterpret_cast<T *>(currentSlot_++);
}

template <typename T, size_t BlockSize>
void MemoryPool<T, BlockSize>::deallocate(T *p, size_t n)
{
    if (p != nullptr)
    {
        reinterpret_cast<Slot *>(p)->next = freeSlots_;
        freeSlots_ = reinterpret_cast<Slot *>(p);
    }
}

template <typename T, size_t BlockSize>
template <typename U, typename... Args>
void MemoryPool<T, BlockSize>::construct(U *p, Args &&...args)
{
    new (p) U(std::forward<Args>(args)...);
}

template <typename T, size_t BlockSize>
template <typename U>
void MemoryPool<T, BlockSize>::destroy(U *p)
{
    p->~U();
}