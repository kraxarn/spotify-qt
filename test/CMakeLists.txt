find_package(Qt5Test REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

enable_testing(true)

file(GLOB TEST_SRC "*.[hc]pp")
add_executable(spotify-qt-tests ${TEST_SRC})
add_test(NAME spotify-qt-tests COMMAND spotify-qt-tests)

target_link_libraries(spotify-qt-tests PRIVATE
		spotify-qt
		Qt5::Test
		Qt5::Widgets
		Qt5::Network)