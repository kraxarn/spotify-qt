# Qt
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Source files
file(GLOB MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# Header files
file(GLOB MAIN_INC "${CMAKE_CURRENT_SOURCE_DIR}/include/lib/qt/*.hpp")
file(GLOB SUB_INC "${CMAKE_CURRENT_SOURCE_DIR}/include/lib/qt/*/*.hpp")
# Add all files
set(LIB_QT_SOURCES ${MAIN_SRC} ${MAIN_INC} ${SUB_INC})

# Qt
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Network REQUIRED)
set(LIB_QT_LIBRARIES Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Network)

# Headers
set(LIB_QT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include")
