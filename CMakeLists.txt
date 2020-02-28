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
find_package(Qt5 COMPONENTS Core Widgets Network Gui Svg REQUIRED)

# Glob source files
file(GLOB MAIN_SRC "src/*.[hc]pp")
file(GLOB SPOTIFY_SRC "src/spotify/*.[hc]pp")

## Add all source files to main executable
add_executable(spotify-qt ${MAIN_SRC} ${SPOTIFY_SRC} res.qrc)

# Link Qt/KDE
target_link_libraries(spotify-qt PRIVATE
		Qt5::Core
		Qt5::Widgets
		Qt5::Network
		Qt5::Gui
		Qt5::Svg)