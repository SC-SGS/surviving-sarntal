cmake_minimum_required(VERSION 3.20)
project(surviving-sarntal)

# Enable testing
enable_testing()

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set CMake policy if version is greater than 3.24.0
if (${CMAKE_VERSION} VERSION_GREATER "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif ()

# Create an option for enabling coverage support (for developers)
option(ENABLE_COVERAGE "Enable coverage report generation" OFF)

if (ENABLE_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} --coverage")

    # Add message to verify flags
    message("CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
    message("CMAKE_C_FLAGS: ${CMAKE_C_FLAGS}")
    message("CMAKE_EXE_LINKER_FLAGS: ${CMAKE_EXE_LINKER_FLAGS}")
endif ()

# Variable for optional custom coverage folder name
set(COVERAGE_DIR_NAME "" CACHE STRING "Optional custom name for the coverage folder")

# Export compile commands for clang-tidy and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add custom modules to CMake module path
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/libs/cmake)

# Function to remove unnecessary .clang-tidy files in libraries
function(remove_clang_tidy target_dir)
    if (EXISTS ${target_dir}/.clang-tidy)
        file(REMOVE ${target_dir}/.clang-tidy)
    endif ()
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
# Add poly2tri (local tarball)
FetchContent_Declare(
        poly2tri
        URL ${CMAKE_CURRENT_SOURCE_DIR}/libs/poly2tri-master.tar.gz
)

# Fetch content for all declared libraries
FetchContent_MakeAvailable(raylib googletest yaml-cpp spdlog poly2tri)

# Remove unnecessary .clang-tidy files from fetched libraries
remove_clang_tidy(${raylib_SOURCE_DIR})
remove_clang_tidy(${googletest_SOURCE_DIR})
remove_clang_tidy(${yaml-cpp_SOURCE_DIR})
remove_clang_tidy(${spdlog_SOURCE_DIR})
remove_clang_tidy(${poly2tri_SOURCE_DIR})

# Include directories
include_directories(libs/NITE/Include /usr/include/ni)
include_directories(${poly2tri_SOURCE_DIR}/poly2tri)
if (APPLE)
    execute_process(
            COMMAND bash -c "brew info sdl2 | grep 'Cellar/sdl2' | awk '{print $1}'"
            OUTPUT_VARIABLE SDL2_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT SDL2_PATH)
        message(FATAL_ERROR "SDL2 location could not be found. Make sure you have install sdl2 with homebrew or run the user_setup.sh script again.")
    endif()
    include_directories(${SDL2_PATH}/include)
endif ()


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

