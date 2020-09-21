#pragma once

#include <util/util.h>

class Interface {
    std::string input;
    std::string output;

    enum class Mode {
        Launch,
        Convert,
    };

    Mode mode = Mode::Launch;
public:
    int exec();

    Interface(int count, char **args);
};