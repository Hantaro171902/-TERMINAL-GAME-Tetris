cmake_minimum_required(VERSION 3.10)

# Project name and version
project(TerminalHangman VERSION 1.0)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set source and header files
set(SOURCES
    src/main.cpp
    src/ultils.cpp
    src/tetris.cpp
    src/block.cpp
    src/color.cpp
)

set(HEADERS
    src/headers/tetris.hpp
    src/headers/block.hpp
    src/headers/color.hpp
    src/headers/ultils.hpp
)

# Executable target
add_executable(Tetris ${SOURCES} ${HEADERS})

target_include_directories(Tetris PRIVATE src/headers)

# Optional: copy asset folder into build dir
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
