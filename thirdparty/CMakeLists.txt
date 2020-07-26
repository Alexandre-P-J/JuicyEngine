# bgfx
set( BGFX_BUILD_TOOLS     ON  CACHE INTERNAL "" )
set( BGFX_BUILD_EXAMPLES  OFF CACHE INTERNAL "" )
set( BGFX_CUSTOM_TARGETS  OFF CACHE INTERNAL "" )
add_subdirectory( bgfx.cmake )

# glm
set( GLM_TEST_ENABLE      OFF CACHE INTERNAL "" )
set( BUILD_SHARED_LIBS    OFF CACHE INTERNAL "" )
set( BUILD_STATIC_LIBS    OFF CACHE INTERNAL "" )
add_subdirectory( glm )

# SDL2
if (SDL2STATIC)
    add_subdirectory(SDL2)
endif()

# spdlog
add_subdirectory(spdlog)

# lua
add_subdirectory(lua-5.3.5)

# sol2 (sol v3)
add_subdirectory(sol2)

# EnTT
add_subdirectory(EnTT)

# cereal
option(SKIP_PORTABILITY_TEST "Skip portability (32 bit) tests" ON)
option(SKIP_PERFORMANCE_COMPARISON "Skip building performance comparison (requires boost)" ON)
option(JUST_INSTALL_CEREAL "Don't do anything besides installing the library" ON)
add_subdirectory(cereal)
