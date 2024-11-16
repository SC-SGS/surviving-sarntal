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

# Update alternatives to ensure the correct versions of clang-format and clang-tidy
if command -v clang-format &> /dev/null; then
    INSTALLED_VERSION=$(clang-format --version | grep -oP '(?<=version )\d+' | head -n 1)
    if [ "$INSTALLED_VERSION" -lt 18 ]; then
        echo "Clang-format version $INSTALLED_VERSION is less than 18. Updating alternatives for clang-format version 18..."
        sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100
    else
        echo "Clang-format version $INSTALLED_VERSION is sufficient."
    fi
else
    echo "Clang-format is not installed. Setting up clang-format version 18..."
    sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-18 100
fi

if command -v clang-tidy &> /dev/null; then
    INSTALLED_VERSION=$(clang-tidy --version | grep -oP '(?<=version )\d+' | head -n 1)
    if [ "$INSTALLED_VERSION" -lt 18 ]; then
        echo "Clang-tidy version $INSTALLED_VERSION is less than 18. Updating alternatives for clang-tidy version 18..."
        sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100
    else
        echo "Clang-tidy version $INSTALLED_VERSION is sufficient."
    fi
else
    echo "Clang-tidy is not installed. Setting up clang-tidy version 18..."
    sudo update-alternatives --install /usr/bin/clang-tidy clang-tidy /usr/bin/clang-tidy-18 100
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
