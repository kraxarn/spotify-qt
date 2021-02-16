cmake_minimum_required(VERSION 3.9)

project(spotify-qt-lib-test)

file(GLOB MAIN_SRC "src/*.[hc]pp")
add_executable(spotify-qt-lib-test ${MAIN_SRC})

target_link_libraries(spotify-qt-lib-test PRIVATE spotify-qt-lib)
