add_library(cpu STATIC
    include/cpu/options.h
    include/cpu/memory.h
    include/cpu/cpu.h

    memory.cpp
    codes.cpp
    cpu.cpp)

target_include_directories(cpu PUBLIC include)
target_link_libraries(cpu PUBLIC rom)