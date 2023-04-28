macro(FIND_DVCS _dest)
	if(EXISTS "${_dest}/.hg")
		FIND_HOST_PACKAGE(Hg)
		if(HG_FOUND)
			set(DVCS_EXECUTABLE ${HG_EXECUTABLE})
		endif()

	elseif(EXISTS "${_dest}/.git")
		FIND_HOST_PACKAGE(Git)
		if(GIT_FOUND)
			set(DVCS_EXECUTABLE ${GIT_EXECUTABLE})
		endif()
	endif()

	if(DVCS_EXECUTABLE)
		set(DVCS_FOUND TRUE)
	endif()
endmacro()


function(DVCS_EXECUTE _out)
	execute_process(COMMAND ${DVCS_EXECUTABLE} ${ARGN}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		OUTPUT_VARIABLE _output
		RESULT_VARIABLE _result
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	if(${_result} EQUAL 0)
		set(${_out} "${_output}" PARENT_SCOPE)
	endif()
endfunction()


function(DVCS_CALL _name _sep ${ARGN})
	DVCS_EXECUTE(_exec ${ARGN})
	if(DEFINED _exec)
		message(STATUS "DVCS ${_name}: ${_exec}")
		set(dvcs_${_name} ${_exec} PARENT_SCOPE)
		if(NOT "${_sep}" STREQUAL "")
			set(VERSION_DVCS ${VERSION_DVCS}${_sep}${_exec} PARENT_SCOPE)
		endif()
	endif()
endfunction()


macro(CHECK_DVCS)
	if(HG_FOUND)
		DVCS_CALL("tag" "" id -t)
	elseif(GIT_FOUND)
		DVCS_CALL("tag" "" tag -l --points-at HEAD)
	endif()
	if(NOT "${dvcs_tag}" STREQUAL "tip" AND NOT "${dvcs_tag}" STREQUAL "" AND NOT "${dvcs_tag}" STREQUAL "undefined")
		if(NOT dvcs_tag STREQUAL PROJECT_VERSION)
			message(FATAL_ERROR "DVCS Tag and defined PROJECT_VERSION is not equal")
		endif()

	else()
		if(HG_FOUND)
			DVCS_EXECUTE(dvcs_distance log -r ${PROJECT_VERSION}::. --template 1)
			string(LENGTH "${dvcs_distance}" dvcs_distance)
			message(STATUS "DVCS distance: ${dvcs_distance}")
			set(VERSION_DVCS ${VERSION_DVCS}+${dvcs_distance})
		elseif(GIT_FOUND)
			DVCS_CALL("distance" "+" rev-list --count ${PROJECT_VERSION}...HEAD)
		endif()

		GET_DVCS_INFO()
	endif()
endmacro()

macro(GET_DVCS_INFO)
	if(HG_FOUND)
		DVCS_CALL("branch" "-" branch)
	elseif(GIT_FOUND)
		DVCS_CALL("branch" "-" rev-parse --abbrev-ref HEAD)
	endif()

	if(HG_FOUND)
		DVCS_CALL("phase" "" log -r . -T {phase})
		if(DEFINED dvcs_phase)
			if("${dvcs_phase}" STREQUAL "public")
				unset(dvcs_phase)
			else()
				set(VERSION_DVCS ${VERSION_DVCS}-${dvcs_phase})
			endif()
		endif()
	endif()

	if(HG_FOUND)
		DVCS_CALL("revision" "-" id -i)
	elseif(GIT_FOUND)
		DVCS_CALL("revision" "-" rev-parse --verify --short HEAD)
	endif()
endmacro()

set(VERSION_DVCS ${PROJECT_VERSION})
FIND_DVCS(${PROJECT_SOURCE_DIR})
if(DVCS_FOUND)
	option(ENABLE_DVCS "Check consistency of version/tag and get additional revision data" true)
	if(ENABLE_DVCS)
		CHECK_DVCS()
	endif()
endif()

function(CHECK_VERSION _out)
	if(PROJECT_VERSION_MINOR)
		math(EXPR _odd "${PROJECT_VERSION_MINOR} % 2")
		if(_odd OR dvcs_revision)
			set(${_out} TRUE PARENT_SCOPE)
			return()
		endif()
	endif()

	set(${_out} FALSE PARENT_SCOPE)
endfunction()

CHECK_VERSION(IS_DEVELOPER_VERSION)
