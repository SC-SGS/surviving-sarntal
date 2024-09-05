#!/bin/bash

# Install necessary packages for regular users

install_on_linux() {
    sudo apt-get update
    sudo apt-get install -y build-essential cmake xorg-dev libsdl2-dev
}

install_on_mac() {
    # List of packages to install
    packages=("cmake" "sdl2")
    packages_to_install=()

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

    # Update Homebrew
    brew update

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
        echo "All packages needed to run the game are already installed."
    fi

    # Get the SDL2 installation path using `brew info`. This check is need to make sure cmake can build the project
    SDL2_PATH=$(brew info sdl2 | grep 'Cellar/sdl2' | awk '{print $1}')
    if [ -d "$SDL2_PATH" ]; then
        echo "The sdl2 location needed to build the project was found successfully. Location: $SDL2_PATH For more information see the wiki page on Installation."
    else
        echo "SDL2 is not installed or the path is not valid. Please make sure all requirements are met. You can find more information on the wiki page Installation."
        exit 1
    fi
}

install_on_windows() {
    if grep -qEi "(Microsoft|WSL)" /proc/version &> /dev/null; then
        # Running in WSL
        sudo apt-get update
        sudo apt-get install -y build-essential cmake xorg-dev libsdl2-dev
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
