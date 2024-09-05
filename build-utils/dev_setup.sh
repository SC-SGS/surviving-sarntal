#!/bin/bash

# Call the first script to install build dependencies
source ./build-utils/user_setup.sh

# Install additional packages for developers

install_on_linux() {
    sudo apt-get update
    sudo apt-get install -y clang-tidy clang-format git pre-commit python3 lcov
}

install_on_mac() {
    # List of packages to install
    packages=("clang-format" "git" "pre-commit" "python" "lcov")

    # Check if Homebrew is installed
    if ! command -v brew &> /dev/null; then
        echo "Homebrew not found."

        # Ask for permission to install Homebrew
        read -p "Do you want to install Homebrew? (Y/n): " install_brew
        if [[ "$install_brew" != "Y" ]]; then
            echo "Homebrew installation skipped. Exiting."
            exit 1
        fi

        echo "Installing Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
    else
        echo "Homebrew is already installed."
    fi

# Check if the packages are already installed on the machine
    for package in "${packages[@]}"; do
        if brew list "$package" &> /dev/null; then
            echo "$package is already installed."
        else
            packages_to_install+=("$package")
        fi
    done

    # If there are packages to install, list them and ask for permission
    if [ ${#packages_to_install[@]} -gt 0 ]; then
        echo "The following packages will be installed:"
        for package in "${packages_to_install[@]}"; do
            echo "- $package"
        done

        # Ask for permission to install the packages
        read -p "Do you want to proceed with the installation of these packages? (Y/n): " install_packages
        if [[ "$install_packages" != "Y" ]]; then
            echo "Package installation skipped. Exiting."
            exit 1
        fi

        # Install the packages
        brew install "${packages_to_install[@]}"
    else
        echo "All packages needed to develop the game are already installed."
    fi
}

install_on_windows() {
    if grep -qEi "(Microsoft|WSL)" /proc/version &> /dev/null; then
        # Running in WSL
        sudo apt-get update
        sudo apt-get install -y clang-tidy clang-format git pre-commit python3 lcov
    else
        echo "Please run this script inside a Unix-like environment (such as WSL) on Windows."
        exit 1
    fi
}

# Detect platform and call the appropriate function
case "$OSTYPE" in
    linux-gnu*) install_on_linux ;;
    darwin*) install_on_mac ;;
    cygwin*|msys*|win32*) install_on_windows ;;
    *) echo "Unsupported platform: $OSTYPE" ;;
esac
