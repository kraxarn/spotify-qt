# CMake doesn't allow linking of libraries outside of main CMake file
# Create a new project to link against later
project(spotify-qt-lib-third-party)
add_library(${PROJECT_NAME} STATIC)

option(USE_KEYCHAIN "Keychain support" ON)

# SQLite cache support
option(USE_DB_CACHE "Use SQLite based cache" ON)
option(USE_BUNDLED_SQLITE "Force use bundled SQLite" OFF)

# Some libraries assume headers are in the root
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include/thirdparty)

if (USE_KEYCHAIN)
	include(keychain.cmake)
endif ()

if (USE_DB_CACHE AND NOT USE_BUNDLED_SQLITE)
	find_package(PkgConfig QUIET)
	if (PkgConfig_FOUND)
		pkg_check_modules(SQLITE sqlite3 QUIET)
		if (SQLITE_FOUND)
			target_link_directories(${PROJECT_NAME} PRIVATE "${SQLITE_LIBRARY_DIRS}")
			target_include_directories(${PROJECT_NAME} PRIVATE "${SQLITE_INCLUDE_DIRS}")
			target_link_libraries(${PROJECT_NAME} PRIVATE "${SQLITE_LIBRARIES}")
			message(STATUS "Using SQLite ${SQLITE_VERSION} (system)")
		endif ()
	endif ()
endif ()

if (NOT SQLITE_VERSION)
	# sqlite3.h is expected in the root
	target_include_directories(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include/thirdparty)

	target_sources(${PROJECT_NAME} PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/src/sqlite3.c)

	message(STATUS "Using SQLite (bundled)")
endif ()