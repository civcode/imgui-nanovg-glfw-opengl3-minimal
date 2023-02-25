cmake_minimum_required(VERSION 3.16)

project(imgui-nanovg)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmax-errors=5")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -fmax-errors=5")

find_package(glfw3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)

add_definitions(-DNANOVG_GL3)

if (WIN32)
    add_definitions(-DNANOVG_USE_GLEW)
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/dep/glfw-3.3.7/lib/cmake)
endif (WIN32)

include_directories(
    ${PROJECT_SOURCE_DIR}/dep/imgui
    ${PROJECT_SOURCE_DIR}/dep/imgui/backends
    ${PROJECT_SOURCE_DIR}/dep/nanovg/src
    ${PROJECT_SOURCE_DIR}/dep/nanovg/example
    ${PROJECT_SOURCE_DIR}/dep/stb
    ${PROJECT_SOURCE_DIR}/dep

    if (WIN32)
        ${PROJECT_SOURCE_DIR}/dep/glew-2.1.0/include
    endif (WIN32)
)

file(GLOB imgui_SRC
    ${PROJECT_SOURCE_DIR}/dep/imgui/*.h
    ${PROJECT_SOURCE_DIR}/dep/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/dep/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/dep/imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB nanovg_SRC
    //${PROJECT_SOURCE_DIR}/dep/nanovg/src/*.h
    ${PROJECT_SOURCE_DIR}/dep/nanovg/src/*.c
    ${PROJECT_SOURCE_DIR}/dep/nanovg/example/perf.c
)

set(SOURCES 
    ${imgui_SRC}
    ${nanovg_SRC}
)

if (WIN32)
    file(COPY ${CMAKE_SOURCE_DIR}/dep/glew-2.1.0/lib/glew32.dll DESTINATION ${CMAKE_SOURCE_DIR}/build)
endif (WIN32)

add_executable(imgui-nanovg src/main.cpp ${SOURCES} )

target_link_libraries(imgui-nanovg OpenGL::GL glfw ${CMAKE_DL_LIBS})
target_compile_features(imgui-nanovg PUBLIC cxx_std_17)
