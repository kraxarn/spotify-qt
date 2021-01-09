cmake_minimum_required(VERSION 3.9)

project(spotify-qt-lib-test)

add_executable(spotify-qt-lib-test
		test/datetimetests.cpp)

target_include_directories(spotify-qt-lib-test PRIVATE "../thirdparty")

target_link_libraries(spotify-qt-lib-test PRIVATE spotify-qt-lib)
