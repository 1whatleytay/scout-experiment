#include <interface/interface.h>

#include <emulator/emulator.h>

#include <fmt/printf.h>

int Interface::exec() {
    if (input.empty()) {
        fmt::print("Missing input file.\n");
        return -1;
    }

    std::vector<uint8_t> data = loadFile(input);

    if (data.empty()) {
        fmt::print("Invalid input file.\n");
        return -1;
    }

    switch (mode) {
        case Mode::Launch: {
            Emulator(data).exec();
            break;
        }
        case Mode::Convert: {
            std::vector<u8> flipped = Rom::flipEndian<u16>(data);
            writeFile(output, flipped);
            break;
        }
    }

    return 0;
}

Interface::Interface(i32 count, char **args) {
    for (uint32_t a = 1; a < count; a++) {
        const char *arg = args[a];

        if (strcmp(arg, "-z") == 0) {
            if (a + 1 < count) {
                mode = Mode::Convert;
                output = args[a + 1];
                a++;
            } else {
                fmt::print("Missing output arg for -z.");
            }
        } else {
            input = arg;
        };
    }
}