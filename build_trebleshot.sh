#!/bin/sh
BUILD="release"  # release, debug
if [ -n "$1" ]; then
	BUILD="$1"
fi
echo "Building: $BUILD"
mkdir -p "cmake-build-$BUILD"
cd "cmake-build-$BUILD"
eval `qtchooser -qt=5 -print-env | grep QTLIBDIR`
cmake -DQTLIBDIR="$QTLIBDIR" -DCMAKE_BUILD_TYPE=$BUILD -G "CodeBlocks - Unix Makefiles" ..
cmake --build . --target trebleshot -- -j 2
#./trebleshot
