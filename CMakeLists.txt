cmake_minimum_required(VERSION 3.2)

# Compiler stuff
project(JuicyEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SETTINGS
# Paths
set(RESOURCES_DIR "data" CACHE STRING "resources folder")
set(SHADERS_SUBDIR "shaders" CACHE STRING "shaders parent folder")
# API
option(BGFX_BUILD_TOOLS      "Build bgfx tools."                             ON) # required to build shaders
option(BGFX_BUILD_EXAMPLES   "Build bgfx examples."                          OFF)
option(BGFX_INSTALL          "Create installation target."                   OFF)
option(BGFX_INSTALL_EXAMPLES "Install examples and their runtimes."          OFF)
option(BGFX_CUSTOM_TARGETS   "Include convenience custom targets."           OFF)
option(BGFX_USE_OVR          "Build with OVR support."                       OFF)
option(BGFX_AMALGAMATED      "Amalgamated bgfx build for faster compilation" OFF)
option(BX_AMALGAMATED        "Amalgamated bx build for faster compilation"   OFF)
option(BGFX_CONFIG_DEBUG     "Enables debug configuration on all builds"     OFF)
option(BGFX_USE_DEBUG_SUFFIX "Add 'd' suffix to debug output targets"        OFF)
set(BGFX_OPENGL_VERSION  "" CACHE STRING "Specify minimum opengl version" )
# SDL2
option(SDL2STATIC "Build SDL2 statically" ON)

# Thirdparty and game projects
add_subdirectory(thirdparty)
include(thirdparty/bgfx.cmake/cmake/util/ConfigureDebugging.cmake)

# Engine
file(GLOB ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp") # Engine sources
add_library(JuicyEngine STATIC "${ENGINE_SOURCES}")
target_include_directories(JuicyEngine PUBLIC src) # Engine headers

# Preprocessor
target_compile_definitions(JuicyEngine PRIVATE RESOURCES_FOLDER="${RESOURCES_DIR}" SHADERS_PARTIAL_PATH="${SHADERS_SUBDIR}")

# Engine Linking
if (SDL2STATIC)
	target_link_libraries(JuicyEngine PUBLIC bgfx glm spdlog lua sol2 EnTT SDL2-static) # sdl2 static may be changed
else()
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/include")
	target_link_libraries(JuicyEngine PUBLIC bgfx glm spdlog lua sol2 EnTT SDL2)
endif()