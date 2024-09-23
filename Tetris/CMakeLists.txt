cmake_minimum_required(VERSION 3.30)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)

# Add the executable
add_executable(Tetris MACOSX_BUNDLE src/main.cpp)

# Include directories for your project
target_include_directories(Tetris PUBLIC include/)

# Include glad library 
add_library(glad "include/glad/glad.c")  

# Find and link GLFW
find_package(GLFW3 REQUIRED)

# Link GLFW and necessary macOS frameworks
target_link_libraries(Tetris PRIVATE glfw glad)
target_link_libraries(Tetris PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit" "-framework CoreVideo")

