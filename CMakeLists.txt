cmake_minimum_required(VERSION 3.20)
project(surviving-sarntal)

# Enable testing
enable_testing()

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set CMake policy if version is greater than 3.24.0
if(${CMAKE_VERSION} VERSION_GREATER "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")

# Add message to verify flags
message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")


# Export compile commands for clang-tidy and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom modules to CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/cmake)

# Function to remove unnecessary .clang-tidy files in libraries
function(remove_clang_tidy target_dir)
    if(EXISTS ${target_dir}/.clang-tidy)
        file(REMOVE ${target_dir}/.clang-tidy)
    endif()
endfunction()

# Include required modules
include(FetchContent)

# Add raygui as an interface library
add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE libs/raygui)

# FetchContent declarations for external libraries
FetchContent_Declare(raylib URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/raylib-4.5.0.tar.gz)
FetchContent_Declare(googletest URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/googletest-1.14.0.tar.gz)
FetchContent_Declare(yaml-cpp URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/yaml-cpp-0.8.0.tar.gz)
FetchContent_Declare(spdlog URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/spdlog-1.13.0.tar.gz)

FetchContent_MakeAvailable(raylib)
FetchContent_MakeAvailable(googletest)
FetchContent_MakeAvailable(yaml-cpp)
FetchContent_MakeAvailable(spdlog)

# Remove unnecessary .clang-tidy files from fetched libraries
remove_clang_tidy(${raylib_SOURCE_DIR})
remove_clang_tidy(${googletest_SOURCE_DIR})
remove_clang_tidy(${yaml-cpp_SOURCE_DIR})
remove_clang_tidy(${spdlog_SOURCE_DIR})

# Include directories
include_directories(libs/NITE/Include /usr/include/ni)
if(APPLE)
    include_directories(/opt/homebrew/Cellar/sdl2/2.30.3/include)
endif()

# Add the src and test directories
add_subdirectory(src)
add_subdirectory(test)

# Custom target to filter compile_commands.json
find_package(Python3 REQUIRED)
add_custom_target(
        filter_compile_commands
        COMMAND ${CMAKE_COMMAND} -E echo "Filtering compile_commands.json"
        COMMAND ${CMAKE_COMMAND} -E env python3 ${CMAKE_SOURCE_DIR}/build-utils/cleanupCompilationDatabase.py
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Filtering compile_commands.json to exclude libraries"
)

# Add dependencies to ensure filtering is done after the main targets are built
add_dependencies(surviving-sarntal filter_compile_commands)
add_dependencies(test-surviving-sarntal filter_compile_commands)

