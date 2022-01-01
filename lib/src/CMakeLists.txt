add_subdirectory(cache)
add_subdirectory(crash)
add_subdirectory(github)
add_subdirectory(search)
add_subdirectory(spotify)
add_subdirectory(spotifyapi)

message(STATUS "Building: ${PROJECT_NAME}")

target_sources(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/base64.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/datetime.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/developermode.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/format.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/httpclient.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/image.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/json.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/logmessage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lyrics.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/random.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/settings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/strings.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/system.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/uri.cpp)