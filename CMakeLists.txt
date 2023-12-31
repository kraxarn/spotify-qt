cmake_minimum_required(VERSION 3.9)

project(spotify-qt LANGUAGES CXX VERSION 3.11)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional D-Bus support
option(USE_DBUS "Use D-Bus integration" ON)

# Optional KCrash support
option(USE_KCRASH "Use KCrash integration" ON)

# LTO support
option(USE_LTO "Use link time optimization" OFF)

# Prefer new OpenGL
if (POLICY CMP0072)
	cmake_policy(SET CMP0072 NEW)
endif ()

# Set target macOS version
if (APPLE)
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
endif ()

# Find Qt
include(cmake/FindQt.cmake)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network Gui Svg REQUIRED)
if (USE_DBUS)
	find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS DBus QUIET)
endif ()

# Find KCRash
if (USE_KCRASH)
	find_package(KF5Crash QUIET)
endif ()

# Create main executable
if (ANDROID)
	add_library(spotify-qt SHARED res.qrc)
elseif (MSVC)
	add_executable(spotify-qt WIN32 res.qrc
		${CMAKE_CURRENT_SOURCE_DIR}/res/app/app.rc)
elseif (APPLE)
	set(APP_ICNS ${CMAKE_CURRENT_SOURCE_DIR}/res/app/spotify-qt.icns)
	add_executable(spotify-qt MACOSX_BUNDLE res.qrc ${APP_ICNS})
	# Load properties from Info.plist file
	set_target_properties(spotify-qt PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/res/app/Info.plist)
	# Copy icon file to Resources folder
	set_source_files_properties(${APP_ICNS} PROPERTIES
		MACOSX_PACKAGE_LOCATION "Resources")
else ()
	add_executable(spotify-qt res.qrc)
endif ()

# Collect source files
add_subdirectory(src)

# Headers are in src/
target_include_directories(spotify-qt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Get version from Git
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		OUTPUT_VARIABLE GIT_COMMIT
		OUTPUT_STRIP_TRAILING_WHITESPACE)
	target_compile_definitions(spotify-qt PRIVATE GIT_COMMIT="${GIT_COMMIT}")
endif ()

# App related definitions
target_compile_definitions(spotify-qt PRIVATE APP_VERSION="v${PROJECT_VERSION}")
target_compile_definitions(spotify-qt PRIVATE APP_NAME="${PROJECT_NAME}")
target_compile_definitions(spotify-qt PRIVATE APP_ICON="${PROJECT_NAME}")
target_compile_definitions(spotify-qt PRIVATE ORG_NAME="kraxarn")
target_compile_definitions(spotify-qt PRIVATE PKG_NAME="com.kraxarn")
target_compile_definitions(spotify-qt PRIVATE BUILD_TYPE="${CMAKE_BUILD_TYPE}")

# Install icons and desktop shortcut on unix-like
if (UNIX)
	install(FILES res/logo/spotify-qt.svg DESTINATION share/icons/hicolor/scalable/apps)
	install(FILES res/app/spotify-qt.desktop DESTINATION share/applications)
endif ()

if (APPLE)
	# macOS bundling
	install(TARGETS spotify-qt BUNDLE DESTINATION bin)
else ()
	# Binary for other targets
	install(TARGETS spotify-qt RUNTIME DESTINATION bin)
endif ()

# Don't show console window under windows
if (WIN32 AND NOT MSVC)
	target_link_options(spotify-qt PRIVATE -mwindows)
endif ()

# IPO / LTO
if (USE_LTO)
	include(CheckIPOSupported)
	check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
	if (ipo_supported AND CMAKE_BUILD_TYPE STREQUAL "Release")
		set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
	elseif (ipo_supported)
		message(STATUS "LTO is only supported in Release builds")
	else ()
		message(STATUS "LTO error: ${ipo_error}")
	endif ()
endif ()

# Link Qt
target_link_libraries(spotify-qt PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Network
	Qt${QT_VERSION_MAJOR}::Gui
	Qt${QT_VERSION_MAJOR}::Svg)

# spotify-qt-lib
set(LIB_QT_IMPL ON)
add_subdirectory(lib)
target_link_libraries(spotify-qt PRIVATE spotify-qt-lib)

# Enable all compiler warnings under GCC/Clang
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	target_compile_options(spotify-qt PRIVATE -Wall -Wextra)
endif ()

# D-Bus support is optional
if (Qt${QT_VERSION_MAJOR}DBus_FOUND)
	target_compile_definitions(spotify-qt PRIVATE USE_DBUS)
	target_link_libraries(spotify-qt PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
endif ()

# KCrash support
if (KF5Crash_FOUND AND NOT USE_QT6)
	target_compile_definitions(spotify-qt PRIVATE USE_KCRASH)
	target_link_libraries(spotify-qt PRIVATE KF5::Crash)
endif ()
