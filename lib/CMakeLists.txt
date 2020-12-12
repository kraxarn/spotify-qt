cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB MAIN_SRC "*.[hc]pp")
file(GLOB SUB_SRC "*/*.[hc]pp")
add_library(spotify-qt-lib STATIC ${MAIN_SRC} ${SUB_SRC})