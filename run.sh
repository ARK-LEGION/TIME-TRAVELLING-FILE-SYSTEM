#!/usr/bin/env bash
set -euo pipefail

echo "Compiling ASSIGNMENT1.cpp..."
g++ -std=c++17 -O2 -Wall -Wextra -pedantic ASSIGNMENT1.cpp -o ASSIGNMENT1

echo "Compilation successful."
echo "Running ASSIGNMENT1 (reads from stdin). Type commands, then press Ctrl+D to end."

# Run the correct binary name on Windows (MinGW produces .exe)
if [[ -f "./ASSIGNMENT1.exe" ]]; then
  ./ASSIGNMENT1.exe
else
  ./ASSIGNMENT1
fi