cmake_minimum_required(VERSION 3.23)

project(rasterizer)

set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")

find_package(OpenMP REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

file(GLOB SRC_LIST src/*.cpp)
file(GLOB SHADER_LIST shaders/*)

add_executable(rasterizer)

target_sources(rasterizer 
PUBLIC
    ${SRC_LIST}
)

target_link_libraries(rasterizer
    glad::glad
    glfw
    yaml-cpp::yaml-cpp
    OpenMP::OpenMP_CXX
)
