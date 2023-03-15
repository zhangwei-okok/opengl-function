cmake_minimum_required(VERSION 3.0)

project(Genbu02)
add_compile_options(-std=c++11)


SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin/)


# add header path
include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/include/assimp
        ${PROJECT_SOURCE_DIR}/glm-master
        ${PROJECT_SOURCE_DIR}/shader
)

if (APPLE)
    # for <GLFW/glfw3.h>
    include_directories(
            /usr/local/include
            /opt/local/include
            /opt/homebrew/include
            ${PROJECT_SOURCE_DIR}/include
    )
endif ()

# set common source
file(GLOB SRC
        ./src/*.c
        ./src/*.cpp
        ./src/GLSLProgram.cpp
        )

# set specific source and other option for platform
if (WIN32)
    file(GLOB PLATFORM_SRC


            )
elseif (UNIX)
    # support both mac and linux
    add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)


    file(GLOB PLATFORM_SRC
            ./imgui-master/examples/libs/glfw/include/GLFW/glfw3.*
            ./imgui-master/backends/imgui_impl_glfw.*
            ./imgui-master/backends/imgui_impl_opengl3.*

            )
endif ()

# add link path
if (APPLE)
    link_directories(
            /usr/local/lib
            /opt/local/lib
            /opt/homebrew/lib
            ${PROJECT_SOURCE_DIR}/lib
    )
elseif (WIN32)
    link_libraries(
            ${PROJECT_SOURCE_DIR}/lib
    )
endif ()
# generate binary
add_executable(vertex_texture ${SRC} ${PLATFORM_SRC} vertex_texture.cpp)
add_executable(single_vertex_texture_shader ${SRC} ${PLATFORM_SRC} single_vertex_texture_shader.cpp)
add_executable(blending ${SRC} ${PLATFORM_SRC} blending.cpp)
add_executable(logicop ${SRC} ${PLATFORM_SRC} logicop.cpp)
add_executable(template ${SRC} ${PLATFORM_SRC} template.cpp)
add_executable(basicPoints ${SRC} ${PLATFORM_SRC} basicPoints.cpp)
add_executable(depth_test ${SRC} ${PLATFORM_SRC} depth_test.cpp)
add_executable(feed_back ${SRC} ${PLATFORM_SRC} feed_back.cpp)
add_executable(complex ${SRC} ${PLATFORM_SRC} complex.cpp)
add_executable(multi_vertex_texture_shader ${SRC} ${PLATFORM_SRC} multi_vertex_texture_shader.cpp)
# link lib, should install glfw first or prebuild lib and embed in project
if (WIN32)
    target_link_libraries(vertex_texture
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(single_vertex_texture_shader
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(blending
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(logicop
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(template
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(basicPoints
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(depth_test
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(feed_back
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
    target_link_libraries(complex

            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            )
    target_link_libraries(multi_vertex_texture_shader
            ${PROJECT_SOURCE_DIR}/lib/libassimp.dll
            ${PROJECT_SOURCE_DIR}/lib/glfw3.dll)
elseif (APPLE)
    # mac: brew install glfw3
    find_library(OPENGL_LIBRARY OpenGL REQUIRED)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVID_LIBRARY CoreVideo REQUIRED)
    message(${COCOA_LIBRARY})
    message(${IOKIT_LIBRARY})
    message(${COREVID_LIBRARY})

    target_link_libraries(${PROJECT_NAME}
            ${OPENGL_LIBRARY}
            ${COCOA_LIBRARY}
            ${IOKIT_LIBRARY}
            ${COREVID_LIBRARY}

            glfw # use this lib name
            )
elseif (UNIX AND NOT APPLE)
    find_package(ASSIMP REQUIRED)
    # linux: sudo apt install libglfw3-dev
    target_link_libraries(basicPoints
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(blending
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(depth_test
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(logicop
            GL
            glfw # use this lib name
            dl
            )

    target_link_libraries(template
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(vertex_texture
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(single_vertex_texture_shader
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(feed_back
            GL
            glfw # use this lib name
            dl
            )
    target_link_libraries(complex
            GL
            glfw # use this lib name
            dl
            assimp
            )
    target_link_libraries(multi_vertex_texture_shader
            GL
            glfw # use this lib name
            dl
            )
endif ()

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
#set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
