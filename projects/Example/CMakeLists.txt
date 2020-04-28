set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable( example Example.cpp f_simple.sc v_simple.sc )
target_include_directories( example PUBLIC include )
target_link_libraries( example JuicyPixels)
set_target_properties( example PROPERTIES FOLDER "projects" )

add_shader( v_simple.sc VERTEX   OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders DX11_MODEL 5_0 GLSL 130 )
add_shader( f_simple.sc FRAGMENT OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders DX11_MODEL 5_0 GLSL 130 )

configure_debugging( example WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR} )