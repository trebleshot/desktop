#!/bin/sh
mkdir "cmake-build-debug"
cd "cmake-build-debug"
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" ..
cmake --build . --target trebleshot -- -j 2
./trebleshot
