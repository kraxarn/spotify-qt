cmake_minimum_required(VERSION 3.5)

project(spotify-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(Qt5 COMPONENTS Core Widgets Network Gui Svg DBus REQUIRED)

# Glob source files
file(GLOB MAIN_SRC "src/*.[hc]pp")
file(GLOB SPOTIFY_SRC "src/spotify/*.[hc]pp")
file(GLOB MEDIAPLAYER_SRC "src/mediaplayer/*.[hc]pp")

# Add all source files to main executable
if(ANDROID)
    add_library(spotify-qt SHARED ${MAIN_SRC} ${SPOTIFY_SRC} ${MEDIAPLAYER_SRC} res.qrc)
else()
    add_executable(spotify-qt ${MAIN_SRC} ${SPOTIFY_SRC} ${MEDIAPLAYER_SRC} res.qrc)
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
install(TARGETS spotify-qt CONFIGURATIONS Release RUNTIME DESTINATION bin)

# Link Qt/KDE
target_link_libraries(spotify-qt PRIVATE
		Qt5::Core
		Qt5::Widgets
		Qt5::Network
		Qt5::Gui
		Qt5::Svg
		Qt5::DBus)
