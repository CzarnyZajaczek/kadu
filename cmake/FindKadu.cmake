# Try to find Kadu development headers
# Once done this will define
#
#  KADU_FOUND - system has Kadu headers
#  KADU_INCLUDE_DIR - the Kadu include directory
#
# Based on FindQutIM by Ruslan Nigmatullin, <euroelessar@gmail.com>
# Copyright (c) 2009, Ruslan Nigmatullin, <euroelessar@gmail.com>
# Copyrignt (c) 2011, Rafał 'Vogel' Malinowski <vogel@kadu.im>

# libraries
find_package (Qt4 4.7.0 REQUIRED)
set (QT_USE_QTXML 1)
set (QT_USE_QTNETWORK 1)
set (QT_USE_QTWEBKIT 1)
set (QT_USE_QTSVG 1)
if (UNIX AND NOT APPLE)
	set (QT_USE_QTDBUS 1)
endif (UNIX AND NOT APPLE)
include (${QT_USE_FILE})

if (KADU_INCLUDE_DIR)
	# Already in cache, be silent
	set (KADU_FIND_QUIETLY TRUE)
endif (KADU_INCLUDE_DIR)

if (NOT KADU_DO_NOT_FIND)
	find_path (KADU_INCLUDE_DIR
		kadu-core/kadu-application.h
		PATHS ${KADU_SEARCH_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/../../
		PATH_SUFFIXES include/kadu
	)

	if (KADU_INCLUDE_DIR)
		if (NOT KADU_FIND_QUIETLY)
			message (STATUS "Found Kadu headers: ${KADU_INCLUDE_DIR}/kadu")
		endif (NOT KADU_FIND_QUIETLY)
	else (KADU_INCLUDE_DIR)
		if (KADU_FIND_REQUIRED)
			message (FATAL_ERROR "Could not find Kadu development headers")
		else (KADU_FIND_REQUIRED)
			message (STATUS "Could not find Kadu development headers")
		endif (KADU_FIND_REQUIRED)
    endif (KADU_INCLUDE_DIR)
endif (NOT KADU_DO_NOT_FIND)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

string (REPLACE "-O3" "-O2" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
string (REPLACE "-O3" "-O2" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

option (ENABLE_DEVELOPER_BUILD "Turn on some features helpful during development process (does not necessarily turn on debug symbols in binaries)" OFF)

if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_DEVELOPER_BUILD OR WIN32)
	set (DEBUG_ENABLED 1)
	add_definitions (-DDEBUG_ENABLED)
endif (CMAKE_BUILD_TYPE STREQUAL "Debug" OR ENABLE_DEVELOPER_BUILD OR WIN32)

if (MSVC)
	add_definitions (/D_CRT_SECURE_NO_WARNINGS=1)
endif (MSVC)

if (MINGW)
	# override cmake bug/feature?
	set (CMAKE_SHARED_LIBRARY_PREFIX "")
endif (MINGW)

# warnings
if (UNIX AND NOT APPLE)
	if (ENABLE_DEVELOPER_BUILD)
		set (CMAKE_C_FLAGS "-Werror ${CMAKE_C_FLAGS}")
		set (CMAKE_CXX_FLAGS "-Werror ${CMAKE_CXX_FLAGS}")
	endif (ENABLE_DEVELOPER_BUILD)

	set (CMAKE_C_FLAGS "-W -Wall ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")
elseif (APPLE)
	set (CMAKE_C_FLAGS "-W -Wall ${CMAKE_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "-W -Wall ${CMAKE_CXX_FLAGS}")
elseif (MINGW)
	set (CMAKE_C_FLAGS "-ffast-math -mtune=i686")
	set (CMAKE_CXX_FLAGS "-ffast-math -mtune=i686")
endif (UNIX AND NOT APPLE)

if (NOT KADU_DATADIR)
	set (KADU_DATADIR ${CMAKE_INSTALL_PREFIX}/share)
endif (NOT KADU_DATADIR)
if (NOT KADU_LIBDIR)
	set (KADU_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})
endif (NOT KADU_LIBDIR)
if (NOT KADU_BINDIR)
	set (KADU_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)
endif (NOT KADU_BINDIR)
if (NOT KADU_INSTALL_INCLUDE_DIR)
	set (KADU_INSTALL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include)
endif (NOT KADU_INSTALL_INCLUDE_DIR)
if (NOT KADU_CMAKE_MODULES_DIR)
	set (KADU_CMAKE_MODULES_DIR ${CMAKE_ROOT}/Modules)
endif (NOT KADU_CMAKE_MODULES_DIR)

if (WIN32)
	set (KADU_DATA_DIR ".")
	set (KADU_PLUGINS_LIBDIR "plugins")
	set (KADU_BIN_DIR ".")
else (WIN32)
	set (KADU_DATA_DIR "share/kadu")
	set (KADU_PLUGINS_LIBDIR lib${LIB_SUFFIX}/kadu/plugins)
	set (KADU_BIN_DIR "bin")
endif (WIN32)

set (KADU_PLUGINS_DIR ${KADU_DATA_DIR}/plugins)

macro (kadu_api_directories INCLUDE_DIR)
	foreach (ARG ${ARGN})
		file (GLOB API_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${ARG}/*.h)
		install (FILES ${API_HEADERS} DESTINATION ${KADU_INSTALL_INCLUDE_DIR}/kadu/${INCLUDE_DIR}/${ARG})
	endforeach (ARG)
endmacro (kadu_api_directories)

macro (kadu_configuration)
	install (FILES ${ARGN}
		DESTINATION ${KADU_DATA_DIR}/configuration
	)
endmacro (kadu_configuration)

macro (kadu_plugin_configuration)
	install (FILES ${ARGN}
		DESTINATION ${KADU_PLUGINS_DIR}/configuration
	)
endmacro (kadu_plugin_configuration)

macro (kadu_plugin_desc PLUGIN_DESC)
	install (FILES ${ARGN}
		DESTINATION ${KADU_PLUGINS_DIR}
	)
endmacro (kadu_plugin_desc)

macro (kadu_plugin)
	set (VARIABLE_NAME "PLUGIN_NAME")

	set (PLUGIN_NAME "")
	set (PLUGIN_SOURCES "")
	set (PLUGIN_MOC_SOURCES "")
	set (PLUGIN_TRANSLATION_SOURCES "")
	set (PLUGIN_CONFIGURATION_FILES "")
	set (PLUGIN_DATA_FILES "")
	set (PLUGIN_DATA_DIRECTORY "")
	set (PLUGIN_BUILDDEF "")
	set (PLUGIN_DEPENDENCIES "")
	set (PLUGIN_LIBRARIES "")

	include_directories (".")
	include_directories ("${KADU_INCLUDE_DIR}/kadu")
	include_directories ("${KADU_INCLUDE_DIR}/kadu/core")

	foreach (ARG ${ARGV})
		if ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
			set (PLUGIN_NAME ${ARG})
			set (VARIABLE_NAME "")
		elseif (ARG MATCHES "(PLUGIN_SOURCES|PLUGIN_MOC_SOURCES|PLUGIN_TRANSLATION_SOURCES|PLUGIN_CONFIGURATION_FILES|PLUGIN_DATA_FILES|PLUGIN_DATA_DIRECTORY|PLUGIN_BUILDDEF|PLUGIN_DEPENDENCIES|PLUGIN_LIBRARIES)")
			set (VARIABLE_NAME ${ARG})
		elseif (VARIABLE_NAME STREQUAL "")
			message (FATAL_ERROR "Invalid invocation of kadu_plugin macro")
		else ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
			set (${VARIABLE_NAME} "${${VARIABLE_NAME}};${ARG}")
		endif ("${VARIABLE_NAME}" STREQUAL "PLUGIN_NAME")
	endforeach (ARG)

	if (WIN32)
		include_directories (${CMAKE_SOURCE_DIR}/plugins)

		# opisy .desc sa w iso-8859-2
		set (CMAKE_RC_FLAGS "-c 28592")
		# wygeneruj plik z wersja modulu
		set (PLUGIN_SOURCES ${PLUGIN_SOURCES} ${PLUGIN_NAME}.rc)
		add_custom_command (OUTPUT ${PLUGIN_NAME}.rc
			COMMAND ${CMAKE_SOURCE_DIR}/modules/modver.bat ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.desc ${PLUGIN_NAME}.rc
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${PLUGIN_NAME}.desc
			COMMENT "Building RC source ${PLUGIN_NAME}.rc"
		)

		# naglowki do SDK
		install (DIRECTORY "." DESTINATION ${KADU_DATA_DIR}/kadu-sdk/include/kadu-plugins/${PLUGIN_NAME} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE)
	endif (WIN32)

	install (FILES ${PLUGIN_NAME}.desc
		DESTINATION ${KADU_PLUGINS_DIR}
	)

	if (PLUGIN_MOC_SOURCES)
		qt4_wrap_cpp (PLUGIN_MOC_FILES ${PLUGIN_MOC_SOURCES})
	endif (PLUGIN_MOC_SOURCES)

	if (PLUGIN_TRANSLATION_SOURCES)
		qt4_add_translation (PLUGIN_TRANSLATION_FILES ${PLUGIN_TRANSLATION_SOURCES})
	endif (PLUGIN_TRANSLATION_SOURCES)

	kadu_plugin_configuration (${PLUGIN_CONFIGURATION_FILES})

	install (FILES ${PLUGIN_TRANSLATION_FILES}
		DESTINATION ${KADU_PLUGINS_DIR}/translations
	)

	if (NOT "${PLUGIN_DATA_FILES}" STREQUAL "")
		install (FILES ${PLUGIN_DATA_FILES}
			DESTINATION ${KADU_PLUGINS_DIR}/data/${PLUGIN_NAME}
		)
	endif (NOT "${PLUGIN_DATA_FILES}" STREQUAL "")

	if (NOT "${PLUGIN_DATA_DIRECTORY}" STREQUAL "")
		install (DIRECTORY ${PLUGIN_DATA_DIRECTORY}
			DESTINATION ${KADU_PLUGINS_DIR}/data/${PLUGIN_NAME}
		)
	endif (NOT "${PLUGIN_DATA_DIRECTORY}" STREQUAL "")

	add_library (${PLUGIN_NAME} SHARED ${PLUGIN_SOURCES} ${PLUGIN_MOC_FILES})
	add_custom_target (${PLUGIN_NAME}-translations DEPENDS ${PLUGIN_TRANSLATION_FILES})

	add_dependencies (${PLUGIN_NAME} ${PLUGIN_NAME}-translations)

	if (NOT ${PLUGIN_BUILDDEF} STREQUAL "")
		set_target_properties (${PLUGIN_NAME} PROPERTIES COMPILE_DEFINITIONS ${PLUGIN_BUILDDEF})
	endif (NOT ${PLUGIN_BUILDDEF} STREQUAL "")

	if (NOT "${PLUGIN_LIBRARIES}" STREQUAL "")
		target_link_libraries (${PLUGIN_NAME} ${PLUGIN_LIBRARIES})
	endif (NOT "${PLUGIN_LIBRARIES}" STREQUAL "")

	if (WIN32)
		target_link_libraries (${PLUGIN_NAME} kadu_core ${PLUGIN_DEPENDENCIES} ${QT_LIBRARIES})
		install (TARGETS ${PLUGIN_NAME} ARCHIVE DESTINATION ${KADU_DATA_DIR}/kadu-sdk/lib)
	endif (WIN32)
	if (APPLE)
		set_target_properties (${PLUGIN_NAME} PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
	endif (APPLE)
	install (TARGETS ${PLUGIN_NAME} RUNTIME DESTINATION ${KADU_PLUGINS_LIBDIR} LIBRARY DESTINATION ${KADU_PLUGINS_LIBDIR})
endmacro (kadu_plugin)
