#!/bin/bash

# Cross-platform run script

# Ensure the project is built
source build-utils/build.sh

# Change to the build directory and run the executable
if [[ "$OSTYPE" == "cygwin"* || "$OSTYPE" == "msys"* || "$OSTYPE" == "win32"* ]]; then
    cd cmake-build-debug/src || exit
    ./surviving-sarntal.exe
    cd ../..
else
    cd cmake-build-debug/src || exit
    ./surviving-sarntal
    cd ../..
fi
