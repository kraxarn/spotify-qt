cmake_minimum_required(VERSION 3.9)

project(spotify-qt-lib-test)

add_executable(spotify-qt-lib-test
	src/main.cpp
	src/base64tests.cpp
	src/datetimetests.cpp
	src/enumstests.cpp
	src/formattests.cpp
	src/imagetests.cpp
	src/jsontests.cpp
	src/logtests.cpp
	src/optionaltests.cpp
	src/settingstests.cpp
	src/spotifyapitests.cpp
	src/stringstests.cpp
	src/systemtests.cpp
	src/vectortests.cpp
	src/uritests.cpp)

target_link_libraries(spotify-qt-lib-test PRIVATE spotify-qt-lib)
