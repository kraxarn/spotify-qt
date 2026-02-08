cmake_minimum_required(VERSION 3.10)

project(spotify-qt-lib-test)

add_executable(spotify-qt-lib-test
	src/main.cpp
	src/base64tests.cpp
	src/datetimetests.cpp
	src/enumstests.cpp
	src/fmttests.cpp
	src/formattests.cpp
	src/imagetests.cpp
	src/jsontests.cpp
	src/logtests.cpp
	src/resulttests.cpp
	src/settingstests.cpp
	src/spotify/tracktests.cpp
	src/spotify/utiltests.cpp
	src/stopwatchtests.cpp
	src/stringstests.cpp
	src/systemtests.cpp
	src/uritests.cpp
	src/vectortests.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
	spotify-qt-lib
	Qt${QT_VERSION_MAJOR}::Core
)
