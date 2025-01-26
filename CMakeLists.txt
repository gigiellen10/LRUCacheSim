# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(PA1 LANGUAGES C)

# Add the executable target called pr
add_executable(p
    src/functions.c
    src/cachesim.c
)

# Set C standard (optional, recommended)
set_target_properties(p PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO
)

# Set build type to Debug (will be used if not overridden)
set(CMAKE_BUILD_TYPE Debug)

# Optional: Add additional flags for debugging
target_compile_options(p PRIVATE -g)  # Add debugging symbols
