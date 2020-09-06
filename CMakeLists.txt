cmake_minimum_required(VERSION 3.2)

# Compiler stuff
project(JuicyEngine)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

# SETTINGS
# Paths, do not edit
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
file(GLOB_RECURSE ENGINE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp") # Engine sources
add_library(JuicyEngine STATIC "${ENGINE_SOURCES}")
target_include_directories(JuicyEngine PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src") # Engine headers

# Asset compilation and packaging
macro(PACKAGE_ASSETS RESDIR)
    # Compile and copy vertex shaders
    file(GLOB_RECURSE VERTEX_SHADERS "${RESDIR}/*.vert")
    foreach(VS ${VERTEX_SHADERS})
        file(RELATIVE_PATH RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${RESDIR}" ${VS})
        get_filename_component(RELATIVE ${RELATIVE} DIRECTORY)
        add_shader(${VS} VERTEX   OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/data/${RELATIVE}/shaders" DX11_MODEL 5_0 GLSL 130)
    endforeach()
    # Compile and copy fragment shaders
    file(GLOB_RECURSE FRAGMENT_SHADERS "${RESDIR}/*.frag")
    foreach(FS ${FRAGMENT_SHADERS})
        file(RELATIVE_PATH RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${RESDIR}" ${FS})
        get_filename_component(RELATIVE ${RELATIVE} DIRECTORY)
        add_shader(${FS} FRAGMENT   OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/data/${RELATIVE}/shaders" DX11_MODEL 5_0 GLSL 130)
    endforeach()
    # Copy other resources
    set(EXCEPTIONS ${VERTEX_SHADERS} ${FRAGMENT_SHADERS})
    file(GLOB_RECURSE RESOURCEFILES "${RESDIR}/*")
    list(REMOVE_ITEM RESOURCEFILES EXCEPTIONS)
    foreach(RF ${RESOURCEFILES})
        file(RELATIVE_PATH RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}/${RESDIR}" ${RF})
        get_filename_component(RELATIVE ${RELATIVE} DIRECTORY)
        file(COPY ${RF} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data/${RELATIVE}")
    endforeach()
endmacro()


# Engine Linking
if (SDL2STATIC)
    target_link_libraries(JuicyEngine PUBLIC bgfx glm spdlog lua sol2 nlohmann_json assimp SDL2-static) # sdl2 static may be changed
else()
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/SDL2/include")
        target_link_libraries(JuicyEngine PUBLIC bgfx glm spdlog lua sol2 nlohmann_json assimp SDL2)
endif()
