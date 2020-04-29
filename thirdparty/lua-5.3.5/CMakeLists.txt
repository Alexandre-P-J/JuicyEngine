cmake_minimum_required(VERSION 3.0)

project(lua LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

file(GLOB LUA_SOURCE "src/*.c")
add_library(lua STATIC "${LUA_SOURCE}")
include_directories("src")