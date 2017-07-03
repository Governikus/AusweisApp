MACRO(FIND_DVCS _dest)
	IF(EXISTS "${_dest}/.hg")
		FIND_HOST_PACKAGE(Hg)
		IF(HG_FOUND)
			SET(DVCS_EXECUTABLE ${HG_EXECUTABLE})
		ENDIF()

	ELSEIF(EXISTS "${_dest}/.git")
		FIND_HOST_PACKAGE(Git)
		IF(GIT_FOUND)
			SET(DVCS_EXECUTABLE ${GIT_EXECUTABLE})
		ENDIF()
	ENDIF()

	IF(DVCS_EXECUTABLE)
		SET(DVCS_FOUND TRUE)
	ENDIF()
ENDMACRO()


FUNCTION(DVCS_EXECUTE _out)
	EXECUTE_PROCESS(COMMAND ${DVCS_EXECUTABLE} ${ARGN}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE _output
		RESULT_VARIABLE _result
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	IF(${_result} EQUAL 0)
		SET(${_out} "${_output}" PARENT_SCOPE)
	ENDIF()
ENDFUNCTION()


FUNCTION(DVCS_CALL _name _sep ${ARGN})
	DVCS_EXECUTE(_exec ${ARGN})
	IF(DEFINED _exec)
		MESSAGE(STATUS "DVCS ${_name}: ${_exec}")
		SET(dvcs_${_name} ${_exec} PARENT_SCOPE)
		IF(NOT "${_sep}" STREQUAL "")
			SET(VERSION_DVCS ${VERSION_DVCS}${_sep}${_exec} PARENT_SCOPE)
		ENDIF()
	ENDIF()
ENDFUNCTION()


MACRO(CHECK_DVCS)
	SET(VERSION_DVCS ${PROJECT_VERSION})

	IF(HG_FOUND)
		DVCS_CALL("tag" "" id -t)
	ELSEIF(GIT_FOUND)
		DVCS_CALL("tag" "" name-rev --tags --name-only HEAD)
	ENDIF()
	IF(NOT "${dvcs_tag}" STREQUAL "tip" AND NOT "${dvcs_tag}" STREQUAL "" AND NOT "${dvcs_tag}" STREQUAL "undefined")
		IF(NOT dvcs_tag STREQUAL PROJECT_VERSION)
			MESSAGE(FATAL_ERROR "DVCS Tag and defined PROJECT_VERSION is not equal")
		ENDIF()

	ELSE()
		IF(HG_FOUND)
			DVCS_EXECUTE(dvcs_distance log -r ${PROJECT_VERSION}::. --template 1)
			STRING(LENGTH "${dvcs_distance}" dvcs_distance)
			MESSAGE(STATUS "DVCS distance: ${dvcs_distance}")
			SET(VERSION_DVCS ${VERSION_DVCS}+${dvcs_distance})
		ELSEIF(GIT_FOUND)
			DVCS_CALL("distance" "+" rev-list --count ${PROJECT_VERSION}...HEAD)
		ENDIF()

		GET_DVCS_INFO()
	ENDIF()
ENDMACRO()

MACRO(GET_DVCS_INFO)
	IF(HG_FOUND)
		DVCS_CALL("branch" "-" branch)
	ELSEIF(GIT_FOUND)
		DVCS_CALL("branch" "-" rev-parse --abbrev-ref HEAD)
	ENDIF()

	IF(HG_FOUND)
		DVCS_CALL("phase" "" log -r . -T {phase})
		IF(DEFINED dvcs_phase)
			IF("${dvcs_phase}" STREQUAL "public")
				UNSET(dvcs_phase)
			ELSE()
				SET(VERSION_DVCS ${VERSION_DVCS}-${dvcs_phase})
			ENDIF()
		ENDIF()
	ENDIF()

	IF(HG_FOUND)
		DVCS_CALL("revision" "-" id -i)
	ELSEIF(GIT_FOUND)
		DVCS_CALL("revision" "-" rev-parse --verify --short HEAD)
		DVCS_EXECUTE(dvcs_dirty_wc diff --shortstat)
		IF(DEFINED dvcs_dirty_wc)
			MESSAGE(STATUS "DVCS: dirty working copy")
			SET(dvcs_revision ${dvcs_revision}+)
			SET(VERSION_DVCS ${VERSION_DVCS}+)
		ENDIF()
	ENDIF()
ENDMACRO()


FIND_DVCS(${PROJECT_SOURCE_DIR})
IF(DVCS_FOUND)
	OPTION(ENABLE_DVCS "Check consistency of version/tag and get additional revision data" true)
	IF(ENABLE_DVCS)
		CHECK_DVCS()
	ENDIF()
ENDIF()
