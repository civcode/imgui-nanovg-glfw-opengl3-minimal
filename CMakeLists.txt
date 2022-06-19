cmake_minimum_required(VERSION 3.16)

project(imgui-nanovg)

set(CMAKE_BUILD_TYPE debug)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmax-errors=5")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors -fmax-errors=5")

find_package(glfw3 REQUIRED CONFIG)
find_package(OpenGL REQUIRED)
find_package(Eigen3 3.3 REQUIRED NO_MODULE)

add_definitions(-DNANOVG_GL3)

include_directories(
    ${PROJECT_SOURCE_DIR}/dep/imgui
    ${PROJECT_SOURCE_DIR}/dep/imgui/backends
    ${PROJECT_SOURCE_DIR}/dep/nanovg/src
    ${PROJECT_SOURCE_DIR}/dep/nanovg/example
    ${PROJECT_SOURCE_DIR}/dep/stb
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/dep/SimpleTimer/include
)

file(GLOB imgui_SRC
    ${PROJECT_SOURCE_DIR}/dep/imgui/*.h
    ${PROJECT_SOURCE_DIR}/dep/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/dep/imgui/backends/imgui_impl_glfw.cpp
    ${PROJECT_SOURCE_DIR}/dep/imgui/backends/imgui_impl_opengl3.cpp
)

file(GLOB nanovg_SRC
    ${PROJECT_SOURCE_DIR}/dep/nanovg/src/*.h
    ${PROJECT_SOURCE_DIR}/dep/nanovg/src/*.c
    ${PROJECT_SOURCE_DIR}/dep/nanovg/example/perf.c
)

set(SOURCES 
    ${imgui_SRC}
    ${nanovg_SRC}
    ${PROJECT_SOURCE_DIR}/src/DrawTest.cpp
    ${PROJECT_SOURCE_DIR}/dep/SimpleTimer/src/SimpleTimer.cpp

)

add_executable(imgui-nanovg src/main.cpp ${SOURCES} )

target_link_libraries(imgui-nanovg OpenGL::GL glfw ${CMAKE_DL_LIBS} Eigen3::Eigen)

