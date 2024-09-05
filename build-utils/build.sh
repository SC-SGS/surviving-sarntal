#!/bin/bash

# Cross-platform build script

# Determine platform
case "$OSTYPE" in
    darwin*)
        # macOS: Use Unix Makefiles
        cmake -S . -B cmake-build-debug -G "Unix Makefiles"
        ;;
    linux-gnu*)
        # Linux: Drop the Unix Makefiles generator
        cmake -S . -B cmake-build-debug
        ;;
    cygwin*|msys*|win32*)
        # Windows using WSL or Git Bash: Drop Unix Makefiles generator
        cmake -S . -B cmake-build-debug
        ;;
    *)
        echo "Unsupported platform: $OSTYPE"
        exit 1
        ;;
esac

# Build the project
cmake --build cmake-build-debug
