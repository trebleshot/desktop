cmake_minimum_required(VERSION 3.8.2)

project(TrebleShot)

# Add folder where are supportive functions
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find includes in the build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Turn on automatic invocation of the MOC
set(CMAKE_AUTOMOC ON)

# Minimum C++11
set(CMAKE_CXX_STANDARD 11)

# Include Qt basic functions
include(QtCommon)

# Set PROJECT_VERSION_PATCH and PROJECT_VERSION_TWEAK to 0 if not present, needed by add_project_meta
fix_project_version()

#For Build on Mac: uncomment following srting and provice proper way to QT
set(CMAKE_PREFIX_PATH ~/DevToolsMac/QT/5.9.6/clang_64)

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Make this a GUI application on Windows
if (WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif ()

set(PROJECT_NAME "TrebleShot")
set(PROJECT_DESCRIPTION "Sophisticated and multi-platform file-sharing software")
set(PROJECT_AUTHOR "Genonbeta")
set(PROJECT_URL "https://github.com/genonbeta/TrebleShot-Desktop")

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(PROJECT_VERSION_CODE 91)

# Find the Qt5 libraries
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Widgets REQUIRED)

IF (Qt5Core_FOUND)
    MESSAGE(STATUS "Found Qt ${Qt5Core_VERSION_STRING}")

    # FIX: Qt was built with -reduce-relocations
    if (Qt5_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif ()

    # guess plugins and libraries directory
    set(QT_PLUGINS_DIR "${Qt5Core_DIR}/../../../plugins")
    get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
    get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
ELSE ()
    MESSAGE(FATAL_ERROR "No Qt5 found!")
ENDIF ()

configure_file(src/build.h.in src/build.h)

add_project_meta(META_FILES_TO_INCLUDE)

qt5_add_resources(QRCS
        assets/resources.qrc)

qt5_wrap_ui(UI_HEADERS
        src/ui/DeviceChooserDialog.ui
        src/ui/FileAdditionProgressDialog.ui
        src/ui/MainWindow.ui
        src/ui/ManageDevicesDialog.ui
        src/ui/ShowTransferDialog.ui
        src/ui/TransferRequestProgressDialog.ui
        src/dialog/WelcomeDialog.ui)

set(UTIL_LIBRARY
        src/build.h
        src/broadcast/CommunicationServer.h
        src/broadcast/CommunicationServer.cpp
        src/broadcast/NetworkServiceDiscovery.h
        src/broadcast/NetworkServiceDiscovery.cpp
        src/broadcast/SeamlessServer.h
        src/broadcast/SeamlessServer.cpp
        src/broadcast/SeamlessClient.h
        src/broadcast/SeamlessClient.cpp
        src/config/Config.h
        src/config/Config.cpp
        src/config/Keyword.h
        src/coolsocket/CoolSocket.h
        src/coolsocket/CoolSocket.cpp
        src/database/AccessDatabase.h
        src/database/AccessDatabase.cpp
        src/database/ReconstructionException.h
        src/database/ReconstructionException.cpp
        src/database/object/NetworkDevice.h
        src/database/object/NetworkDevice.cpp
        src/database/object/TextStreamObject.h
        src/database/object/TextStreamObject.cpp
        src/database/object/TransferObject.h
        src/database/object/TransferObject.cpp
        src/database/object/TransferGroup.h
        src/database/object/TransferGroup.cpp
        src/dialog/WelcomeDialog.h
        src/dialog/WelcomeDialog.cpp
        src/model/NetworkDeviceModel.h
        src/model/NetworkDeviceModel.cpp
        src/model/TransferGroupModel.h
        src/model/TransferGroupModel.cpp
        src/model/TransferObjectModel.h
        src/model/TransferObjectModel.cpp
        src/ui/DeviceChooserDialog.h
        src/ui/DeviceChooserDialog.cpp
        src/ui/FileAdditionProgressDialog.h
        src/ui/FileAdditionProgressDialog.cpp
        src/ui/ManageDevicesDialog.h
        src/ui/ManageDevicesDialog.cpp
        src/ui/ShowTransferDialog.h
        src/ui/ShowTransferDialog.cpp
        src/ui/TransferRequestProgressDialog.h
        src/ui/TransferRequestProgressDialog.cpp
        src/util/AppUtils.h
        src/util/AppUtils.cpp
        src/util/CommunicationBridge.h
        src/util/CommunicationBridge.cpp
        src/util/GThread.h
        src/util/GThread.cpp
        src/util/Interrupter.h
        src/util/Interrupter.cpp
        src/util/NetworkDeviceLoader.h
        src/util/NetworkDeviceLoader.cpp
        src/util/TransferUtils.h
        src/util/TransferUtils.cpp
        src/util/ViewUtils.h
        src/util/ViewUtils.cpp src/model/FlawedTransferModel.cpp src/model/FlawedTransferModel.h)

add_library(sources OBJECT ${UTIL_LIBRARY} ${UI_HEADERS} ${QRCS})
add_executable(trebleshot src/main.cpp src/ui/MainWindow.cpp)

# Add the Qt5 Widgets for linking
if (NOT TARGET Qt5::WinMain)
    add_library(Qt5::WinMain STATIC IMPORTED)
endif ()

target_link_libraries(sources PUBLIC Qt5::Core Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets)
target_link_libraries(trebleshot PUBLIC sources)
