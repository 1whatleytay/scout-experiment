add_library(emulator STATIC
    include/emulator/emulator.h

    emulator.cpp)

target_include_directories(emulator PUBLIC include)
target_link_libraries(emulator PUBLIC util rom cpu)