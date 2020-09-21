add_library(interface STATIC
    include/interface/interface.h

    interface.cpp)

target_include_directories(interface PUBLIC include)
target_link_libraries(interface PUBLIC emulator)