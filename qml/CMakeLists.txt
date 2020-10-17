project(spotify-qt-quick)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Quick QuickControls2 Widgets DBus WebEngine REQUIRED)

include_directories("..")

file(GLOB QML_SRC "${PROJECT_SOURCE_DIR}/src/*.[hc]pp")
add_library(spotify-qt-quick STATIC ${QML_SRC})

target_link_libraries(spotify-qt-quick PRIVATE
		Qt5::Core
		Qt5::Quick
		Qt5::QuickControls2
		# TODO: Some stuff like dialogs still depend on Widgets (v3.4)
		Qt5::Widgets
		# TODO: D-Bus is still required (v3.1)
		Qt5::DBus
		# TODO: WebEngine doesn't actually work on mobile (v3.1)
		Qt5::WebEngine)

find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=0 --tags
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			OUTPUT_VARIABLE GIT_TAG
			OUTPUT_STRIP_TRAILING_WHITESPACE)
elseif(NOT GIT_TAG)
	message(STATUS "warning: git or source code dir not found")
	set(GIT_TAG "v0.0")
endif()

target_compile_definitions(spotify-qt-quick PRIVATE LIB_VERSION="${GIT_TAG}")