# Source files
file(GLOB MAIN_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# Header files
file(GLOB MAIN_INC "${CMAKE_CURRENT_SOURCE_DIR}/include/lib/curl/*.hpp")
# Add all files
set(LIB_CURL_SOURCES ${MAIN_SRC} ${MAIN_INC} ${SUB_INC} PARENT_SCOPE)

# Threading
find_package(Threads REQUIRED)

# https://github.com/whoshuu/cpr
find_path(CPR_INCLUDE_DIR NAMES cpr.h)
find_library(CPR_LIBRARY NAMES cpr)
set(LIB_CURL_LIBRARIES cpr pthread PARENT_SCOPE)

# Headers
set(LIB_CURL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include" PARENT_SCOPE)
