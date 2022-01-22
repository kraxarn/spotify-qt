target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include)

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src/fmt/format.cc
	${CMAKE_CURRENT_SOURCE_DIR}/src/fmt/os.cc)