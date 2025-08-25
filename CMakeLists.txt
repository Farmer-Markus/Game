cmake_minimum_required(VERSION 3.6)

project(Game)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_subdirectory(external)

add_executable(game main.cpp)
target_link_libraries(game SDL3::SDL3)