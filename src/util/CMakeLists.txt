add_library(util STATIC
    include/util/util.h

    util.cpp)

target_include_directories(util PUBLIC include)
target_link_libraries(util PUBLIC fmt)