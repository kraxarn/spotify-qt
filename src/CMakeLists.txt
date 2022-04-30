add_subdirectory(dialog)
add_subdirectory(ipc)
add_subdirectory(keyring)
add_subdirectory(lib)
add_subdirectory(list)
add_subdirectory(listitem)
add_subdirectory(mediaplayer)
add_subdirectory(menu)
add_subdirectory(settingspage)
add_subdirectory(spotify)
add_subdirectory(spotifyclient)
add_subdirectory(util)
add_subdirectory(view)
add_subdirectory(widget)

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp)