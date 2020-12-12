cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Glob source files
file(GLOB MAIN_SRC "*.[hc]pp")
file(GLOB ENUM_SRC "enum/*.[hc]pp")
file(GLOB SPOTIFY_SRC "spotify/*.[hc]pp")
file(GLOB SPOTIFY_CORE_SRC "spotify/core/*.[hc]pp")
file(GLOB SPOTIFY_API_SRC "spotify/core/api/*.[hc]pp")
file(GLOB UTIL_SRC "util/*.[hc]pp")

# Add source files
add_library(spotify-qt-lib STATIC
		${MAIN_SRC}
		${ENUM_SRC}
		${SPOTIFY_SRC}
		${SPOTIFY_CORE_SRC}
		${SPOTIFY_API_SRC}
		${UTIL_SRC})

# Temporary for C++17 filesystem for use in Settings
target_link_libraries(spotify-qt-lib PRIVATE stdc++fs)

# For async
find_package(Threads REQUIRED)
target_link_libraries(spotify-qt-lib PRIVATE Threads::Threads)

# OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(spotify-qt-lib PRIVATE OpenSSL::SSL OpenSSL::Crypto)
target_compile_definitions(spotify-qt-lib PRIVATE CPPHTTPLIB_OPENSSL_SUPPORT)

# OpenSSL (Windows)
if (MINGW)
	target_link_libraries(spotify-qt-lib PRIVATE ws2_32 crypt32)
endif ()