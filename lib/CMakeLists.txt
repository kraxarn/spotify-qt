cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB MAIN_SRC "lib/*.[hc]pp")
file(GLOB SUB_SRC "lib/*/*.[hc]pp")
add_library(spotify-qt-lib STATIC ${MAIN_SRC} ${SUB_SRC})

# Main source folder has the headers
target_include_directories(spotify-qt-lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Unit testing
if (USE_TESTS)
	add_subdirectory(test)
endif ()