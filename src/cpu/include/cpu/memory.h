#pragma once

#include <rom/rom.h>
#include <cpu/options.h>

#include <functional>

typedef std::function<u8(u32)> MemoryRead;
typedef std::function<void(u32, u8)> MemoryWrite;

class MemoryRegion {
public:
    u32 start;
    u32 size;

    enum class Intention {
        Read,
        Write,
    };

    enum class Type {
        Empty,
        Dummy,
        ReadWriteData,
        ReadOnlyData,
        ReadWriteDevice,
        ReadOnlyDevice,
        WriteOnlyDevice,
        Mirror,
    };

    Type type;

    u8 *data = nullptr;
    MemoryRead readDevice;
    MemoryWrite writeDevice;
    u32 mirrorStart = 0;

    bool supports(Intention intention) const;

    MemoryRegion();
    MemoryRegion(u32 start, u32 size);
    MemoryRegion(u32 start, u32 size, void *data);
    MemoryRegion(u32 start, u32 size, const void *data);
    MemoryRegion(u32 start, u32 size, MemoryRead device);
    MemoryRegion(u32 start, u32 size, MemoryWrite device);
    MemoryRegion(u32 start, u32 size, MemoryRead read, MemoryWrite write);
    MemoryRegion(u32 start, u32 size, u32 mirrorStart);
};

u8 unimplementedRead(u32 address);
void unimplementedWrite(u32 address, u8 value);

class Memory {
    std::vector<MemoryRegion> regions;
    MemoryRegion findRegion(u32 address, MemoryRegion::Intention intention);

    std::vector<u8> ram;
    std::vector<u8> spMemory;

    MipsInterface mipsInterface;
    RamRegisters ramRegisters;
    RamInterface ramInterface;
    SignalRegisters signalRegisters;
    ParallelInterface parallelInterface;

public:
    u8 getByte(u32 address);
    void setByte(u32 address, u8 value);

    template <typename T>
    T get(u32 address) {
        constexpr ssi size = sizeof(T);

        T result = 0;

        for (ssi a = 0; a < size; a++) {
            result <<= 8;
            result |= getByte(address + a);
        }

        // might need swap
        return result;
    }

    template <typename T>
    void set(u32 address, T value) {
        constexpr ssi size = sizeof(T);

        for (ssi a = 0; a < size; a++) {
            setByte(address + a, (value >> ((size - a - 1) * 8)) & 0xFF);
        }
    }

    const Rom &rom;

    explicit Memory(const Rom &rom);
};