cmake_minimum_required(VERSION 3.5)

project(spotify-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if we should include Qt Quick
if(USE_QT_QUICK)
	set(QT_QUICK_PKG Quick QuickControls2)
	set(QT_QUICK_LINK Qt5::Quick Qt5::QuickControls2)
	file(GLOB QML_SRC "qml/src/*.[hc]pp")
	add_compile_definitions(USE_QT_QUICK)
endif()

# Find Qt
find_package(Qt5 COMPONENTS Core Widgets Network Gui Svg DBus ${QT_QUICK_PKG} REQUIRED)

# Glob source files
file(GLOB MAIN_SRC "src/*.[hc]pp")
file(GLOB SUB_SRC "src/*/*.[hc]pp")

# Add all source files to main executable
if(ANDROID)
    add_library(spotify-qt SHARED ${MAIN_SRC} ${SUB_SRC} ${QML_SRC} res.qrc)
else()
    add_executable(spotify-qt ${MAIN_SRC} ${SUB_SRC} ${QML_SRC} ${LIB_SRC} res.qrc)
endif()

# Get version from Git
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			OUTPUT_VARIABLE GIT_TAG
			OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(NOT GIT_TAG)
	message(STATUS "warning: git or source code dir not found")
	set(GIT_TAG "v0.0")
endif()

target_compile_definitions(spotify-qt PRIVATE APP_VERSION="${GIT_TAG}")

# Set install targets for 'make install'
install(TARGETS spotify-qt RUNTIME DESTINATION bin)
if (UNIX)
	# Install icons and desktop shortcut on unix-like
	install(FILES res/logo/spotify-qt.svg DESTINATION share/icons/hicolor/scalable/apps)
	install(FILES res/app/spotify-qt.desktop DESTINATION share/applications)
endif()

# Don't show console window under windows
if (WIN32)
	target_link_options(spotify-qt PRIVATE -mwindows)
endif ()

# Link Qt/KDE
target_link_libraries(spotify-qt PRIVATE
		Qt5::Core
		Qt5::Widgets
		Qt5::Network
		Qt5::Gui
		Qt5::Svg
		Qt5::DBus
		${QT_QUICK_LINK})