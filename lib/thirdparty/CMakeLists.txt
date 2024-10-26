# CMake doesn't allow linking of libraries outside of main CMake file
# Create a new project to link against later
project(spotify-qt-lib-third-party)
add_library(${PROJECT_NAME} STATIC)

# Some libraries assume headers are in the root
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/include/thirdparty)