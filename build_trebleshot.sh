#!/bin/sh
BUILD="release"  # release, debug
if [ "$1" ]; then
	BUILD="$1"
fi
echo "Building: $BUILD"
mkdir "cmake-build-$BUILD"
cd "cmake-build-$BUILD" || exit
eval `qtchooser -qt=5 -print-env | grep QTLIBDIR`
cmake -DQTLIBDIR="$QTLIBDIR" -DCMAKE_BUILD_TYPE="$BUILD" -G "CodeBlocks - Unix Makefiles" ..
cmake --build . --target trebleshot -- -j 2
#./trebleshot
