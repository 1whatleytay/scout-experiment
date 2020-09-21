#pragma once

#include <string>
#include <vector>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef size_t ssi;

typedef float f32;
typedef double f64;

constexpr ssi kb(ssi value) { return value * 1024; }
constexpr ssi mb(ssi value) { return kb(value) * 1024; }
constexpr ssi gb(ssi value) { return mb(value) * 1024; }

std::vector<u8> loadFile(const std::string &path);
void writeFile(const std::string &path, const std::vector<u8> &data);

template <typename T>
T swap(T input) {
    T result = 0;

    for (uint32_t a = 0; a < sizeof(T); a++) {
        result <<= 8;
        // extract next byte
        u8 shift = (input >> a * 8) & 0xFF;
        // write to next spot
        result |= shift;
    }

    return result;
}

template <typename T>
T shift(T input, u32 start, u32 count) {
    constexpr T mask = ~T(0);

    return (input >> start) & (mask >> (sizeof(T) * 8 - count));
}

template <typename T>
class Number {
    T value = 0;
public:
    T get() {
        return swap(value);
    }

    operator T() {
        return get();
    }

    Number() = default;
    Number(T value) : value(value) { }
};

class Scanner {
    const u8 *data;

public:
    template <typename T>
    T pop() {
        T value = *reinterpret_cast<const T *>(data);
        data += sizeof(T);
        return value;
    }

    template <typename T>
    void pop(T *out, ssi size) {
        std::memcpy(out, data, size);
        data += size;
    }

    explicit Scanner(const u8 *data);
};
