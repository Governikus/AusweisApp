include_guard(GLOBAL)


macro(SET_TEMPLATES) # Provide some base templates for SCRIPTs
	if(DEFINED ENV{WORKSPACE})
		set(WORKSPACE $ENV{WORKSPACE})
	elseif(DEFINED ENV{CI_PROJECT_DIR})
		set(WORKSPACE $ENV{CI_PROJECT_DIR})
	else()
		set(WORKSPACE ${CMAKE_BINARY_DIR})
	endif()

	if(DEFINED ENV{CACHE_DIR})
		set(CACHE_DIR $ENV{CACHE_DIR})
	else()
		set(CACHE_DIR ${WORKSPACE}/cache)
	endif()

	set(T_TEMP "${WORKSPACE}/tmp")
	set(ENV{TMP} "${T_TEMP}")
	set(ENV{TEMP} "${T_TEMP}")
	set(ENV{TMPDIR} "${T_TEMP}")

	if(NOT EXISTS "${T_TEMP}")
		file(MAKE_DIRECTORY "${T_TEMP}")
	endif()

	if(DEFINED LIBS)
		set(T_LIBS_DIR ${LIBS})
	else()
		set(T_LIBS_DIR libs)
	endif()
	if(DEFINED BUILD)
		set(T_BUILD_DIR ${BUILD})
	else()
		set(T_BUILD_DIR build)
	endif()
	set(T_DIST_DIR ${T_BUILD_DIR}/dist)
	set(T_BUILD ${CMAKE_COMMAND} --build ${T_BUILD_DIR})
	set(T_TARGET ${T_BUILD} --target)
	set(T_CTEST ${CMAKE_CTEST_COMMAND} --test-dir ${T_BUILD_DIR} --output-on-failure)
	set(T_CMAKE_FRESH ${CMAKE_COMMAND} --fresh)
	set(T_CFG ${T_CMAKE_FRESH} ${PARAMS} -S ${CMAKE_SOURCE_DIR} -B ${T_BUILD_DIR})
	set(T_CFG_LIBS ${T_CMAKE_FRESH} ${PARAMS} -S ${CMAKE_SOURCE_DIR}/libs -B ${T_LIBS_DIR})
	set(T_CFG_LIBS_DEPS ${T_CMAKE_FRESH} -S ${CMAKE_SOURCE_DIR}/libs -B ${T_LIBS_DIR})
endmacro()


function(STEP)
	set(options NO_ECHO)
	set(oneValueArgs CHDIR OUTPUT RESULT PATH INPUT_FILE OUTPUT_FILE)
	set(multiValueArgs ENV)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(_PARAM_ENV)
		set(ENV_CMD ${CMAKE_COMMAND} -E env)
		foreach(env ${_PARAM_ENV})
			if(env MATCHES "^PATH=")
				message(FATAL_ERROR "Use explicit PATH parameter")
			endif()
			list(APPEND ENV_CMD "${env}")
		endforeach()
		list(APPEND ENV_CMD --)
	endif()

	if(NOT _PARAM_CHDIR OR (ECHO AND NOT _PARAM_NO_ECHO))
		set(_PARAM_CHDIR ${CMAKE_BINARY_DIR})
	endif()

	if(_PARAM_OUTPUT)
		set(OUTPUT_OPTION OUTPUT_VARIABLE)
		set(OUTPUT _output)
	endif()

	if(_PARAM_INPUT_FILE)
		set(INPUT_FILE_OPTION INPUT_FILE)
	endif()

	if(_PARAM_OUTPUT_FILE)
		set(OUTPUT_FILE_OPTION OUTPUT_FILE)
	endif()

	if(_PARAM_NO_ECHO OR NOT ECHO)
		set(COMMAND_ECHO STDOUT)
	else()
		set(ECHO_CMD ${CMAKE_COMMAND} -E echo)
		set(COMMAND_ECHO NONE)
	endif()

	if(_PARAM_PATH)
		set(TMP_PATH "$ENV{PATH}")
		if(CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
			set(SEP ";")
		else()
			set(SEP ":")
		endif()
		set(ENV{PATH} "${_PARAM_PATH}${SEP}$ENV{PATH}")
	endif()
	execute_process(COMMAND ${ECHO_CMD} ${ENV_CMD} ${_PARAM_UNPARSED_ARGUMENTS}
		RESULT_VARIABLE _result
		${OUTPUT_OPTION} ${OUTPUT}
		${INPUT_FILE_OPTION} ${_PARAM_INPUT_FILE}
		${OUTPUT_FILE_OPTION} ${_PARAM_OUTPUT_FILE}
		ECHO_OUTPUT_VARIABLE
		WORKING_DIRECTORY ${_PARAM_CHDIR}
		COMMAND_ECHO ${COMMAND_ECHO})
	if(TMP_PATH)
		set(ENV{PATH} "${TMP_PATH}")
		unset(TMP_PATH)
	endif()

	if(NOT ${_result} EQUAL 0)
		if(_PARAM_RESULT)
			set(${_PARAM_RESULT} ${_result} PARENT_SCOPE)
		else()
			message(FATAL_ERROR "Process failed: ${_result}")
		endif()
	endif()

	if(_PARAM_OUTPUT)
		set(${_PARAM_OUTPUT} ${${OUTPUT}} PARENT_SCOPE)
	endif()
endfunction()


function(hashsum)
	set(options NO_FILENAME)
	set(multiValueArgs ALGORITHM)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT DEFINED _PARAM_ALGORITHM)
		set(_PARAM_ALGORITHM SHA256)
	endif()

	foreach(entry ${_PARAM_UNPARSED_ARGUMENTS})
		foreach(algo ${_PARAM_ALGORITHM})
			string(TOLOWER "${algo}" fileending)
			file(${algo} ${entry} fileHash)
			cmake_path(GET entry FILENAME fileName)

			set(hashfile ${entry}.${fileending})
			if(${_PARAM_NO_FILENAME})
				set(content "${fileHash}\n")
			else()
				set(content "${fileHash}  ${fileName}\n")
			endif()
			file(CONFIGURE OUTPUT "${hashfile}" CONTENT "${content}" @ONLY NEWLINE_STYLE UNIX)
		endforeach()
	endforeach()
endfunction()


function(PROPAGATE_PARAM _param)
	string(REGEX MATCH "-D;?${_param}=([^ ]+)" _match "${PARAMS}")
	if(_match)
		set(${_param} "${CMAKE_MATCH_1}" PARENT_SCOPE)
	endif()
endfunction()


macro(FETCH_VERSION)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_DIR})
	PROPAGATE_PARAM(CMAKE_ANDROID_ARCH_ABI)
	include(Version)
	include(DVCS)
	list(POP_BACK CMAKE_MODULE_PATH)

	set(BRANCH ${dvcs_branch})
endmacro()
