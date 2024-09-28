cmake_minimum_required(VERSION 3.30)
project(DOOM)

set(CMAKE_CXX_STANDARD 17)

# Add the executable
add_executable(DOOM MACOSX_BUNDLE src/main.cpp)

# Include directories for your project
include_directories("${CMAKE_SOURCE_DIR}/include")
include_directories("${CMAKE_SOURCE_DIR}/shaders")
include_directories("${CMAKE_SOURCE_DIR}/textures")

target_include_directories(DOOM PUBLIC include/)

# Include glad library 
add_library(glad "lib/glad/glad.c")  

# Find and link GLFW
find_package(GLFW3 REQUIRED)

# Link GLFW and necessary macOS frameworks
target_link_libraries(DOOM PRIVATE glfw glad)
target_link_libraries(DOOM PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit" "-framework CoreVideo")

