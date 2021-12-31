add_subdirectory(artist)
add_subdirectory(context)
add_subdirectory(search)
add_subdirectory(sidepanel)

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/audiofeatures.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cacheview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/clienthandlerlogview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/configview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/crashes.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/debugview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/logview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/lyricsview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/maincontent.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/maintoolbar.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/splashscreen.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/systeminfoview.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/trayicon.cpp)