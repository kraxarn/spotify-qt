# CMake doesn't allow linking of libraries outside of main CMake file
# Create a new project to link against later
project(spotify-qt-lib-third-party)
add_library(${PROJECT_NAME} STATIC)

option(USE_KEYCHAIN "Keychain support" ON)

# SQLite cache support
option(USE_DB_CACHE "Use SQLite based cache" ON)
option(USE_BUNDLED_SQLITE "Force use bundled SQLite" OFF)

# Bundled SQLite version
set(BUNDLED_SQLITE_VERSION "3.41.1")
set(BUNDLED_SQLITE_HASH "02c0b580272ff9160b4c70459f85fd1a1c602a9f081c2c375b32ffa68483f55e")

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
		# Get version for URL
		string(REPLACE "." ";" sqlite_version "${BUNDLED_SQLITE_VERSION}")
		list(GET sqlite_version 0 sqlite_version_major)
		list(GET sqlite_version 1 sqlite_version_minor)
		list(GET sqlite_version 2 sqlite_version_patch)
		math(EXPR sqlite_version "${sqlite_version_major} * 1000000")
		math(EXPR sqlite_version "${sqlite_version} + ${sqlite_version_minor} * 10000")
		math(EXPR sqlite_version "${sqlite_version} + ${sqlite_version_patch} * 100")

		FetchContent_Declare(
			sqlite_amalgamation
			URL "https://www.sqlite.org/2023/sqlite-amalgamation-${sqlite_version}.zip"
			URL_HASH "SHA3_256=${BUNDLED_SQLITE_HASH}"
			DOWNLOAD_EXTRACT_TIMESTAMP true
		)
		message(STATUS "Downloading SQLite3, please wait...")
		FetchContent_MakeAvailable(sqlite_amalgamation)

		enable_language(C)
		target_link_libraries(${PROJECT_NAME} INTERFACE ${CMAKE_DL_LIBS})

		target_include_directories(${PROJECT_NAME} PRIVATE
			${sqlite_amalgamation_SOURCE_DIR})

		target_sources(${PROJECT_NAME} PRIVATE
			${sqlite_amalgamation_SOURCE_DIR}/sqlite3.c)
	endif ()

	message(STATUS "Using SQLite ${BUNDLED_SQLITE_VERSION} (bundled)")
endif ()

# Generate SQL model
if (USE_DB_CACHE)
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/../res/model_cache.sql" model_cache)
	set(MODEL_CACHE_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/../res/model_cache.hpp")
	file(WRITE "${MODEL_CACHE_HEADER}" "#pragma once\n\nconst char* model_cache_sql = R\"(")
	file(APPEND "${MODEL_CACHE_HEADER}" "${model_cache}")
	file(APPEND "${MODEL_CACHE_HEADER}" ")\";")
endif ()