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
	include(FetchContent OPTIONAL RESULT_VARIABLE FetchContent_supported)
	if (FetchContent_supported STREQUAL "NOTFOUND")
		message(STATUS "FetchContent not supported")
	else ()
		FetchContent_Declare(
			sqlite_amalgamation
			URL https://www.sqlite.org/2022/sqlite-amalgamation-3390400.zip
			URL_HASH SHA3_256=96894cf2c57f76ccbeda41aa6ff0ce1f6b093deced8335801d38ddb34ec9690e
			DOWNLOAD_EXTRACT_TIMESTAMP true
		)
		message(STATUS "Downloading SQLite3, please wait...")
		FetchContent_MakeAvailable(sqlite_amalgamation)

		target_include_directories(${PROJECT_NAME} PRIVATE
			${sqlite_amalgamation_SOURCE_DIR})

		target_sources(${PROJECT_NAME} PRIVATE
			${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c)
	endif ()

	message(STATUS "Using SQLite (bundled)")
endif ()

# Generate SQL model
if (USE_DB_CACHE)
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../res/model_cache.sql" model_cache)
	string(PREPEND model_cache "#pragma once\nconst char* model_cache_sql = R\"(")
	string(APPEND model_cache ")\";")
	file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/../res/model_cache.hpp" "${model_cache}")
endif ()