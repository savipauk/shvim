cmake_minimum_required(VERSION 3.22.1)

project(shvim)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Generates compile_commands.json in build/ 
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find SDL2
find_package(SDL2 REQUIRED)

# Find SDL2_ttf
find_package(SDL2_ttf REQUIRED)

# Find OpenGL
find_package(OpenGL REQUIRED)

# ImGui
add_library(ImGui STATIC
    third_party/imgui/imgui.cpp
    third_party/imgui/imgui_demo.cpp
    third_party/imgui/imgui_draw.cpp
    third_party/imgui/imgui_tables.cpp
    third_party/imgui/imgui_widgets.cpp
    third_party/imgui/backends/imgui_impl_sdl2.cpp 
    third_party/imgui/backends/imgui_impl_opengl3.cpp
    third_party/imgui/misc/cpp/imgui_stdlib.cpp
)
target_include_directories(ImGui PUBLIC
    third_party/imgui
    third_party/imgui/backends
    third_party/imgui/misc/cpp
)
target_link_libraries(ImGui PUBLIC SDL2::SDL2 OpenGL::GL)

# Source files
file(GLOB_RECURSE SRC_FILES src/*.cc)

add_executable(shvim ${SRC_FILES}) 
target_link_libraries(shvim SDL2::SDL2 ImGui SDL2_ttf::SDL2_ttf OpenGL::GL)
