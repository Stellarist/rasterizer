cmake_minimum_required(VERSION 3.20)

project(rasterizer)

add_compile_definitions(PROJECT_PATH="${CMAKE_SOURCE_DIR}")

file(GLOB INC_LIST src/*.hpp)
file(GLOB SRC_LIST src/*.cpp)
file(GLOB SHADER_LIST shaders/*)

find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

add_executable(rasterizer ${INC_LIST} ${SRC_LIST})

target_link_libraries(rasterizer
    glfw
    glad::glad
    yaml-cpp::yaml-cpp
)
