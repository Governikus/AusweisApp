INCLUDE(CheckCXXCompilerFlag)
INCLUDE(CMakeParseArguments)

FUNCTION(ADD_FLAG)
	SET(options NOQUOTES)
	SET(oneValueArgs)
	SET(multiValueArgs LINK VAR)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	IF(NOT _PARAM_VAR)
		SET(_PARAM_VAR CMAKE_CXX_FLAGS)
	ENDIF()

	IF(_PARAM_LINK)
		SET(CMAKE_REQUIRED_LIBRARIES ${_PARAM_LINK})
	ENDIF()

	FOREACH(flag ${_PARAM_UNPARSED_ARGUMENTS})
		STRING(REPLACE "-" "_" flagname ${flag})
		STRING(REPLACE " " "_" flagname ${flagname})

		CHECK_CXX_COMPILER_FLAG(${flag} ${flagname})
		IF(${flagname})
			FOREACH(var ${_PARAM_VAR})
				IF(_PARAM_NOQUOTES)
					SET(${var} ${${var}} ${flag} PARENT_SCOPE)
				ELSE()
					SET(${var} "${${var}} ${flag}" PARENT_SCOPE)
				ENDIF()
			ENDFOREACH()

			RETURN()
		ENDIF()
	ENDFOREACH()
ENDFUNCTION()


FUNCTION(GET_FILE_MATCHER _result_remove _result_keep)
	IF(NOT ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
		LIST(APPEND matcher_remove "_debug")
	ENDIF()

	IF(NOT ANDROID)
		LIST(APPEND matcher_remove "_android")
	ELSE()
		LIST(APPEND matcher_keep "_android")
	ENDIF()

	IF(NOT IOS)
		LIST(APPEND matcher_remove "_ios")
	ELSE()
		LIST(APPEND matcher_keep "_ios")
	ENDIF()

	IF(NOT MAC)
		LIST(APPEND matcher_remove "_osx")
	ELSE()
		LIST(APPEND matcher_keep "_osx")
	ENDIF()

	IF(NOT WIN32)
		LIST(APPEND matcher_remove "_win")
	ELSE()
		LIST(APPEND matcher_keep "_win")
	ENDIF()

	IF(NOT LINUX)
		LIST(APPEND matcher_remove "_linux")
	ELSE()
		LIST(APPEND matcher_keep "_linux")
	ENDIF()

	IF(NOT BSD)
		LIST(APPEND matcher_remove "_bsd")
	ELSE()
		LIST(APPEND matcher_keep "_bsd")
	ENDIF()

	SET(${_result_remove} ${matcher_remove} PARENT_SCOPE)
	SET(${_result_keep} ${matcher_keep} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(GET_FILE_EXTENSIONS _result)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs PATH)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	SET(FILE_EXTENSIONS *.cpp)
	OPTION(WITH_HEADER "Add header to target; only useful for some IDEs" OFF)
	IF(WITH_HEADER)
		LIST(APPEND FILE_EXTENSIONS *.h)
	ENDIF()
	IF(IOS)
		LIST(APPEND FILE_EXTENSIONS *.m *.mm)
	ENDIF()

	IF(_PARAM_PATH)
		FOREACH(path ${_PARAM_PATH})
			FOREACH(ext ${FILE_EXTENSIONS})
				LIST(APPEND tmpExt "${path}/${ext}")
			ENDFOREACH()
		ENDFOREACH()
		LIST(APPEND FILE_EXTENSIONS ${tmpExt})
	ENDIF()

	SET(${_result} ${FILE_EXTENSIONS} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(ADD_PLATFORM_LIBRARY _name)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs PATH)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	GET_FILE_EXTENSIONS(FILE_EXTENSIONS PATH ${_PARAM_PATH})
	FILE(GLOB_RECURSE FILES ${FILE_EXTENSIONS})

	GET_FILE_MATCHER(matcher_remove matcher_keep)

	FOREACH(file ${FILES})
		SET(keep FALSE)
		FOREACH(match ${matcher_keep})
			IF("${file}" MATCHES ${match})
				SET(keep TRUE)
				BREAK()
			ENDIF()
		ENDFOREACH()

		IF(NOT keep)
			FOREACH(match ${matcher_remove})
				IF("${file}" MATCHES ${match})
					LIST(REMOVE_ITEM FILES "${file}")
					BREAK()
				ELSEIF("${file}" MATCHES "_generic")
					LIST(REMOVE_ITEM FILES "${file}")
					LIST(APPEND GENERICS "${file}")
					BREAK()
				ENDIF()
			ENDFOREACH()
		ENDIF()
	ENDFOREACH()

	FOREACH(generic ${GENERICS})
		STRING(FIND "${generic}" "generic" position REVERSE)
		STRING(SUBSTRING "${generic}" 0 ${position} generic_basename)
		STRING(REPLACE "${CMAKE_SOURCE_DIR}" "" generic_basename "${generic_basename}")

		SET(found FALSE)
		FOREACH(file ${FILES})
			IF("${file}" MATCHES "${generic_basename}" AND NOT "${file}" MATCHES ".h$")
				SET(found TRUE)
				BREAK()
			ENDIF()
		ENDFOREACH()

		IF(NOT found)
			LIST(APPEND FILES "${generic}")
		ENDIF()
	ENDFOREACH()

	ADD_LIBRARY(${_name} ${FILES})
	TARGET_INCLUDE_DIRECTORIES(${_name} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
ENDFUNCTION()


# This FUNCTION is a workaround for a "bug" in cmake to use OBJECT LIBRARIES with IMPORTED TARGETS like Qt
# http://www.cmake.org/Bug/view.php?id=14778
FUNCTION(ADD_OBJECT_LIBRARY TargetName Files)
	ADD_LIBRARY(${TargetName} OBJECT ${Files})

	FOREACH(MODULE ${ARGN})
		TARGET_INCLUDE_DIRECTORIES(${TargetName} PRIVATE $<TARGET_PROPERTY:${MODULE},INTERFACE_INCLUDE_DIRECTORIES>)
		TARGET_COMPILE_DEFINITIONS(${TargetName} PRIVATE $<TARGET_PROPERTY:${MODULE},INTERFACE_COMPILE_DEFINITIONS>)
	ENDFOREACH()

	IF(Qt5_POSITION_INDEPENDENT_CODE)
		SET_TARGET_PROPERTIES(${TargetName} PROPERTIES POSITION_INDEPENDENT_CODE ON)
	ENDIF()
ENDFUNCTION()


FUNCTION(GET_QUOTED_STRING _dest _str _filename)
	IF(CMAKE_GENERATOR STREQUAL Xcode AND CMAKE_VERSION VERSION_LESS "3.5")
		SET(tmp_var \\\\"${_str}\\\\")
	ELSEIF(NOT "${_filename}" MATCHES ".rc$")
		SET(tmp_var \\"${_str}\\")
	ELSE()
		SET(tmp_var ${_str})
	ENDIF()

	SET(${_dest} ${tmp_var} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(ADD_STRING_DEFINITION _str _def)
	IF(ARGN)
		FOREACH(arg ${ARGN})
			IF (${arg} MATCHES "^AusweisApp")
				SET(ARG_TYPE TARGET)
			ELSE()
				SET(ARG_TYPE SOURCE)
			ENDIF()

			IF(NOT "${_str}" STREQUAL "")
				GET_QUOTED_STRING(tmp_var ${_str} ${arg})
				SET(tmp_var =\"${tmp_var}\")
			ENDIF()

			SET_PROPERTY(${ARG_TYPE} ${arg} APPEND_STRING PROPERTY COMPILE_FLAGS " -D${_def}${tmp_var}")
		ENDFOREACH()
	ELSE()
		IF(NOT "${_str}" STREQUAL "")
			GET_QUOTED_STRING(tmp_var ${_str} ${arg})
			SET(tmp_var =${tmp_var})
		ENDIF()
		ADD_DEFINITIONS(-D${_def}${tmp_var})
	ENDIF()
ENDFUNCTION()

FUNCTION(ADD_DEFINITION _def)
	ADD_STRING_DEFINITION("" ${_def} ${ARGN})
ENDFUNCTION()


FUNCTION(SUBDIRLIST _result _currentdir)
	FILE(GLOB children RELATIVE ${_currentdir} ${_currentdir}/*)
	SET(dirlist "")
	FOREACH(child ${children})
		IF(IS_DIRECTORY ${_currentdir}/${child})
			LIST(APPEND dirlist ${child})
		ENDIF()
	ENDFOREACH()
	SET(${_result} ${dirlist} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(DIRLIST_OF_FILES _result _files)
	FILE(GLOB_RECURSE children ${_files})
	SET(dirlist "")
	FOREACH(child ${children})
		GET_FILENAME_COMPONENT(dir ${child} DIRECTORY)
		LIST(APPEND dirlist ${dir})
	ENDFOREACH()
	LIST(REMOVE_DUPLICATES dirlist)
	SET(${_result} ${dirlist} PARENT_SCOPE)
ENDFUNCTION()

IF("${CMAKE_SYSTEM_NAME}" MATCHES "BSD")
	SET(BSD true)
ENDIF()

IF(APPLE AND NOT IOS)
	SET(MAC true)
ENDIF()

IF(UNIX AND NOT ANDROID AND NOT APPLE AND NOT CYGWIN AND NOT BSD)
	SET(LINUX true)
ENDIF()

IF(LINUX OR WIN32 OR MAC OR CYGWIN OR BSD)
	SET(DESKTOP true)
ENDIF()


FUNCTION(FILE_SIZE _outSize _file)
	IF(LINUX)
		SET(SIZE_COMMAND stat -c "%s" "${_file}")
	ELSEIF(MAC)
		SET(SIZE_COMMAND stat -f "%z" "${_file}")
	ELSE()
		RETURN()
	ENDIF()

	EXECUTE_PROCESS(COMMAND ${SIZE_COMMAND}
			OUTPUT_VARIABLE SIZE_OUTPUT
			RESULT_VARIABLE SIZE_RESULT
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE)

	IF(${SIZE_RESULT} EQUAL 0)
		SET(${_outSize} ${SIZE_OUTPUT} PARENT_SCOPE)
	ENDIF()
ENDFUNCTION()

IF(NOT COMMAND FIND_HOST_PACKAGE)
	MACRO(FIND_HOST_PACKAGE)
		FIND_PACKAGE(${ARGN})
	ENDMACRO()
ENDIF()

FUNCTION(FETCH_TARGET_LOCATION _destination _target)
	GET_TARGET_PROPERTY(tmp "${_target}" IMPORTED_LOCATION_${CMAKE_BUILD_TYPE})

	IF(NOT tmp)
		IF("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
			GET_TARGET_PROPERTY(tmp "${_target}" IMPORTED_LOCATION_RELEASE)
		ELSEIF("${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
			GET_TARGET_PROPERTY(tmp "${_target}" IMPORTED_LOCATION_DEBUG)
		ENDIF()
	ENDIF()

	IF(NOT tmp)
		GET_TARGET_PROPERTY(tmp "${_target}" IMPORTED_LOCATION)
	ENDIF()

	SET(${_destination} ${tmp} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(map_set_value _map _key _value)
	SET(${_map}_${_key} "${_value}" PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(map_get_value _var _map _key)
	SET(${_var} ${${_map}_${_key}} PARENT_SCOPE)
ENDFUNCTION()


IF(WIN32)
	IF(WIN_SIGN_SUBJECT_NAME OR (WIN_SIGN_KEYSTORE AND WIN_SIGN_KEYSTORE_PSW))
		FIND_PROGRAM(SIGNTOOL_CMD signtool.exe)
		IF(SIGNTOOL_CMD)
			OPTION(WIN_TIMESTAMP "Timestamp the MSI/Files" ON)
			IF(NOT WIN_SIGN_HASHALGO)
				SET(WIN_SIGN_HASHALGO SHA256)
			ENDIF()
			SET(SIGNTOOL_PARAMS sign /v /fd ${WIN_SIGN_HASHALGO} /d AusweisApp2 /du https://www.ausweisapp.bund.de)

			IF(WIN_SIGN_SUBJECT_NAME)
				SET(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /n ${WIN_SIGN_SUBJECT_NAME})
				MESSAGE(STATUS "Files will be signed using: ${WIN_SIGN_SUBJECT_NAME}")
			ELSE()
				SET(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /f ${WIN_SIGN_KEYSTORE} /p ${WIN_SIGN_KEYSTORE_PSW})
				MESSAGE(STATUS "Files will be signed using: ${WIN_SIGN_KEYSTORE}")
			ENDIF()

			IF(WIN_TIMESTAMP)
				IF(NOT WIN_TIMESTAMP_URL)
					SET(WIN_TIMESTAMP_URL http://timestamp.digicert.com)
				ENDIF()
				SET(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /tr ${WIN_TIMESTAMP_URL} /td ${WIN_SIGN_HASHALGO})
			ENDIF()
		ELSE()
			MESSAGE(STATUS "Could NOT find signtool.exe to sign files")
		ENDIF()
	ENDIF()
ENDIF()
