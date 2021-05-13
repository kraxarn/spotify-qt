cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX VERSION 0.1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB MAIN_SRC "src/*.cpp")
file(GLOB SUB_SRC "src/*/*.cpp")
# Header files
file(GLOB MAIN_INC "include/lib/*.hpp")
file(GLOB SUB_INC "include/lib/*/*.hpp")
# Add all files
add_library(spotify-qt-lib STATIC ${MAIN_SRC} ${SUB_SRC} ${MAIN_INC} ${SUB_INC})

# Main source folder has the headers
target_include_directories(spotify-qt-lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Optional SQLite cache support
set(LIB_USE_SQLITE ON)
if (LIB_USE_SQLITE)
	find_package(PkgConfig QUIET)
	if (PKG_CONFIG_FOUND)
		pkg_check_modules(SQLite QUIET sqlite3)
		if (SQLite_FOUND)
			message(STATUS "SQLite found")
			target_link_libraries(spotify-qt-lib PRIVATE ${SQLite_LIBRARIES})
			target_compile_definitions(spotify-qt-lib PRIVATE LIB_SQLITE)
		endif ()
	endif ()
endif ()

# Unit testing
if (USE_TESTS)
	add_subdirectory(test)
endif ()