#pragma once

#include <util/util.h>

class Header {
public:
    Number<u32> magic; // might have register information
    Number<u32> clockRate;
    Number<u32> pc;
    Number<u32> releaseAddress;
    Number<u64> checksum;
    Number<u64> unknown1;
    char name[20] = "";
    Number<u32> unknown2;
    Number<u32> format;
    char cartrigeId[2] = "";
    char countryCode;
    Number<u8> version;
    u32 bootstrap[1008];
};

class Rom {
public:
    Header header;
    std::vector<u8> data;

    template <typename T>
    static std::vector<u8> flipEndian(const std::vector<u8> &data) {
        constexpr u32 unit = sizeof(T);

        std::vector<u8> result(data.size());

        assert(data.size() % unit == 0);

        for (uint32_t a = 0; a < data.size() / unit; a++) {
            // retrive
            T val = *reinterpret_cast<const T *>(&data[a * unit]);
            // write
            *reinterpret_cast<T *>(&result[a * unit]) = swap<u16>(val);
        }

        return result;
    }

    explicit Rom(std::vector<u8> data);
};