cmake_minimum_required(VERSION 3.8.2)

project(TrebleShot VERSION 1.0)

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Include Qt basic functions
include(QtCommon)

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()

# There may be a way to tell up front if Qt5 is going to be found, but I haven't found
# a foolproof way to do it yet, so settle for the default error message for now.

#if(NOT CMAKE_PREFIX_PATH AND NOT Qt5Widgets_DIR)
#message(WARNING "CMAKE_PREFIX_PATH is not defined, so find_package may not work. Set the CMAKE_PREFIX_PATH "
#"environment variable to the install prefix of Qt 5, either on the command line as "
#"-DCMAKE_PREFIX_PATH=\"path/to/Qt5/lib/cmake\" or with set(CMAKE_PREFIX_PATH path/to/Qt5/lib/cmake)")
#endif(NOT CMAKE_PREFIX_PATH AND NOT Qt5Widgets_DIR)

#For Build on Mac: uncomment following srting and provice proper way to QT
set(CMAKE_PREFIX_PATH ~/DevToolsMac/QT/5.9.6/clang_64)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Make this a GUI application on Windows
if (WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif ()

# Find the Qt5 libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)

qt5_add_resources(QRCS assets/resources.qrc)
qt5_wrap_ui(UI_HEADERS mainwindow.ui)

add_library(assets SHARED ${UI_HEADERS} ${QRCS})

set(UTIL_LIBRARY src/accessdatabase.h
        src/accessdatabase.cpp
        src/communicationserver.h
        src/communicationserver.cpp
        src/config.h
        src/coolsocket.h
        src/coolsocket.cpp
        src/keyword.h
        src/stringlistmodel.h
        src/stringlistmodel.cpp
        src/transferobject.h
        src/transferobject.cpp)

# Generate UI and resources

add_library(utils SHARED ${UTIL_LIBRARY})

add_project_meta(META_FILES_TO_INCLUDE)

# Do something something
add_executable(TrebleShot main.cpp mainwindow.cpp)

# Add the Qt5 Widgets for linking
if (NOT TARGET Qt5::WinMain)
    add_library(Qt5::WinMain STATIC IMPORTED)
endif ()

target_link_libraries(utils PUBLIC Qt5::Core Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets)
target_link_libraries(TrebleShot assets utils)
