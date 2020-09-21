#include <cpu/memory.h>

#include <fmt/printf.h>

bool MemoryRegion::supports(Intention intention) const {
    switch (type) {
        case Type::Empty: return false;
        case Type::Dummy:
        case Type::Mirror:
        case Type::ReadWriteData:
        case Type::ReadWriteDevice: return true;
        case Type::ReadOnlyData:
        case Type::ReadOnlyDevice: return intention == Intention::Read;
        case Type::WriteOnlyDevice: return intention == Intention::Write;
        default: return false;
    }
}

MemoryRegion::MemoryRegion() : start(0), size(0), type(Type::Empty) { }
MemoryRegion::MemoryRegion(u32 start, u32 size) : start(start), size(size), type(Type::Dummy) { }
MemoryRegion::MemoryRegion(u32 start, u32 size, void *data)
    : start(start), size(size), type(Type::ReadWriteData), data(reinterpret_cast<u8 *>(data)) { }
MemoryRegion::MemoryRegion(u32 start, u32 size, const void *data)
    : start(start), size(size), type(Type::ReadOnlyData), data(reinterpret_cast<u8 *>(const_cast<void *>(data))) { }
MemoryRegion::MemoryRegion(u32 start, u32 size, MemoryRead device)
    : start(start), size(size), type(Type::ReadOnlyDevice), readDevice(std::move(device)) { }
MemoryRegion::MemoryRegion(u32 start, u32 size, MemoryWrite device)
    : start(start), size(size), type(Type::WriteOnlyDevice), writeDevice(std::move(device)) { }
MemoryRegion::MemoryRegion(u32 start, u32 size, MemoryRead read, MemoryWrite write)
    : start(start), size(size), type(Type::ReadWriteDevice),
    readDevice(std::move(read)), writeDevice(std::move(write)) { }
MemoryRegion::MemoryRegion(u32 start, u32 size, u32 mirrorStart)
    : start(start), size(size), type(Type::Mirror), mirrorStart(mirrorStart) { }

u8 unimplementedRead(u32 address) {
    fmt::print("Unimplemented GET 0x{:0>8x}\n", address);

    return 0;
}

void unimplementedWrite(u32 address, u8 value) {
    fmt::print("Unimplemented SET 0x{:0>8x} = 0x{:0>2x}\n", address, value);
}

MemoryRegion Memory::findRegion(u32 address, MemoryRegion::Intention intention) {
    for (const MemoryRegion &region : regions) {
        if (address >= region.start && address < region.start + region.size && region.supports(intention)) {
            return region;
        }
    }

    return { };
}

u8 Memory::getByte(u32 address) {
    MemoryRegion region = findRegion(address, MemoryRegion::Intention::Read);
    u32 subAddress = address;

    while (region.type == MemoryRegion::Type::Mirror) {
        subAddress += region.mirrorStart - region.start;
        region = findRegion(subAddress, MemoryRegion::Intention::Read);
    }

    assert(region.type != MemoryRegion::Type::Empty);

    switch (region.type) {
        case MemoryRegion::Type::ReadOnlyData:
        case MemoryRegion::Type::ReadWriteData:
            return region.data[subAddress - region.start];
        case MemoryRegion::Type::ReadOnlyDevice:
        case MemoryRegion::Type::ReadWriteDevice:
            return region.readDevice(subAddress);
        case MemoryRegion::Type::Dummy:
            return unimplementedRead(address);
        default:
            assert(false);
    }

    return 0;
}

void Memory::setByte(u32 address, u8 value) {
    MemoryRegion region = findRegion(address, MemoryRegion::Intention::Write);
    u32 subAddress = address;

    while (region.type == MemoryRegion::Type::Mirror) {
        subAddress += region.mirrorStart - region.start;
        region = findRegion(subAddress, MemoryRegion::Intention::Write);
    }

    assert(region.type != MemoryRegion::Type::Empty);

    switch (region.type) {
        case MemoryRegion::Type::ReadWriteData:
            region.data[subAddress - region.start] = value;
            break;
        case MemoryRegion::Type::WriteOnlyDevice:
        case MemoryRegion::Type::ReadWriteDevice:
            region.writeDevice(subAddress, value);
            break;
        case MemoryRegion::Type::Dummy:
            unimplementedWrite(address, value);
            break;
        default:
            assert(false);
    }
}

Memory::Memory(const Rom &rom) : rom(rom), ram(mb(4)), spMemory(kb(8)) {
    std::memcpy(spMemory.data(), &rom.header, sizeof(Header));

    regions = {
        MemoryRegion(0x00000000, ram.size(), ram.data()),
        MemoryRegion(0x04000000, spMemory.size(), spMemory.data()),
        MemoryRegion(0x03F00000, sizeof(RamRegisters), &ramRegisters),
        MemoryRegion(0x03F00000 + sizeof(RamRegisters), 0x00100000 - sizeof(RamRegisters)),
        MemoryRegion(0x04040000, sizeof(SignalRegisters), &signalRegisters),
        MemoryRegion(0x04300000, sizeof(MipsInterface), &mipsInterface),
        MemoryRegion(0x04700000, sizeof(RamInterface), &ramInterface),
        MemoryRegion(0x10000000, sizeof(Header), &rom.header),
        MemoryRegion(0x04600000, sizeof(ParallelInterface), &parallelInterface),
        MemoryRegion(0x80000000, 0x20000000, u32(0x00000000)),
        MemoryRegion(0xA0000000, 0x20000000, u32(0x00000000)),
    };
}