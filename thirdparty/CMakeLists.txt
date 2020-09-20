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
add_subdirectory(lua-5.4.0)

# sol2 (sol v3)
add_subdirectory(sol2)

# nlohmann json
set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
set(JSON_MultipleHeaders ON CACHE INTERNAL "")
set(JSON_ImplicitConversions ON CACHE INTERNAL "")
add_subdirectory(json)

# Assimp
#set( ASSIMP_BUILD_TESTS  OFF CACHE INTERNAL "" )
#set( ASSIMP_INSTALL      OFF CACHE INTERNAL "" )
#set( ASSIMP_DOUBLE_PRECISION OFF CACHE INTERNAL "")
#add_subdirectory(assimp)
