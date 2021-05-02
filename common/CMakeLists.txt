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
file(GLOB MAIN_SRC "src/*.cpp")
file(GLOB MAIN_INC "include/common/*.hpp")
add_library(spotify-qt-common STATIC ${MAIN_SRC} ${MAIN_INC})

# Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)
target_link_libraries(spotify-qt-common PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network)

# Include src directory
target_include_directories(spotify-qt-common PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
