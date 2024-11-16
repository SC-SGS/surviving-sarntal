#!/bin/bash

# Update the package list
sudo apt-get update

# Install required dependencies
sudo apt-get install -y \
    build-essential \
    cmake \
    xorg-dev \
    libsdl2-dev \
    clang-tidy \
    clang-format \
    git \
    pre-commit \
    python3 \
    lcov

sudo apt-get install -y software-properties-common wget build-essential cmake xorg-dev libsdl2-dev git pre-commit python3 lcov

# Add the official LLVM APT repository for the latest Clang versions
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 18  # Install LLVM version 18

# Verify installation paths
CLANG_FORMAT_PATH="/usr/lib/llvm-18/bin/clang-format"
CLANG_TIDY_PATH="/usr/lib/llvm-18/bin/clang-tidy"

# Update alternatives to point to LLVM tools
if [ -f "$CLANG_FORMAT_PATH" ]; then
    echo "Setting up clang-format version 18..."
    sudo update-alternatives --install /usr/bin/clang-format clang-format "$CLANG_FORMAT_PATH" 100
else
    echo "Error: clang-format-18 binary not found at $CLANG_FORMAT_PATH"
fi

if [ -f "$CLANG_TIDY_PATH" ]; then
    echo "Setting up clang-tidy version 18..."
    sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy "$CLANG_TIDY_PATH" 100
else
    echo "Error: clang-tidy-18 binary not found at $CLANG_TIDY_PATH"
fi

# Echo installed software versions
echo "Installed software versions:"
echo "--------------------------------"
echo "gcc version: $(gcc --version | head -n 1)"
echo "cmake version: $(cmake --version | head -n 1)"
echo "clang-tidy version: $(clang-tidy --version | head -n 1)"
echo "clang-format version: $(clang-format --version | head -n 1)"
echo "git version: $(git --version)"
echo "pre-commit version: $(pre-commit --version)"
echo "python3 version: $(python3 --version)"
echo "lcov version: $(lcov --version | head -n 1)"
echo "--------------------------------"
