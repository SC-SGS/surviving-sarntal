#!/bin/bash

# Directory containing the source files
SRC_DIR="src/"
TEST_DIR="test/"

# Compile commands directory
BUILD_DIR="cmake-build-debug/"

# ANSI color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_message() {
    local color="$1"
    local message="$2"
    echo -e "${color}${message}${NC}"
}

# Generate compile_commands.json if it doesn't exist
if [ ! -f "$BUILD_DIR/compile_commands.json" ]; then
    print_message $YELLOW "Generating compile_commands.json..."
    cmake -B $BUILD_DIR
    if [ $? -eq 0 ]; then
        print_message $GREEN "Successfully generated compile_commands.json"
    else
        print_message $RED "Failed to generate compile_commands.json"
        exit 1
    fi
fi

# Find all .cpp files in the src directory
FILES=$(find $SRC_DIR $TEST_DIR -type f \( -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \))

# Run clang-tidy on each file
for FILE in $FILES; do
    print_message $YELLOW "Running clang-tidy on $FILE..."
    clang-tidy $FILE -p=$BUILD_DIR --warnings-as-errors=* --header-filter="^((src|test)/.*)$" --use-color
    if [ $? -eq 0 ]; then
        print_message $GREEN "clang-tidy passed for $FILE"
    else
        print_message $RED "clang-tidy found issues in $FILE"
    fi
done

# Run clang tidy on all files

print_message $YELLOW "Running clang-tidy on all files..."
clang-tidy -p=$BUILD_DIR $FILES --warnings-as-errors=* --header-filter="^((src|test)/.*)$" --use-color
if [ $? -eq 0 ]; then
    print_message $GREEN "clang-tidy passed for all files"
else
    print_message $RED "clang-tidy found issues in files"
fi