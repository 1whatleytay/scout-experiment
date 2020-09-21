#pragma once

#include <util/util.h>

#include <rom/rom.h>
#include <cpu/cpu.h>

class Emulator {
    Rom rom;

public:
    void exec();

    explicit Emulator(const std::vector<uint8_t> &data);
};