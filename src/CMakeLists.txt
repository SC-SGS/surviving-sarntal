# Gather source files
file(GLOB_RECURSE SRC "*.cpp" "*.h" "*.hpp")

# Create the library from source files
add_library(surviving-sarntal-library ${SRC})
add_executable(surviving-sarntal main.cpp)

# Set the C++ standard
target_compile_features(surviving-sarntal-library PUBLIC cxx_std_17)

# Link external libraries to the library
target_link_libraries(surviving-sarntal-library PUBLIC raylib raygui yaml-cpp spdlog::spdlog)

if (APPLE)
    execute_process(
            COMMAND bash -c "brew info sdl2 | grep 'Cellar/sdl2' | awk '{print $1}'"
            OUTPUT_VARIABLE SDL2_PATH
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if (NOT SDL2_PATH)
        message(FATAL_ERROR "SDL2 location could not be found. Make sure you have install sdl2 with homebrew or run the user_setup.sh script again.")
    endif()
    target_link_libraries(surviving-sarntal-library PUBLIC ${SDL2_PATH}/lib/libSDL2.dylib)
else ()
    target_link_libraries(surviving-sarntal-library PUBLIC SDL2)
endif ()

if(ENABLE_COVERAGE)
    # Apply coverage flags to the library
    set_target_properties(surviving-sarntal-library PROPERTIES COMPILE_FLAGS "${GCC_COVERAGE_COMPILE_FLAGS}")
    set_target_properties(surviving-sarntal-library PROPERTIES LINK_FLAGS "${GCC_COVERAGE_LINK_FLAGS}")
endif()

# Link the library to the executable
target_link_libraries(surviving-sarntal PUBLIC surviving-sarntal-library)
