#include <emulator/emulator.h>

void Emulator::exec() {
    Cpu cpu(rom);

    cpu.exec();
}

Emulator::Emulator(const std::vector<uint8_t> &data) : rom(data) { }