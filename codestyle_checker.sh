#!/bin/bash

if [[ "$#" -lt 1 ]]
then
	echo "error: no input files"
	exit 1
fi

SOLUTION_DIRECTORY=./$1

./run-clang-format.py -r "$SOLUTION_DIRECTORY" &&
clang-tidy -extra-arg=-std=c++20 -p ./build "$SOLUTION_DIRECTORY"/*cpp
