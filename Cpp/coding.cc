// Code pratices learning from leveldb
// https://github.com/google/leveldb/blob/master/util/coding.h

#include <cstdint>
#include <cstring>

void EncodeFixed32(char *dst, uint32_t value)
{
    uint8_t *const buffer = reinterpret_cast<uint8_t *>(dst);
    buffer[0] = static_cast<uint8_t>(value);
    buffer[1] = static_cast<uint8_t>(value >> 8);
    buffer[2] = static_cast<uint8_t>(value >> 16);
    buffer[3] = static_cast<uint8_t>(value >> 24);
}

void EncodeFixed64(char *dst, uint64_t value)
{
    uint8_t *const buffer = reinterpret_cast<uint8_t *>(dst);
    buffer[0] = static_cast<uint8_t>(value);
    buffer[1] = static_cast<uint8_t>(value >> 8);
    buffer[2] = static_cast<uint8_t>(value >> 16);
    buffer[3] = static_cast<uint8_t>(value >> 24);
    buffer[4] = static_cast<uint8_t>(value >> 32);
    buffer[5] = static_cast<uint8_t>(value >> 40);
    buffer[6] = static_cast<uint8_t>(value >> 48);
    buffer[7] = static_cast<uint8_t>(value >> 56);
}

uint32_t DecodeFixed32(const char *ptr)
{
    const uint8_t *const buffer = reinterpret_cast<const uint8_t *>(ptr);
    return (static_cast<uint32_t>(buffer[0])) |
           (static_cast<uint32_t>(buffer[1]) << 8) |
           (static_cast<uint32_t>(buffer[2]) << 16) |
           (static_cast<uint32_t>(buffer[3]) << 24);
}

uint64_t DecodeFixed64(const char *ptr)
{
    const uint8_t *const buffer = reinterpret_cast<const uint8_t *>(ptr);
    return (static_cast<uint32_t>(buffer[0])) |
           (static_cast<uint32_t>(buffer[1]) << 8) |
           (static_cast<uint32_t>(buffer[2]) << 16) |
           (static_cast<uint32_t>(buffer[3]) << 24) |
           (static_cast<uint32_t>(buffer[4]) << 32) |
           (static_cast<uint32_t>(buffer[5]) << 40) |
           (static_cast<uint32_t>(buffer[6]) << 48) |
           (static_cast<uint32_t>(buffer[7]) << 56);
}

char *EncodeVarint32(char *dst, uint32_t value)
{
    uint8_t *ptr = reinterpret_cast<uint8_t *>(dst);
    static const int B = 128;
    if (v < (1 << 7))
    {
        *(ptr++) = v;
    }
    else if (v < (1 << 14))
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7);
    }
    else if (v < (1 << 21))
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14);
    }
    else if (v < (1 << 28))
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = v >> 21;
    }
    else
    {
        *(ptr++) = v | B;
        *(ptr++) = (v >> 7) | B;
        *(ptr++) = (v >> 14) | B;
        *(ptr++) = (v >> 21) | B;
        *(ptr++) = v >> 28;
    }
    return reinterpret_cast<char *>(ptr);
}

char *EncodeVarint64(char *dst, uint64_t v)
{
    static const int B = 128;
    uint8_t *ptr = reinterpret_cast<uint8_t *>(dst);
    while (v >= B)
    {
        *(ptr++) = v | B;
        v = v >> 7;
    }
    *(ptr++) = v;
    return reinterpret_cast<char *>(ptr);
}

const char *GetVarint32PtrFallback(const char *p, const char *limit, uint32_t *value)
{
    uint32_t result = 0;
    for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7)
    {
        uint32_t byte = *(static_cast<const uint8_t *>(p));
        p++;
        if (byte & 128)
        {
            result |= ((byte & 127) << shift);
        }
        else
        {
            result |= (byte << shift);
            *value = result;
            return reinterpret_cast<const char *>(p);
        }
    }
    return nullptr;
}

const char *GetVarint64Ptr(const char *p, const char *limit, uint64_t *value)
{
    uint64_t result = 0;
    for (uint32_t shift = 0; shift <= 63 && p < limit; shift += 7)
    {
        uint64_t byte = *(reinterpret_cast<const uint8_t *>(p));
        p++;
        if (byte & 128)
        {
            result |= ((byte & 127) << shift);
        }
        else
        {
            result |= (byte << shift);
            *value = result;
            return reinterpret_cast<const char *>(p);
        }
    }
    return nullptr;
}