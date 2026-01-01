#!/bin/bash

# Development environment setup script for VideoGameVersionControl

echo "========================================="
echo "VGVC Development Environment Setup"
echo "========================================="

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Linux*)     OS_TYPE=Linux;;
    Darwin*)    OS_TYPE=Mac;;
    MINGW*)     OS_TYPE=Windows;;
    *)          OS_TYPE="UNKNOWN:${OS}"
esac

echo "Detected OS: ${OS_TYPE}"

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check for required tools
echo ""
echo "Checking for required tools..."

# Check CMake
if command_exists cmake; then
    CMAKE_VERSION=$(cmake --version | head -n1)
    echo "✓ CMake found: ${CMAKE_VERSION}"
else
    echo "✗ CMake not found. Please install CMake 3.20 or higher."
    exit 1
fi

# Check Git
if command_exists git; then
    GIT_VERSION=$(git --version)
    echo "✓ Git found: ${GIT_VERSION}"
else
    echo "✗ Git not found. Please install Git 2.30 or higher."
    exit 1
fi

# Check for Qt
echo ""
echo "Checking for Qt6..."
if command_exists qmake6 || command_exists qmake; then
    echo "✓ Qt appears to be installed"
else
    echo "⚠ Qt6 may not be installed or not in PATH"
    echo "  Please install Qt 6.5 or higher from https://www.qt.io/download"
fi

# Check for C++ compiler
echo ""
echo "Checking for C++ compiler..."
if command_exists g++; then
    GCC_VERSION=$(g++ --version | head -n1)
    echo "✓ g++ found: ${GCC_VERSION}"
elif command_exists clang++; then
    CLANG_VERSION=$(clang++ --version | head -n1)
    echo "✓ clang++ found: ${CLANG_VERSION}"
else
    echo "✗ No C++ compiler found. Please install g++ or clang++."
    exit 1
fi

# Create build directory
echo ""
echo "Creating build directory..."
mkdir -p build
echo "✓ Build directory created"

# Platform-specific instructions
echo ""
echo "========================================="
echo "Next Steps:"
echo "========================================="
echo ""
echo "1. Configure the build:"
echo "   cd build"
echo "   cmake .."
echo ""
echo "2. Build the project:"
echo "   cmake --build ."
echo ""
echo "3. Run the application:"
echo "   ./bin/vgvc"
echo ""
echo "========================================="
echo "Setup complete!"
echo "========================================="
