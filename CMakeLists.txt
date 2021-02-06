cmake_minimum_required(VERSION 3.5)

project(spotify-qt LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optional D-Bus support
if (NOT DEFINED USE_DBUS)
	set(USE_DBUS 1)
endif ()

# Optional Qt 6 support
if (DEFINED USE_QT6)
	set(QT_VERSION_MAJOR 6)
else()
	set(QT_VERSION_MAJOR 5)
endif()

# Prefer new OpenGL
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()

# Find Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Widgets Network Gui Svg REQUIRED)
if (USE_DBUS)
	find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS DBus QUIET)
endif ()
message(STATUS "Using Qt ${QT_VERSION_MAJOR}")

# Glob source files
file(GLOB MAIN_SRC "src/*.[hc]pp")
file(GLOB SUB_SRC "src/*/*.[hc]pp")

if (USE_QT_QUICK)
	set(QML_RES qml/res.qrc)
endif ()

# Add all source files to main executable
if (ANDROID)
	add_library(spotify-qt SHARED ${MAIN_SRC} ${SUB_SRC} ${QML_RES} res.qrc)
else ()
	add_executable(spotify-qt ${MAIN_SRC} ${SUB_SRC} ${QML_RES} ${LIB_SRC} res.qrc)
endif ()

# Get version from Git
find_package(Git QUIET)
if (GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
			OUTPUT_VARIABLE GIT_COMMIT
			OUTPUT_STRIP_TRAILING_WHITESPACE)
	target_compile_definitions(spotify-qt PRIVATE GIT_COMMIT="${GIT_COMMIT}")
endif ()

# Set install targets for 'make install'
install(TARGETS spotify-qt RUNTIME DESTINATION bin)
if (UNIX)
	# Install icons and desktop shortcut on unix-like
	install(FILES res/logo/spotify-qt.svg DESTINATION share/icons/hicolor/scalable/apps)
	install(FILES res/app/spotify-qt.desktop DESTINATION share/applications)
endif ()

# Don't show console window under windows
if (WIN32)
	target_link_options(spotify-qt PRIVATE -mwindows)
endif ()

# Link Qt
target_link_libraries(spotify-qt PRIVATE
		Qt${QT_VERSION_MAJOR}::Core
		Qt${QT_VERSION_MAJOR}::Widgets
		Qt${QT_VERSION_MAJOR}::Network
		Qt${QT_VERSION_MAJOR}::Gui
		Qt${QT_VERSION_MAJOR}::Svg)

# spotify-qt-lib
add_subdirectory(lib)
target_link_libraries(spotify-qt PRIVATE spotify-qt-lib)

# Check if we should include Qt Quick
if (USE_QT_QUICK)
	add_subdirectory(qml)
	target_compile_definitions(spotify-qt PRIVATE USE_QT_QUICK)
	target_link_libraries(spotify-qt PRIVATE spotify-qt-quick)
	set_target_properties(spotify-qt PROPERTIES OUTPUT_NAME spotify-qt-quick)
endif ()

# D-Bus support is optional
if (Qt5DBus_FOUND)
	target_compile_definitions(spotify-qt PRIVATE USE_DBUS)
	target_link_libraries(spotify-qt PRIVATE Qt5::DBus)
endif ()