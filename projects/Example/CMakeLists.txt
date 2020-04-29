set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Compile and copy vertex shaders
file(GLOB_RECURSE VERTEX_SHADERS "*.vert")
foreach(VS ${VERTEX_SHADERS})
    file(RELATIVE_PATH RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${VS})
    get_filename_component(RELATIVE ${RELATIVE} DIRECTORY)
    add_shader(${VS} VERTEX   OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/data/${RELATIVE}/shaders" DX11_MODEL 5_0 GLSL 130)
endforeach()

# Compile and copy fragment shaders
file(GLOB_RECURSE FRAGMENT_SHADERS "*.frag")
foreach(FS ${FRAGMENT_SHADERS})
    file(RELATIVE_PATH RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${FS})
    get_filename_component(RELATIVE ${RELATIVE} DIRECTORY)
    add_shader(${FS} FRAGMENT   OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/data/${RELATIVE}/shaders" DX11_MODEL 5_0 GLSL 130)
endforeach()

# Copy other resources
file(GLOB_RECURSE RESOURCEFILES "*.json" "*.png" "*.lua")
foreach(RF ${RESOURCEFILES})
    file(RELATIVE_PATH RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${RF})
    get_filename_component(RELATIVE ${RELATIVE} DIRECTORY)
    file(COPY ${RF} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/data/${RELATIVE}")
endforeach()


add_executable(example Example.cpp ${FRAGMENT_SHADERS} ${VERTEX_SHADERS})
target_link_libraries(example JuicyPixels)
set_target_properties(example PROPERTIES FOLDER "projects")


configure_debugging(example WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR})