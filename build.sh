#!/usr/bin/env bash

set -euo pipefail

cd "$(dirname "$0")"

echo "========================================"
echo "Brusher Build"
echo "========================================"

cmake -S . -B build \
    -DCMAKE_BUILD_TYPE=Release

cmake --build build \
    --config Release \
    --parallel

echo
echo "Build completed successfully."

if [[ "${1:-}" != "--no-run" ]]; then
    echo "Starting Brusher..."
    ./build/brusher
fi
