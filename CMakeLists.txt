cmake_minimum_required(VERSION 3.10)

project(Minecraft)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LINK_STATIC "true to link static, false to link shared" FALSE)

add_subdirectory(external)

add_subdirectory(Minecraft)
