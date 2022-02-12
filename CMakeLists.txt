cmake_minimum_required(VERSION 3.5)

project(spotify-qt LANGUAGES CXX VERSION 3.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional D-Bus support
if (NOT DEFINED USE_DBUS)
	set(USE_DBUS 1)
endif ()

# Optional KCrash support
if (NOT DEFINED USE_KCRASH)
	set(USE_KCRASH 1)
endif ()

# Optional Qt 6 support
if (DEFINED USE_QT6)
	set(QT_VERSION_MAJOR 6)
else ()
	set(QT_VERSION_MAJOR 5)
endif ()

# Prefer new OpenGL
if (POLICY CMP0072)
	cmake_policy(SET CMP0072 NEW)
endif ()

# Qt 6 requires C++17 under MSVC
if (QT_VERSION_MAJOR EQUAL 6 AND MSVC)
	set(CMAKE_CXX_STANDARD 17)
endif ()

# Find Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network Gui Svg REQUIRED)
if (USE_DBUS)
	find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS DBus QUIET)
endif ()
message(STATUS "Using Qt ${QT_VERSION_MAJOR}")

# Find KCRash
if (USE_KCRASH)
	find_package(KF5Crash QUIET)
endif ()

# Create main executable
if (ANDROID)
	add_library(spotify-qt SHARED ${SRC_FILES} res.qrc)
elseif (MSVC)
	add_executable(spotify-qt WIN32 ${SRC_FILES} ${LIB_SRC} res.qrc)
elseif (APPLE)
	add_executable(spotify-qt MACOSX_BUNDLE ${SRC_FILES} ${LIB_SRC} res.qrc)
	set_target_properties(spotify-qt PROPERTIES
		BUNDLE True
		MACOSX_BUNDLE_GUI_IDENTIFIER com.kraxarn.spotify-qt
		MACOSX_BUNDLE_BUNDLE_NAME spotify-qt
		MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
		MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/cmake/customtemplate.plist.in)
else ()
	add_executable(spotify-qt ${SRC_FILES} ${LIB_SRC} res.qrc)
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
target_compile_definitions(spotify-qt PRIVATE BUILD_TYPE="${CMAKE_BUILD_TYPE}")

# Set install targets for 'make install'
if (UNIX)
	# Install icons and desktop shortcut on unix-like
	install(FILES res/logo/spotify-qt.svg DESTINATION share/icons/hicolor/scalable/apps)
	install(FILES res/app/spotify-qt.desktop DESTINATION share/applications)
elseif (APPLE)
	# macOS bundling
	install(TARGETS spotify-qt BUNDLE DESTINATION bin)
endif ()

# Don't show console window under windows
if (WIN32 AND NOT MSVC)
	target_link_options(spotify-qt PRIVATE -mwindows)
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

# Enable all compiler warnings under GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	target_compile_options(spotify-qt PRIVATE -Wall -Wextra)
endif ()

# D-Bus support is optional
if (Qt5DBus_FOUND)
	target_compile_definitions(spotify-qt PRIVATE USE_DBUS)
	target_link_libraries(spotify-qt PRIVATE Qt5::DBus)
endif ()

# KCrash support
if (KF5Crash_FOUND AND NOT USE_QT6)
	target_compile_definitions(spotify-qt PRIVATE USE_KCRASH)
	target_link_libraries(spotify-qt PRIVATE KF5::Crash)
endif ()
