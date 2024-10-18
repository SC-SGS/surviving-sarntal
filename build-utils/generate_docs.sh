#!/bin/bash

# Function to check if doxygen is installed
check_doxygen_installed() {
    if ! command -v doxygen &> /dev/null; then
        echo "Doxygen is not installed."
        return 1
    else
        echo "Doxygen is installed."
        return 0
    fi
}

# Function to prompt for installation
install_doxygen() {
    read -p "Do you want to install Doxygen? (y/n): " answer
    if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
        # Check the operating system and install accordingly
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            echo "Installing Doxygen on Linux..."
            sudo apt-get update && sudo apt-get install -y doxygen
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            echo "Installing Doxygen on macOS..."
            brew install doxygen
        else
            echo "Unsupported operating system. Please install Doxygen manually."
            exit 1
        fi
    else
        echo "Doxygen installation cancelled. Exiting."
        exit 1
    fi
}

# Function to generate the documentation
generate_docs() {
    if [ -f "Doxyfile" ]; then
        echo "Doxyfile found. Generating documentation..."
        doxygen Doxyfile
        echo "Documentation generated successfully."
    else
        echo "No Doxyfile found in the current directory. Exiting."
        exit 1
    fi
}

# Main script execution
echo "Checking if Doxygen is installed..."
if ! check_doxygen_installed; then
    install_doxygen
fi

generate_docs
