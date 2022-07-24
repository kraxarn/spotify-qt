# Find requested Qt version
option(USE_QT6 "Force Qt 6" OFF)
option(USE_QT5 "Force Qt 5" OFF)

if (USE_QT5)
	# Forcing Qt 5
	set(QT_VERSION_MAJOR 5)
elseif (USE_QT6)
	# Forcing Qt 6
	set(QT_VERSION_MAJOR 6)
else ()
	# Auto detect, but prefer Qt 5
	find_package(Qt5 COMPONENTS Core QUIET)
	if (Qt5Core_FOUND)
		set(QT_VERSION_MAJOR 5)
	else ()
		find_package(Qt6 COMPONENTS Core QUIET)
		if (Qt6Core_FOUND)
			set(QT_VERSION_MAJOR 6)
		endif ()
	endif ()
endif ()

# Check if Qt was found
if (QT_VERSION_MAJOR)
	message(STATUS "Found Qt ${QT_VERSION_MAJOR}")
else ()
	message(FATAL_ERROR "Qt not found, please install Qt 5/6 first")
endif ()

# Qt 6 requires C++17 under MSVC
if (QT_VERSION_MAJOR EQUAL 6 AND MSVC)
	set(CMAKE_CXX_STANDARD 17)
endif ()