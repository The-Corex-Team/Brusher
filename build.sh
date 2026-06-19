#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel

if [[ "${1:-}" != "--no-run" ]]; then
    ./build/Brusher
fi
