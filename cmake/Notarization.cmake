CMAKE_MINIMUM_REQUIRED(VERSION 3.8.0)

IF(NOT CMAKE_SCRIPT_MODE_FILE)
	MESSAGE(STATUS "Usage: cmake -P cmake/Notarization.cmake")
ENDIF()

FUNCTION(FETCH_REGEX _output _regex _data)
	STRING(REGEX MATCH "${_regex}" _unused "${_data}")

	IF(CMAKE_MATCH_1)
		STRING(STRIP "${CMAKE_MATCH_1}" output)
		SET(${_output} ${output} PARENT_SCOPE)
	ENDIF()
ENDFUNCTION()

FIND_PROGRAM(XCRUN xcrun)
IF(NOT XCRUN)
	MESSAGE(FATAL_ERROR "Cannot find 'xcrun'")
ENDIF()

FILE(GLOB DMG_FILE *.dmg)
LIST(LENGTH DMG_FILE DMG_FILE_LEN)
IF(NOT DMG_FILE_LEN EQUAL 1)
	MESSAGE(FATAL_ERROR "Cannot determine *.dmg file: ${CMAKE_BINARY_DIR}")
ENDIF()
MESSAGE(STATUS "Using DMG: ${DMG_FILE}")

IF(NOT USER)
	SET(USER ausweisapp@governikus.com)
ENDIF()

EXECUTE_PROCESS(COMMAND ${XCRUN} altool -t osx --notarize-app --verbose -u ${USER} -p @env:PASSWORD --primary-bundle-id com.governikus.ausweisapp2 -f ${DMG_FILE}
					OUTPUT_VARIABLE UUID_OUTPUT)

SET(regex_uuid "RequestUUID = ([-|0-9|a-z]+)")
FETCH_REGEX(UUID "${regex_uuid}" "${UUID_OUTPUT}")
IF(UUID)
	MESSAGE(STATUS "Fetched UUID: ${UUID}")
ELSE()
	MESSAGE(FATAL_ERROR "Cannot fetch UUID: ${UUID_OUTPUT}")
ENDIF()

SET(regex_status "Status: ([a-z| ]+)")
WHILE(TRUE)
	MESSAGE(STATUS "Wait 30 seconds...")
	EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E sleep 30)

	EXECUTE_PROCESS(COMMAND ${XCRUN} altool -u ${USER} -p @env:PASSWORD --notarization-info ${UUID}
						OUTPUT_VARIABLE STATUS_OUTPUT)

	FETCH_REGEX(STATUS "${regex_status}" "${STATUS_OUTPUT}")

	IF(STATUS STREQUAL "success")
		MESSAGE(STATUS "Notarization succeeded...")
		BREAK()
	ELSEIF(STATUS STREQUAL "in progress")
		MESSAGE(STATUS "Waiting...\n${STATUS_OUTPUT}")
	ELSE()
		MESSAGE(STATUS "Fetched Status: ${STATUS}")
		MESSAGE(FATAL_ERROR "Notarization failed:\n${STATUS_OUTPUT}")
	ENDIF()
ENDWHILE()

EXECUTE_PROCESS(COMMAND ${XCRUN} stapler staple -v ${DMG_FILE})
EXECUTE_PROCESS(COMMAND ${XCRUN} stapler validate -v ${DMG_FILE})
