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

# Add a compiler flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Make this a GUI application on Windows
if (WIN32)
    set(CMAKE_WIN32_EXECUTABLE ON)
endif ()

set(PROJECT_NAME "TrebleShot")
set(PROJECT_EXECUTABLE "trebleshot")
set(PROJECT_DESCRIPTION "Multi-platform file-sharing software")
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
find_package(KF5DNSSD NO_MODULE)

IF (Qt5Core_FOUND)
    MESSAGE(STATUS "Found Qt ${Qt5Core_VERSION_STRING}")

    # FIX: Qt was built with -reduce-relocations
    if (Qt5_POSITION_INDEPENDENT_CODE)
        SET(CMAKE_POSITION_INDEPENDENT_CODE ON)
    endif ()

    # guess plugins and libraries directory
    set(QT_DIR "${Qt5Core_DIR}/../../..")
    set(QT_BINARY_DIR "${QT_DIR}/bin")
    set(QT_PLUGINS_DIR "${QT_DIR}/plugins")
    get_target_property(QT_LIBRARY_DIR Qt5::Core LOCATION)
    get_filename_component(QT_LIBRARY_DIR ${QT_LIBRARY_DIR} PATH)
ELSE ()
    MESSAGE(FATAL_ERROR "No Qt5 found!")
ENDIF ()

# For Windows installation Qt libraries will be determined by this suffix.
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  set(QT_LIBRARY_SUFFIX "d.dll")
ELSEIF(CMAKE_BUILD_TYPE MATCHES Release)
  set(QT_LIBRARY_SUFFIX ".dll")
ELSE()
  set(QT_LIBRARY_SUFFIX ".dll")
ENDIF()


configure_file(src/build.h.in src/build.h)

IF (WIN32)
    configure_file(dist/Windows/package.nsi.in package.nsi)
ENDIF ()

configure_file(src/build.h.in src/build.h)

add_project_meta(META_FILES_TO_INCLUDE)

qt5_add_resources(QRCS
        assets/resources.qrc)

qt5_wrap_ui(UI_HEADERS
        src/ui/AboutDialog.ui
		src/ui/ConnectionReselectForDeviceDialog.ui
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
        src/broadcast/DNSSDService.h
        src/broadcast/DNSSDService.cpp
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
        src/model/FlawedTransferModel.h
        src/model/FlawedTransferModel.cpp
        src/model/NetworkDeviceModel.h
        src/model/NetworkDeviceModel.cpp
        src/model/TransferGroupModel.h
        src/model/TransferGroupModel.cpp
        src/model/TransferObjectModel.h
        src/model/TransferObjectModel.cpp
        src/ui/AboutDialog.h
        src/ui/AboutDialog.cpp
		src/ui/ConnectionReselectForDeviceDialog.h
		src/ui/ConnectionReselectForDeviceDialog.cpp
        src/ui/DeviceChooserDialog.h
        src/ui/DeviceChooserDialog.cpp
        src/ui/FileAdditionProgressDialog.h
        src/ui/FileAdditionProgressDialog.cpp
        src/ui/MainWindow.h
        src/ui/MainWindow.cpp
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
		src/util/SynchronizedList.h
		src/util/SynchronizedList.cpp
        src/util/TransferUtils.h
        src/util/TransferUtils.cpp
        src/util/ViewUtils.h
        src/util/ViewUtils.cpp src/model/ConnectionModel.cpp src/model/ConnectionModel.h src/database/DatabaseLoader.cpp src/database/DatabaseLoader.h)

if (KF5DNSSD_FOUND)
    add_definitions(-DUSE_DNSSD_FEATURE=1)
endif ()

add_executable(${PROJECT_EXECUTABLE} src/main.cpp ${UTIL_LIBRARY} ${UI_HEADERS} ${QRCS})

# Add the Qt5 Widgets for linking
if (NOT TARGET Qt5::WinMain)
    add_library(Qt5::WinMain STATIC IMPORTED)
endif ()

target_link_libraries(${PROJECT_EXECUTABLE} PUBLIC Qt5::Core Qt5::Gui Qt5::Network Qt5::Sql Qt5::Widgets)

if (KF5DNSSD_FOUND)
    target_link_libraries(${PROJECT_EXECUTABLE} PUBLIC KF5::DNSSD)
endif ()

fix_win_compiler()

if (NOT WIN32)
    install(TARGETS ${PROJECT_EXECUTABLE}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)
elseif(WIN32 AND MSVC)
	install(TARGETS ${PROJECT_EXECUTABLE}
		RUNTIME DESTINATION "/"
		LIBRARY DESTINATION lib)

	# The Qt5 Debug configuration library files have a 'd' postfix
	install(FILES
			${QT_BINARY_DIR}/Qt5Cored.dll
			${QT_BINARY_DIR}/Qt5Guid.dll
			${QT_BINARY_DIR}/Qt5Networkd.dll
			${QT_BINARY_DIR}/Qt5Widgetsd.dll
			${QT_BINARY_DIR}/Qt5Sqld.dll
		DESTINATION "/"
		CONFIGURATIONS Debug)
	# The Qt5 Release configuration files don't have a postfix
	install(FILES
			${QT_BINARY_DIR}/Qt5Core.dll
			${QT_BINARY_DIR}/Qt5Gui.dll
			${QT_BINARY_DIR}/Qt5Network.dll
			${QT_BINARY_DIR}/Qt5Widgets.dll
			${QT_BINARY_DIR}/Qt5Sql.dll
		DESTINATION "/"
		CONFIGURATIONS Release)
	# The files below are common to all configurations
	install(FILES
		${QT_BINARY_DIR}/plugins/platforms/qwindows.dll
		DESTINATION platforms)

	install(FILES
			distri/winlaunch.bat
		DESTINATION "/")
endif()

if (UNIX AND NOT APPLE)
    install(FILES assets/drawable/${PROJECT_EXECUTABLE}.png
            DESTINATION share/icons/hicolor/256x256/apps/)

    install(FILES dist/${PROJECT_EXECUTABLE}.desktop
            DESTINATION share/applications/)

    install(FILES dist/${PROJECT_EXECUTABLE}.desktop.appdata.xml
            DESTINATION share/appdata/)
endif (UNIX AND NOT APPLE)