cmake_minimum_required(VERSION 3.20)
include(FetchContent)

project(surviving-sarntal)

FetchContent_Declare(flecs URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/flecs-3.2.7.tar.gz)
FetchContent_MakeAvailable(flecs)
FetchContent_Declare(raylib URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib-4.5.0.tar.gz)
FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")
add_executable(surviving-sarntal ${SRC})

target_compile_features(surviving-sarntal PUBLIC cxx_std_17)

target_link_libraries(surviving-sarntal PRIVATE flecs::flecs flecs::flecs_static)
target_link_libraries(surviving-sarntal PRIVATE raylib)
