cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib-qt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB MAIN_SRC "src/*.cpp")
# Header files
file(GLOB MAIN_INC "include/lib/qt/*.hpp")
file(GLOB SUB_INC "include/lib/qt/*/*.hpp")
# Add all files
add_library(spotify-qt-lib-qt STATIC ${MAIN_SRC} ${MAIN_INC} ${SUB_INC})

# Headers
target_include_directories(spotify-qt-lib-qt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
