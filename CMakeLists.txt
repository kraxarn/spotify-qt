cmake_minimum_required(VERSION 3.5)

project(spotify-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

add_executable(spotify-qt
		src/main.cpp
		src/mainwindow.cpp
		src/spotify/spotify.cpp
		src/settings.cpp)
		src/spotify/track.cpp)

target_link_libraries(spotify-qt PRIVATE Qt5::Widgets Qt5::Network)