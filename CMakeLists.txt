cmake_minimum_required(VERSION 3.20)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/cmake)

include(FetchContent)
include(ClangFormat)

project(surviving-sarntal)

add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE libs/raygui)


FetchContent_Declare(flecs URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/flecs-3.2.7.tar.gz)
FetchContent_MakeAvailable(flecs)
FetchContent_Declare(raylib URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib-4.5.0.tar.gz)
FetchContent_MakeAvailable(raylib)
FetchContent_Declare(googletest URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest-1.14.0.tar.gz)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")
add_executable(surviving-sarntal ${SRC})

target_compile_features(surviving-sarntal PUBLIC cxx_std_17)

target_clangformat_setup(surviving-sarntal)

target_link_libraries(surviving-sarntal PRIVATE flecs::flecs_static)
target_link_libraries(surviving-sarntal PRIVATE raylib)
target_link_libraries(surviving-sarntal PRIVATE raygui)

enable_testing()

file(GLOB_RECURSE TEST_SRC "test/*.cpp" "test/*.h")
add_executable(
        main_test
        ${TEST_SRC}
)
target_link_libraries(
        main_test
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(main_test)
