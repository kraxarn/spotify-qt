cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX VERSION 0.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional Qt implementations
if (LIB_QT_IMPL)
	add_subdirectory(qt)
endif ()

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
	${LIB_QT_HEADERS})

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
	target_link_libraries(spotify-qt-lib PRIVATE ${LIB_QT_LIBRARIES})
endif ()

# Unit testing
if (USE_TESTS)
	add_subdirectory(test)
endif ()
