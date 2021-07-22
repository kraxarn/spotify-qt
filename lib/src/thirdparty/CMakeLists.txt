project(spotify-qt-lib-third-party)

# EasyLogging++

add_library(${PROJECT_NAME} STATIC
	${CMAKE_CURRENT_SOURCE_DIR}/easyloggingpp/easylogging++.cc)

target_include_directories(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../../include/thirdparty")
