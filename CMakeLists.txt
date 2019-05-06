#
# This file is meant to be used by just `add_subdirectory ("path_to_this_file")`
# It will define a QtUtils library that you can then add to your target using `target_add_libraries`
# It will provide all required include paths, etc.
#

#
# Override automoc options, in case the parent scope was using a precompiled header.
#
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOMOC_MOC_OPTIONS "")
set (CMAKE_AUTORCC OFF)

#
# Get the required Qt libraries
#
find_package (Qt5 5.12
	COMPONENTS
		Quick
		Widgets
	REQUIRED
)

#
# The library
#
add_library (QtUtils
	QuickView.cpp
	QuickView.h
	Settings.cpp
	Settings.h
	Utils.h
)

#
# The include directories
#
target_include_directories (QtUtils
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}
)

#
# The Qt libraries
#
target_link_libraries (QtUtils
	PUBLIC
		Qt5::Quick
		Qt5::Widgets
)

#
# compiler's requirements
#
target_compile_features (QtUtils
	PUBLIC
		cxx_std_17
)

#
# Some definitions
#
target_compile_definitions (QtUtils
	PRIVATE
		# Disable every possible debug things from Qt when in Release
		$<$<NOT:$<CONFIG:Release>>:QT_QML_DEBUG>
		$<$<CONFIG:Release>:QT_NO_DEBUG>
		$<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT>
		$<$<CONFIG:Release>:QT_NO_INFO_OUTPUT>
		$<$<CONFIG:Release>:QT_NO_WARNING_OUTPUT>
)
