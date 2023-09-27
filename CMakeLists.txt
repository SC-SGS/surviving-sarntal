cmake_minimum_required(VERSION 3.20)

if (${CMAKE_VERSION} VERSION_GREATER "3.24.0")
  cmake_policy(SET CMP0135 NEW)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/cmake)

#set(CMAKE_CXX_CLANG_TIDY
#        clang-tidy-17;
#        -format-style='file';
#        -header-filter=${CMAKE_SOURCE_DIR}/src;
#        -extra-arg=-std=c++17;
#)

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

include_directories(libs/NITE/Include /usr/include/ni)

file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")
if (KINECT)
    list(FILTER SRC EXCLUDE REGEX "main.cpp")
else ()
    list(FILTER SRC EXCLUDE REGEX "main.cpp|kinect_handler.h|kinect_handler.cpp")
endif ()
#now SRC contains all .cpp and .h files in the src folder (and its subdirectories) except main.cpp
add_library(surviving-sarntal-library ${SRC})
#now all those files get made into one library callled surviving-sarntal-library
message(WARNING ${SRC})    #for CMAKE debugging purposes
add_executable(surviving-sarntal src/main.cpp)
#add main.cpp to surviving-sarntal executable

target_compile_features(surviving-sarntal-library PUBLIC cxx_std_17)

target_clangformat_setup(surviving-sarntal-library)

target_link_libraries(surviving-sarntal-library PUBLIC flecs::flecs_static raylib raygui)
if (KINECT)
    add_library( NITE-lib SHARED IMPORTED )
    set_target_properties( NITE-lib PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/libs/NITE/libXnVNite_1_5_2.so)
    target_link_libraries(surviving-sarntal-library PUBLIC NITE-lib)
    target_link_libraries(surviving-sarntal-library PUBLIC OpenNI)
    configure_file(libs/NITE/Include/Sample-Scene.xml . COPYONLY)
    configure_file(libs/NITE/Include/Sample-Tracking.xml . COPYONLY)
    configure_file(libs/NITE/Include/Sample-User.xml . COPYONLY)
    add_definitions(-Dkinect=1)
    #message(WARNING "Kinect is activated")
endif ()


#link some libraries to the surviving-sarntal-library that we are defining atm
target_link_libraries(surviving-sarntal PUBLIC surviving-sarntal-library)
#link the previously defined surviving-sarntal-library to the surviving-sarntal executable. The main.cpp has been included earlier

enable_testing()

file(GLOB_RECURSE TEST_SRC "test/*.cpp" "test/*.h")
add_executable(
        main_test
        ${TEST_SRC}
)
target_include_directories(main_test PUBLIC ".")
target_link_libraries(
        main_test
        GTest::gtest_main
        surviving-sarntal-library
)
#use surviving-sarntal-library for tests aswell. this way all the src-files only have to be compiled once instead of compiling it for the main executable and the tests

set_target_properties(
    surviving-sarntal PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")

include(GoogleTest)
gtest_discover_tests(main_test)
