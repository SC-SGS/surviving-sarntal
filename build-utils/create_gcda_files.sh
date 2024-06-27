#!/bin/bash

# Define the source and build directories
SRC_DIR="./src"
BUILD_DIR_LIB="./cmake-build-debug/src/CMakeFiles/surviving-sarntal-library.dir"
BUILD_DIR_MAIN="./cmake-build-debug/src/CMakeFiles/surviving-sarntal.dir"

# Loop through each .cpp file in the source directory recursively
find "$SRC_DIR" -type f -name "*.cpp" | while read -r cpp_file;
do
  # Get the relative path of the cpp file from the src directory
  relative_path="${cpp_file#$SRC_DIR/}"

  # Get the directory of the relative path
  relative_dir=$(dirname "$relative_path")

  # Create the corresponding directory in the build directory for library
  mkdir -p "$BUILD_DIR_LIB/$relative_dir"

  # Get the base name of the cpp file (without the directory and extension)
  base_name=$(basename "$cpp_file" .cpp)

  # Create an empty .gcda file in the corresponding build directory
  touch "$BUILD_DIR_LIB/$relative_dir/$base_name.cpp.gcda"

  # If the cpp file is in the main directory, create an additional .gcda file in the main build directory
  if [[ "$base_name" == "main" ]]; then
    mkdir -p "$BUILD_DIR_MAIN"
    touch "$BUILD_DIR_MAIN/$base_name.cpp.gcda"
  fi
done

echo "Empty .gcda files created for each .cpp file in the src folder."

