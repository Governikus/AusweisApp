cmake_minimum_required(VERSION 3.25)

###########################################
#### Usage: cmake -P ci.cmake -- cmake args
###########################################

if(NOT CMAKE_SCRIPT_MODE_FILE)
	message(FATAL_ERROR "Script mode is required")
endif()

cmake_path(GET CMAKE_SCRIPT_MODE_FILE PARENT_PATH CMAKE_SOURCE_DIR)
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

if(CMAKE_COMMAND MATCHES " ")
	set(CMAKE_COMMAND cmake)
endif()
if(CMAKE_CTEST_COMMAND MATCHES " ")
	set(CMAKE_CTEST_COMMAND ctest)
endif()

set(CMAKE_DIR "${CMAKE_SOURCE_DIR}/cmake")
set(CI_DIR "${CMAKE_SOURCE_DIR}/ci")
set(CI_RUNNER_DIR "${CI_DIR}/runner")
file(GLOB RUNNER_FILES "${CI_RUNNER_DIR}/*.cmake")

if(NOT DEFINED SPLIT)
	if(DEFINED ENV{SPLIT})
		set(SPLIT $ENV{SPLIT})
	else()
		set(SPLIT ON)
	endif()
endif()

if((DEFINED ENV{REVIEWBOARD_REVIEW_REQUEST} OR DEFINED ENV{PATCH_URL}) AND NOT DEFINED REVIEW)
	set(REVIEW ON)
elseif(DEFINED ENV{RELEASE} AND NOT DEFINED RELEASE)
	set(RELEASE $ENV{RELEASE})
endif()

if(NOT REVIEW AND NOT RELEASE)
	set(DAILY ON)
endif()


function(GET_PARAMS _params _definitions)
	foreach(_arg RANGE ${CMAKE_ARGC})
		if("${CMAKE_ARGV${_arg}}" STREQUAL "--")
			math(EXPR delimiter "${_arg}+1")
			break()
		elseif("${CMAKE_ARGV${_arg}}" MATCHES "^-D")
			list(APPEND definition ${CMAKE_ARGV${_arg}})
		endif()
	endforeach()

	if(DEFINED delimiter)
		foreach(_arg RANGE ${delimiter} ${CMAKE_ARGC})
			list(APPEND param ${CMAKE_ARGV${_arg}})
		endforeach()
		set(${_params} "${param}" PARENT_SCOPE)
	endif()

	if(DEFINED definition)
		set(${_definitions} "${definition}" PARENT_SCOPE)
	endif()
endfunction()


function(CALC_CHECKSUM _out_checksum)
	set(CHECKSUM_FILES ${CMAKE_SCRIPT_MODE_FILE} ${RUNNER_FILES})
	if(ARGN)
		list(APPEND CHECKSUM_FILES ${ARGN})
	endif()

	foreach(file ${CHECKSUM_FILES})
		file(MD5 "${file}" _hash)
		string(MD5 hashes "${_hash}${hashes}")
	endforeach()

	set(${_out_checksum} ${hashes} PARENT_SCOPE)
endfunction()


function(CALC_REPATCH_CHECKSUM _out_checksum _out_repatch)
	CALC_CHECKSUM(hashes)
	if(SPLIT AND PATCHED AND NOT ${_out_checksum} STREQUAL hashes)
		set(${_out_repatch} ON PARENT_SCOPE)
	endif()
	set(${_out_checksum} ${hashes} PARENT_SCOPE)
endfunction()


function(SETUP_RUNNER)
	list(APPEND CMAKE_MODULE_PATH ${CI_RUNNER_DIR})
	foreach(entry ${RUNNER_FILES})
		cmake_path(GET entry STEM filename)

		if(IS_DIRECTORY "${entry}")
			message(FATAL_ERROR "directories are not supported: ${filename}")
		endif()

		message(VERBOSE "Setup runner: ${filename}")
		include(${entry})
	endforeach()
	list(POP_BACK CMAKE_MODULE_PATH)
endfunction()


function(CALL_SCRIPT)
	if(NOT SCRIPT)
		message(FATAL_ERROR "SCRIPT is undefined")
	endif()

	FETCH_VERSION()
	SET_TEMPLATES()
	if(EXISTS ${T_BUILD_DIR})
		step(${CMAKE_COMMAND} -E rm -r ${T_BUILD_DIR})
	endif()

	if(NOT EXISTS "${CACHE_DIR}")
		file(MAKE_DIRECTORY "${CACHE_DIR}")
	endif()

	cmake_path(RELATIVE_PATH SCRIPT BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" OUTPUT_VARIABLE SCRIPT_PATH)
	message(STATUS "Use SCRIPT: ${SCRIPT_PATH}")
	block()
		include(${SCRIPT})
	endblock()
endfunction()


function(FETCH_SCRIPT _out_script _out_path _path _names)
	file(GLOB scripts "${_path}/*.cmake")
	foreach(part ${_names})
		foreach(entry ${scripts})
			cmake_path(GET entry STEM filename)
			if(part STREQUAL filename)
				if(IS_DIRECTORY "${entry}")
					FETCH_SCRIPT(script ${_out_path} "${entry}" "${_names}")
				else()
					set(script ${entry})
				endif()

				if(script)
					set(${_out_script} ${script} PARENT_SCOPE)
					if(IS_DIRECTORY "${entry}")
						set(${_out_path} ${${_out_path}} ${entry} PARENT_SCOPE)
					endif()
					return()
				endif()
			endif()
		endforeach()
	endforeach()
endfunction()


macro(PARSE_FROM_NAME)
	if(DEFINED ENV{CI_JOB_NAME} AND NOT NAME)
		set(NAME $ENV{CI_JOB_NAME})
	endif()
	if(NOT DEFINED ENV{CI} AND NOT NAME)
		set(NAME ${CMAKE_HOST_SYSTEM_NAME})
		message(STATUS "Automatic detection for developer: ${NAME}")
	endif()

	if(NAME)
		string(REGEX MATCHALL "[A-Za-z0-9.-]+" NAMES "${NAME}")
		message(STATUS "Use NAME: ${NAME}")
		message(STATUS "Use NAMES: ${NAMES}")

		if(NOT SCRIPT)
			set(CI_SCRIPTS_DIR "${CI_DIR}/scripts")
			FETCH_SCRIPT(SCRIPT CMAKE_MODULE_PATH "${CI_SCRIPTS_DIR}" "${NAMES}")
			list(APPEND CMAKE_MODULE_PATH ${CI_SCRIPTS_DIR})
		endif()
	endif()
endmacro()


function(RUN)
	SETUP_RUNNER()
	PARSE_FROM_NAME()
	GET_PARAMS(PARAMS DEFINITIONS)
	CALC_REPATCH_CHECKSUM(CHECKSUM REPATCH)

	if(NOT REPATCH AND (NOT REVIEW OR PATCHED))
		CALL_SCRIPT()
	endif()

	if(REVIEW)
		PROCESS_REVIEW()
	endif()
endfunction()


RUN()
