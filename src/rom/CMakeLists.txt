add_library(rom STATIC
    include/rom/rom.h

    rom.cpp)

target_include_directories(rom PUBLIC include)
target_link_libraries(rom PUBLIC util)