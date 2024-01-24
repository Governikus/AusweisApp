cmake_minimum_required(VERSION 3.8.0)

if(NOT CMAKE_SCRIPT_MODE_FILE)
	message(STATUS "Usage: cmake -P cmake/Notarization.cmake")
endif()

function(FETCH_REGEX _output _regex _data)
	string(REGEX MATCH "${_regex}" _unused "${_data}")

	if(CMAKE_MATCH_1)
		string(STRIP "${CMAKE_MATCH_1}" output)
		set(${_output} ${output} PARENT_SCOPE)
	endif()
endfunction()

find_program(XCRUN xcrun)
if(NOT XCRUN)
	message(FATAL_ERROR "Cannot find 'xcrun'")
endif()

file(GLOB DMG_FILE *.dmg)
list(LENGTH DMG_FILE DMG_FILE_LEN)
if(NOT DMG_FILE_LEN EQUAL 1)
	message(FATAL_ERROR "Cannot determine *.dmg file: ${CMAKE_BINARY_DIR}")
endif()
message(STATUS "Using DMG: ${DMG_FILE}")


set(keychain --keychain-profile "AC_PASSWORD")
execute_process(COMMAND ${XCRUN} notarytool submit ${keychain} ${DMG_FILE} OUTPUT_VARIABLE UUID_OUTPUT)

set(regex_uuid "id: ([-|0-9|a-z]+)")
FETCH_REGEX(UUID "${regex_uuid}" "${UUID_OUTPUT}")
if(UUID)
	message(STATUS "Fetched UUID: ${UUID}")
else()
	message(FATAL_ERROR "Cannot fetch UUID: ${UUID_OUTPUT}")
endif()

message(STATUS "Wait...")
execute_process(COMMAND ${XCRUN} notarytool wait ${UUID} ${keychain})

execute_process(COMMAND ${XCRUN} notarytool info ${UUID} ${keychain} OUTPUT_VARIABLE STATUS_OUTPUT)

set(regex_status "status: ([a-zA-Z| ]+)")
FETCH_REGEX(STATUS "${regex_status}" "${STATUS_OUTPUT}")

if(STATUS STREQUAL "Accepted")
	message(STATUS "Notarization succeeded...")
else()
	message(STATUS "Fetched Status: ${STATUS}")
	execute_process(COMMAND ${XCRUN} notarytool log ${UUID} ${keychain})
	message(FATAL_ERROR "Notarization failed:\n${STATUS_OUTPUT}")
endif()

execute_process(COMMAND ${XCRUN} stapler staple -v ${DMG_FILE})
execute_process(COMMAND ${XCRUN} stapler validate -v ${DMG_FILE})
