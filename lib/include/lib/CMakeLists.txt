add_subdirectory(cache)
add_subdirectory(crash)
add_subdirectory(enum)
add_subdirectory(github)
add_subdirectory(search)
add_subdirectory(spotify)

target_sources(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/base64.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/datetime.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/developermode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/format.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/httpclient.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/image.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/json.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/log.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/logmessage.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/lyrics.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/random.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/settings.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/strings.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/system.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/uri.hpp)