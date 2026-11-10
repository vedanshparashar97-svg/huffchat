#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
echo "Building HuffChat C++17 compression engine..."
g++ -std=c++17 -O2 -Wall -Wextra -pedantic huffman_cli.cpp -o huffchat_cli
echo "Build complete: ./huffchat_cli"
