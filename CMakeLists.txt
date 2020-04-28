cmake_minimum_required(VERSION 3.0)
project(JuicyPixels)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(SDL2STATIC "Build SDL2 statically" ON)

add_subdirectory(thirdparty)
include(thirdparty/bgfx.cmake/cmake/util/ConfigureDebugging.cmake)


file(GLOB ENGINE_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp") # Engine sources
add_library(JuicyPixels STATIC "${ENGINE_SOURCES}")
target_include_directories(JuicyPixels PUBLIC include) # Engine headers

if (SDL2STATIC)
	target_link_libraries( JuicyPixels PUBLIC bgfx glm spdlog SDL2-static) # sdl2 static may be changed
else()
	include_directories("${CMAKE_SOURCE_DIR}/thirdparty/SDL2/include")
	target_link_libraries( JuicyPixels PUBLIC bgfx glm spdlog SDL2)
endif()

option(PROJECTS "Build projects." ON)
if(PROJECTS)
	add_subdirectory(projects)
endif()