#!/bin/bash

set -euo pipefail

if [[ "$#" -lt 1 ]]
then
	echo "error: no input directory"
	echo "usage: ./codestyle_checker.sh <task_directory>"
	exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$SCRIPT_DIR"

INPUT_PATH="$1"

if [[ -d "$INPUT_PATH" ]]; then
	SOLUTION_DIRECTORY="$(cd "$INPUT_PATH" && pwd)"
elif [[ -d "$ROOT_DIR/$INPUT_PATH" ]]; then
	SOLUTION_DIRECTORY="$(cd "$ROOT_DIR/$INPUT_PATH" && pwd)"
else
	echo "error: directory '$INPUT_PATH' not found"
	exit 1
fi

BUILD_DIR="$ROOT_DIR/build"
if [[ ! -d "$BUILD_DIR" ]]; then
	echo "error: build directory not found at '$BUILD_DIR'"
	echo "hint: run cmake configure first, e.g. mkdir -p build && cd build && cmake .."
	exit 1
fi

mapfile -t CPP_FILES < <(find "$SOLUTION_DIRECTORY" -type f -name "*.cpp")

if [[ "${#CPP_FILES[@]}" -eq 0 ]]; then
	echo "warning: no .cpp files found in '$SOLUTION_DIRECTORY'"
	exit 0
fi

python3 "$ROOT_DIR/run-clang-format.py" -r "$SOLUTION_DIRECTORY" &&
clang-tidy -extra-arg=-std=c++20 -p "$BUILD_DIR" "${CPP_FILES[@]}"
