cmake_minimum_required(VERSION 3.10)
project(problem_015)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add executable
add_executable(code main.cpp)

# Set output name to 'code' explicitly
set_target_properties(code PROPERTIES OUTPUT_NAME "code")

# Enable all warnings (but don't treat as errors to be more compatible)
target_compile_options(code PRIVATE -Wall -Wextra -Wpedantic)

# Optimize for performance
target_compile_options(code PRIVATE -O2 -DNDEBUG)