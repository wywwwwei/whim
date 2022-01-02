#include <cstddef>
#include <cstdint>

// Returns the smallest address >= the given address that is aligned to N bytes.
//
// N must be a power of two.
template <int N>
constexpr inline const uint8_t *RoundUp(const uint8_t *pointer)
{
    return reinterpret_cast<uint8_t *>(
        (reinterpret_cast<uintptr_t>(pointer) + (N - 1)) &
        ~static_cast<uintptr_t>(N - 1));
}