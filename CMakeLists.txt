#
# This file is meant to be used by just `add_subdirectory ("path_to_this_file")`
# It will define a QtUtils library that you can then add to your target using `target_add_libraries`
# It will provide all required include paths, etc.
#

#
# A few options to configure the library
#
option (QT_UTILS_GENERATE_DOCUMENTATION "OFF by default. When ON, a documentation of the API is generated." OFF)

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
	File.cpp
	File.h
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

#
# Documentation
#
if (${QT_UTILS_GENERATE_DOCUMENTATION})

	# check that doxygen is available
	find_program (DOXYGEN doxygen)
	if ("${DOXYGEN}" STREQUAL "DOXYGEN-NOTFOUND")
		message (WARNING "QT_UTILS_GENERATE_DOCUMENTATION option is ON, but Doxygen wasn't found.\nPlease install it: http://www.doxygen.nl/download.html.")
		set (DOXYGEN OFF)
	else ()
		message (STATUS "Found Doxygen: ${DOXYGEN}")
		set (DOXYGEN ON)
	endif ()

	# check that moxygen is available
	find_program (MOXYGEN moxygen)
	if ("${MOXYGEN}" STREQUAL "MOXYGEN-NOTFOUND")
		message (WARNING "QT_UTILS_GENERATE_DOCUMENTATION option is ON, but Moxygen wasn't found.\nPlease install it: `npm install moxygen -g`.")
		set (MOXYGEN OFF)
	else ()
		message (STATUS "Found Moxygen: ${MOXYGEN}")
		set (MOXYGEN ON)
	endif ()

	if (${DOXYGEN} AND ${MOXYGEN})

		# generate the Doxygen configuration file
		configure_file (
			${CMAKE_CURRENT_SOURCE_DIR}/Doxygen.cfg.in
			${CMAKE_CURRENT_BINARY_DIR}/Doxygen.cfg
			@ONLY
		)

		# add the target that will generate the documentation
		add_custom_target (QtUtilsDocumentation
			ALL
			DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/Doxygen.cfg
			COMMENT "Generating documentation..."
			COMMAND doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxygen.cfg
			COMMAND moxygen --anchors --output ${CMAKE_CURRENT_SOURCE_DIR}/API.md ${CMAKE_CURRENT_BINARY_DIR}/doc/xml
		)

		# make it depend on QtUtils so that the doc is built last
		add_dependencies (QtUtilsDocumentation QtUtils)

	endif ()

endif ()
