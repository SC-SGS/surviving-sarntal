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
