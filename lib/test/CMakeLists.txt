cmake_minimum_required(VERSION 3.9)

project(spotify-qt-lib-test)
include(GoogleTest)

add_executable(spotify-qt-lib-test
		test/datetime.cpp)

target_link_libraries(spotify-qt-lib-test PRIVATE gtest gtest_main spotify-qt-lib)
