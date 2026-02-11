cmake_minimum_required(VERSION 3.16)

project(spotify-qt-lib LANGUAGES CXX VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_TESTS "Build with unit tests" OFF)

option(USE_DEPRECATED_WARNINGS "Enable warnings for deprecated declarations" OFF)

# Source files
file(GLOB MAIN_SRC "src/*.cpp")
file(GLOB SUB_SRC "src/*/*.cpp")
# Header files
file(GLOB MAIN_INC "include/lib/*.hpp")
file(GLOB SUB_INC "include/lib/*/*.hpp")
# Add all files
add_library(spotify-qt-lib STATIC ${MAIN_SRC} ${SUB_SRC} ${MAIN_INC} ${SUB_INC} ${LIB_QT_SOURCES})

# Main source folder has the headers
target_include_directories(spotify-qt-lib PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
	"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/include"
)

# Third-party libraries
add_subdirectory(thirdparty)

# Version macros
target_compile_definitions(spotify-qt-lib PUBLIC LIB_VERSION="v${PROJECT_VERSION}")

# Qt configs
target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_CAST_FROM_ASCII)
target_compile_definitions(${PROJECT_NAME} PRIVATE QT_NO_CAST_TO_ASCII)

if (NOT USE_DEPRECATED_WARNINGS)
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
	endif ()
endif ()

# Check if using GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	# Used by crash handler
	target_compile_definitions(spotify-qt-lib PRIVATE IS_GNU_CXX)
	# Enable all compiler warnings
	target_compile_options(spotify-qt-lib PRIVATE -Wall -Wextra)
endif ()

# Disable "secure" warnings on MSVC
if (MSVC)
	target_compile_definitions(spotify-qt-lib PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

# Link Qt
target_link_libraries(spotify-qt-lib PRIVATE
	Qt${QT_VERSION_MAJOR}::Core
	Qt${QT_VERSION_MAJOR}::Network
)

# Unit testing
if (USE_TESTS)
	add_subdirectory(test)
endif ()
