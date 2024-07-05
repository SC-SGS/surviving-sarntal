# Gather source files
file(GLOB_RECURSE SRC "*.cpp" "*.h" "*.hpp")

# Create the library from source files
add_library(surviving-sarntal-library ${SRC})
add_executable(surviving-sarntal main.cpp)

# Set the C++ standard
target_compile_features(surviving-sarntal-library PUBLIC cxx_std_17)

# Link external libraries to the library
target_link_libraries(surviving-sarntal-library PUBLIC raylib raygui yaml-cpp spdlog::spdlog)

# Apply coverage flags to the library
set_target_properties(surviving-sarntal-library PROPERTIES COMPILE_FLAGS "${GCC_COVERAGE_COMPILE_FLAGS}")
set_target_properties(surviving-sarntal-library PROPERTIES LINK_FLAGS "${GCC_COVERAGE_LINK_FLAGS}")

if (APPLE)
    target_link_libraries(surviving-sarntal-library PUBLIC /opt/homebrew/Cellar/sdl2/2.30.3/lib/libSDL2.dylib)
else ()
    target_link_libraries(surviving-sarntal-library PUBLIC SDL2)
endif ()

# Link the library to the executable
target_link_libraries(surviving-sarntal PUBLIC surviving-sarntal-library)
