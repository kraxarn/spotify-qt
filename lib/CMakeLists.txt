cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX VERSION 0.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create library
add_library(spotify-qt-lib STATIC)

# Collect main source files
add_subdirectory(include)
add_subdirectory(src)

# Optional Qt implementations
if (LIB_QT_IMPL)
	add_subdirectory(qt)
endif ()

# Main source folder has the headers
target_include_directories(spotify-qt-lib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include")

# Version macros
target_compile_definitions(spotify-qt-lib PUBLIC LIB_VERSION="v${PROJECT_VERSION}")

# Check if using GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Used by crash handler
	target_compile_definitions(spotify-qt-lib PRIVATE IS_GNU_CXX)
	# Enable all compiler warnings
	target_compile_options(spotify-qt-lib PRIVATE -Wall -Wextra)
endif ()

# Link optional libraries
if (LIB_QT_LIBRARIES)
	# This can be avoided by using CMP0079
	# but it only supports CMake 3.13+
	target_link_libraries(spotify-qt-lib PRIVATE ${LIB_QT_LIBRARIES})
endif ()

# Unit testing
if (USE_TESTS)
	add_subdirectory(test)
endif ()
