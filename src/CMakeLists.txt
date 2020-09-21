add_subdirectory(util)
add_subdirectory(rom)
add_subdirectory(cpu)
add_subdirectory(emulator)
add_subdirectory(interface)

add_executable(scout main.cpp)
target_link_libraries(scout PUBLIC interface)