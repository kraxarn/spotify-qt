# Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Add source files
add_subdirectory(src)

# Add headers
target_include_directories(${PROJECT_NAME} PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include")

# Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)
set(LIB_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network PARENT_SCOPE)
