#!/bin/bash

# Packaging script for VideoGameVersionControl

echo "========================================="
echo "VGVC Packaging Script"
echo "========================================="

# Detect OS
OS="$(uname -s)"
case "${OS}" in
    Linux*)     OS_TYPE=Linux;;
    Darwin*)    OS_TYPE=Mac;;
    MINGW*)     OS_TYPE=Windows;;
    *)          OS_TYPE="UNKNOWN:${OS}"
esac

echo "Packaging for: ${OS_TYPE}"

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Error: Build directory not found."
    echo "Please run the build first:"
    echo "  mkdir build && cd build && cmake .. && cmake --build ."
    exit 1
fi

cd build

echo ""
echo "Running CPack..."
cpack

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================="
    echo "Packaging successful!"
    echo "========================================="
    echo "Package files are in the build directory."
    ls -lh *.deb *.rpm *.dmg *.exe 2>/dev/null || echo "Check build directory for package files."
else
    echo ""
    echo "Error: Packaging failed."
    exit 1
fi
