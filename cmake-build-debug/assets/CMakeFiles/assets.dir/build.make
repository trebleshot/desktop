# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/veli/.local/share/clion-2018.2.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/veli/.local/share/clion-2018.2.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/veli/Workspace/Qt-CMake-HelloWorld

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug

# Include any dependencies generated for this target.
include assets/CMakeFiles/assets.dir/depend.make

# Include the progress variables for this target.
include assets/CMakeFiles/assets.dir/progress.make

# Include the compile flags for this target's objects.
include assets/CMakeFiles/assets.dir/flags.make

assets/ui_mainwindow.h: ../assets/mainwindow.ui
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ui_mainwindow.h"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/lib/qt5/bin/uic -o /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/ui_mainwindow.h /home/veli/Workspace/Qt-CMake-HelloWorld/assets/mainwindow.ui

assets/qrc_resources.cpp: ../assets/drawable/ic_launcher.png
assets/qrc_resources.cpp: ../assets/strings/Turkish.ts
assets/qrc_resources.cpp: ../assets/strings/Turkish.qm
assets/qrc_resources.cpp: ../assets/resources.qrc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating qrc_resources.cpp"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/lib/qt5/bin/rcc --name resources --output /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/qrc_resources.cpp /home/veli/Workspace/Qt-CMake-HelloWorld/assets/resources.qrc

assets/CMakeFiles/assets.dir/qrc_resources.cpp.o: assets/CMakeFiles/assets.dir/flags.make
assets/CMakeFiles/assets.dir/qrc_resources.cpp.o: assets/qrc_resources.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object assets/CMakeFiles/assets.dir/qrc_resources.cpp.o"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/assets.dir/qrc_resources.cpp.o -c /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/qrc_resources.cpp

assets/CMakeFiles/assets.dir/qrc_resources.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/assets.dir/qrc_resources.cpp.i"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/qrc_resources.cpp > CMakeFiles/assets.dir/qrc_resources.cpp.i

assets/CMakeFiles/assets.dir/qrc_resources.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/assets.dir/qrc_resources.cpp.s"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/qrc_resources.cpp -o CMakeFiles/assets.dir/qrc_resources.cpp.s

assets/CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.o: assets/CMakeFiles/assets.dir/flags.make
assets/CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.o: assets/assets_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object assets/CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.o"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.o -c /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/assets_autogen/mocs_compilation.cpp

assets/CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.i"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/assets_autogen/mocs_compilation.cpp > CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.i

assets/CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.s"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/assets_autogen/mocs_compilation.cpp -o CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.s

# Object files for target assets
assets_OBJECTS = \
"CMakeFiles/assets.dir/qrc_resources.cpp.o" \
"CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.o"

# External object files for target assets
assets_EXTERNAL_OBJECTS =

assets/libassets.so: assets/CMakeFiles/assets.dir/qrc_resources.cpp.o
assets/libassets.so: assets/CMakeFiles/assets.dir/assets_autogen/mocs_compilation.cpp.o
assets/libassets.so: assets/CMakeFiles/assets.dir/build.make
assets/libassets.so: assets/CMakeFiles/assets.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX shared library libassets.so"
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/assets.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
assets/CMakeFiles/assets.dir/build: assets/libassets.so

.PHONY : assets/CMakeFiles/assets.dir/build

assets/CMakeFiles/assets.dir/clean:
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets && $(CMAKE_COMMAND) -P CMakeFiles/assets.dir/cmake_clean.cmake
.PHONY : assets/CMakeFiles/assets.dir/clean

assets/CMakeFiles/assets.dir/depend: assets/ui_mainwindow.h
assets/CMakeFiles/assets.dir/depend: assets/qrc_resources.cpp
	cd /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/veli/Workspace/Qt-CMake-HelloWorld /home/veli/Workspace/Qt-CMake-HelloWorld/assets /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets /home/veli/Workspace/Qt-CMake-HelloWorld/cmake-build-debug/assets/CMakeFiles/assets.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : assets/CMakeFiles/assets.dir/depend

