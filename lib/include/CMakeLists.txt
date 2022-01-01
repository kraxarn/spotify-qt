add_subdirectory(lib)

target_sources(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/lib/settings/account.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/settings/general.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/settings/qt.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/lib/settings/spotify.hpp)