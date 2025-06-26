cmake_minimum_required(VERSION 3.10.0)

#############################################
# Usage: cmake -DCOMPONENT=XYZ -P patch.cmake
#############################################

get_filename_component(CMAKE_SOURCE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)

function(PATCH_SOURCES_EXECUTE _patchlist)
	foreach(_patch ${_patchlist})
		get_filename_component(filename "${_patch}" NAME)
		message(STATUS "Apply patch: ${filename}")
		execute_process(COMMAND "${PATCH_CMD}" ${PATCH_OPTIONS} ${_patch} RESULT_VARIABLE _result)

		if(NOT _result EQUAL 0)
			message(FATAL_ERROR "Patch failed with exit code: ${_result}")
		endif()
	endforeach()
endfunction()

function(FETCH_PATCHES _out _component)
	set(PATCHES_DIR ${CMAKE_SOURCE_DIR}/patches)
	file(GLOB PATCHES "${PATCHES_DIR}/${_component}*.patch")
	set(${_out} ${PATCHES} PARENT_SCOPE)
endfunction()

function(FETCH_PATCHES_NAMES _out _component)
	FETCH_PATCHES(tmplist ${_component})
	foreach(_patch ${tmplist})
		get_filename_component(filename "${_patch}" NAME)
		list(APPEND PATCHES ${filename})
	endforeach()
	set(${_out} ${PATCHES} PARENT_SCOPE)
endfunction()

function(PATCH_SOURCES _component)
	FETCH_PATCHES(PATCHES ${_component})
	PATCH_SOURCES_EXECUTE("${PATCHES}")
endfunction()

macro(SEARCH_PATCH_CMD)
	if(NOT DEFINED ENV{FORCE_PATCH_PY})
		find_program(PATCH_CMD patch CMAKE_FIND_ROOT_PATH_BOTH)
	endif()

	if(PATCH_CMD)
		set(PATCH_OPTIONS -p1 -i)
		if(WIN32)
			set(PATCH_OPTIONS --verbose ${PATCH_OPTIONS})
		endif()
		message(STATUS "Using 'patch' command... ${PATCH_CMD}")
	else()
		message(STATUS "Cannot find 'patch' command... using patch.py")
		find_package(Python REQUIRED)
		set(PATCH_CMD ${Python_EXECUTABLE})
		set(PATCH_OPTIONS ${CMAKE_SOURCE_DIR}/patch.py -p1 --debug -v)
	endif()
endmacro()


if(CMAKE_SCRIPT_MODE_FILE)
	if(COMPONENT MATCHES "qt|openssl")
		SEARCH_PATCH_CMD()
		PATCH_SOURCES(${COMPONENT})
	else()
		message(FATAL_ERROR "Component unknown: ${COMPONENT}")
	endif()
endif()
