cmake_minimum_required(VERSION 3.5)

project(spotify-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Widgets Network Gui REQUIRED)

add_executable(spotify-qt
		src/main.cpp
		src/mainwindow.cpp
		src/settings.cpp
		src/spotify/spotify.cpp
		src/spotify/playlist.cpp
		src/spotify/track.cpp
		src/spotify/playback.hpp
		src/spotify/device.hpp)

target_link_libraries(spotify-qt PRIVATE Qt5::Core Qt5::Widgets Qt5::Network Qt5::Gui)