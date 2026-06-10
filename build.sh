#!/bin/bash

set -e

echo "Creating build directory..."
mkdir -p build

echo "Entering build directory..."
cd build

echo "Configuring project..."
cmake ..

echo "Building Brusher..."
cmake --build . -j$(nproc)

echo "Running Brusher..."
./Brusher