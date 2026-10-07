macro(FIND_DVCS _dest)
	if(EXISTS "${_dest}/.hg")
		find_package(Hg)
		if(HG_FOUND)
			set(DVCS_EXECUTABLE ${HG_EXECUTABLE})
		endif()

	elseif(EXISTS "${_dest}/.git")
		find_package(Git)
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
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
		OUTPUT_VARIABLE _output
		RESULT_VARIABLE _result
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	if(${_result} EQUAL 0)
		set(${_out} "${_output}" PARENT_SCOPE)
	endif()
endfunction()


function(DVCS_CALL _name ${ARGN})
	DVCS_EXECUTE(_exec ${ARGN})
	if(DEFINED _exec)
		message(STATUS "DVCS ${_name}: ${_exec}")
		set(dvcs_${_name} ${_exec} PARENT_SCOPE)
	endif()
endfunction()


macro(GET_DVCS_INFO)
	set(USE_ARTIFACT_DEV_VERSION ON)

	if(DEFINED PROJECT_VERSION)
		if(HG_FOUND)
			DVCS_CALL("tag" id -t)
		elseif(GIT_FOUND)
			DVCS_CALL("tag" tag -l --points-at HEAD)
		endif()
		if(NOT "${dvcs_tag}" STREQUAL "tip" AND NOT "${dvcs_tag}" STREQUAL "" AND NOT "${dvcs_tag}" STREQUAL "undefined")
			if(NOT dvcs_tag STREQUAL PROJECT_VERSION)
				message(FATAL_ERROR "DVCS Tag and defined PROJECT_VERSION is not equal")
			endif()
			set(USE_ARTIFACT_DEV_VERSION OFF)
		else()
			if(HG_FOUND)
				DVCS_EXECUTE(dvcs_distance log -r ${PROJECT_VERSION}::. --template 1)
				string(LENGTH "${dvcs_distance}" dvcs_distance)
				message(STATUS "DVCS distance: ${dvcs_distance}")
			elseif(GIT_FOUND)
				DVCS_CALL("distance" rev-list --count ${PROJECT_VERSION}...HEAD)
			endif()
		endif()
	endif()

	if(HG_FOUND)
		DVCS_CALL("branch" branch)
	elseif(GIT_FOUND)
		DVCS_CALL("branch" rev-parse --abbrev-ref HEAD)
	endif()

	if(HG_FOUND)
		DVCS_CALL("revision" id -i)
	elseif(GIT_FOUND)
		DVCS_CALL("revision" rev-parse --verify --short HEAD)
	endif()

	if(HG_FOUND)
		DVCS_CALL("phase" log -r . -T {phase})
		if(DEFINED dvcs_phase)
			if("${dvcs_phase}" STREQUAL "public")
				if("${dvcs_revision}" MATCHES "\\+")
					set(dvcs_phase "draft")
					message(STATUS "DVCS phase: ${dvcs_phase} (override)")
				else()
					unset(dvcs_phase)
				endif()
			endif()
		endif()
	endif()

	if(DEFINED PROJECT_NAME AND DEFINED PROJECT_VERSION)
		if(PROJECT_VERSION_MINOR LESS 100 AND PROJECT_VERSION_PATCH LESS 100 AND NOT USE_ARTIFACT_DEV_VERSION)
			set(IS_FINAL_VERSION ON)
		else()
			set(IS_FINAL_VERSION OFF)
		endif()
		message(STATUS "DVCS final: ${IS_FINAL_VERSION}")

		if(DEFINED dvcs_distance)
			set(ARTIFACT_DEV_VERSION ${ARTIFACT_DEV_VERSION}+${dvcs_distance})
		endif()

		if(DEFINED dvcs_branch)
			set(ARTIFACT_DEV_VERSION ${ARTIFACT_DEV_VERSION}-${dvcs_branch})
		endif()

		if(DEFINED dvcs_phase)
			set(ARTIFACT_DEV_VERSION ${ARTIFACT_DEV_VERSION}-${dvcs_phase})
		endif()

		if(DEFINED dvcs_revision)
			set(ARTIFACT_DEV_VERSION ${ARTIFACT_DEV_VERSION}-${dvcs_revision})
		endif()


		set(ARTIFACT_FILENAME ${PROJECT_NAME}-${PROJECT_VERSION})
		if(CMAKE_ANDROID_ARCH_ABI)
			set(ARTIFACT_FILENAME ${ARTIFACT_FILENAME}-${CMAKE_ANDROID_ARCH_ABI})
		endif()

		set(ARTIFACT_VERSION ${PROJECT_VERSION})

		if(USE_ARTIFACT_DEV_VERSION)
			set(ARTIFACT_FILENAME ${ARTIFACT_FILENAME}${ARTIFACT_DEV_VERSION})
			set(ARTIFACT_VERSION ${ARTIFACT_VERSION}${ARTIFACT_DEV_VERSION})
		endif()

		if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
			set(ARTIFACT_FILENAME ${ARTIFACT_FILENAME}-debug)
		endif()

		if(USE_APPSTORE_PROFILE)
			set(ARTIFACT_FILENAME ${ARTIFACT_FILENAME}-appstore)
		endif()
	endif()
endmacro()

FIND_DVCS(${CMAKE_SOURCE_DIR})
if(DVCS_FOUND)
	option(ENABLE_DVCS "Check consistency of version/tag and get additional revision data" true)
	if(ENABLE_DVCS)
		GET_DVCS_INFO()
	endif()
endif()
