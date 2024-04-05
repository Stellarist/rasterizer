cmake_minimum_required(VERSION 3.20)

project(rasterizer)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")

file(GLOB SRC_LIST src/*.cpp)
file(GLOB SHADER_LIST shaders/*)

find_package(OpenMP REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_executable(rasterizer ${SRC_LIST})

target_link_libraries(rasterizer
    glad::glad
    glfw
    yaml-cpp::yaml-cpp
    OpenMP::OpenMP_CXX
)
