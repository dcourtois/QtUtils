#
# This library is meant to be used by just `add_subdirectory ("path_to_this_folder")`
# It will define a QtUtils library that you can then add to your target using `target_add_libraries`
# It will provide all required include paths, etc.
#
# Just before adding this directory, you can define a few variables to customize the
# built library:
#
# - QT_UTILS_NAMESPACE	: By default, no namespace is used. But since the names of the
#						  classes defined in this lib might clash with yours, you can
#						  set this variable to a non-empty string, which will then
#						  be used as the namespace for all QtUtils classes.
#

#
# Check options
#
if (DEFINED QT_UTILS_NAMESPACE)
	set (QT_UTILS_NAMESPACE_USED 1)
else ()
	set (QT_UTILS_NAMESPACE_USED 0)
endif ()

#
# Override automoc options, in case the parent scope was using a precompiled header.
#
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOMOC_MOC_OPTIONS "")
set (CMAKE_AUTORCC OFF)

#
# Get the required Qt libraries
#
find_package (Qt5 5
	COMPONENTS
		Quick
		Widgets
	REQUIRED
)

#
# The library
#
add_library (QtUtils
	File.cpp
	File.h
	HttpRequest.cpp
	HttpRequest.h
	Job.cpp
	Job.h
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

		# Platform
		$<$<PLATFORM_ID:Windows>:WINDOWS>
		$<$<PLATFORM_ID:Linux>:LINUX>
		$<$<PLATFORM_ID:Darwin>:MACOS>

	PUBLIC
		# Namespace
		$<${QT_UTILS_NAMESPACE_USED}:QT_UTILS_NAMESPACE=${QT_UTILS_NAMESPACE}>
)
