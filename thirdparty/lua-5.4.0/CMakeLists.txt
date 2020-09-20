cmake_minimum_required(VERSION 3.0)

project(lua LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")

file(GLOB LUA_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")

add_library(lua STATIC "${LUA_SOURCE}")

target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
