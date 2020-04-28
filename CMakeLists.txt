cmake_minimum_required(VERSION 3.0)

# Compiler stuff
project(JuicyPixels)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Settings
option(SDL2STATIC "Build SDL2 statically" ON)
option(PROJECTS "Build projects." ON)

# Thirdparty and game projects
add_subdirectory(thirdparty)
include(thirdparty/bgfx.cmake/cmake/util/ConfigureDebugging.cmake)

if(PROJECTS)
	add_subdirectory(projects)
endif()

# Engine
file(GLOB ENGINE_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp") # Engine sources
add_library(JuicyPixels STATIC "${ENGINE_SOURCES}")
target_include_directories(JuicyPixels PUBLIC include) # Engine headers

# Engine Linking
if (SDL2STATIC)
	target_link_libraries( JuicyPixels PUBLIC bgfx glm spdlog SDL2-static) # sdl2 static may be changed
else()
	include_directories("${CMAKE_SOURCE_DIR}/thirdparty/SDL2/include")
	target_link_libraries( JuicyPixels PUBLIC bgfx glm spdlog SDL2)
endif()

