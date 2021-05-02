cmake_minimum_required(VERSION 3.5)
project(spotify-qt-common LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB MAIN_SRC "${PROJECT_SOURCE_DIR}/src/*.[hc]pp")
add_library(spotify-qt-common STATIC ${MAIN_SRC})

# Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
target_link_libraries(spotify-qt-common PRIVATE Qt${QT_VERSION_MAJOR}::Core)
