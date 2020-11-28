cmake_minimum_required(VERSION 3.5)
project(spotify-qt-lib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
file(GLOB MAIN_SRC "*.[hc]pp")
file(GLOB SUB_SRC "*/*.[hc]pp")
add_library(spotify-qt-lib STATIC ${MAIN_SRC} ${SUB_SRC})

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