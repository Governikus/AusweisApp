INCLUDE(CheckCXXCompilerFlag)

# Check if a compiler flag is supported by current compiler.
#
# Options
#   NOQUOTES:            Do not add quotes to the variable (not used if it is a TARGET).
#   USE_SAME_FOR_LINKER: Use flag value for linker, too.
#   USE_LINKER_ONLY:     Use flag for linker only. Only recognized for USE_SAME_FOR_LINKER.
#
# Parameter
#   NAME: Add a human readable name. This is for configure output only to
#         group multiple flags or to indicate the usage.
#   LINK: Use these linker flags to try the compiler flag. The linker
#         flags won't be added. They are for testing only.
#   VAR:  Checked compiler flags will be added to these variables. (default: CMAKE_CXX_FLAGS)
#         It is possible to add multiple VAR parameter.
#         If VAR parameter is a cmake TARGET the compiler flag will be added
#         to the COMPILE_FLAGS property of this TARGET only.
FUNCTION(ADD_FLAG)
	SET(options NOQUOTES USE_SAME_FOR_LINKER USE_LINKER_ONLY)
	SET(oneValueArgs NAME)
	SET(multiValueArgs LINK VAR)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	IF(NOT _PARAM_VAR)
		SET(_PARAM_VAR CMAKE_CXX_FLAGS)
	ENDIF()

	IF(_PARAM_LINK)
		SET(CMAKE_REQUIRED_LIBRARIES ${_PARAM_LINK})
	ENDIF()

	FOREACH(flag ${_PARAM_UNPARSED_ARGUMENTS})
		SET(flagname ${flag})
		IF(_PARAM_NAME)
			SET(flagname "${flagname}_${_PARAM_NAME}")
		ENDIF()
		STRING(REPLACE "-" "_" flagname ${flagname})
		STRING(REPLACE " " "_" flagname ${flagname})

		# GCC will ignore unknown warning options when used in the -Wno- form. It will complain
		# about it though, if something else goes wrong. To check if this is a warning which can be
		# disabled, we remove the negation for the test only:
		STRING(REPLACE "-Wno-" "-W" flagtest ${flag})

		# This enforces warnings like "-Wunused-command-line-argument" to fail
		# the check.
		SET(errorflag "")
		IF(NOT MSVC)
			SET(errorflag "-Werror")
		ENDIF()

		IF(_PARAM_USE_SAME_FOR_LINKER)
			IF(CMAKE_VERSION VERSION_LESS "3.14")
				SET(CMAKE_REQUIRED_LIBRARIES ${_PARAM_LINK} ${flagtest})
			ELSE()
				SET(CMAKE_REQUIRED_LINK_OPTIONS ${_PARAM_LINK} ${flagtest})
			ENDIF()
			IF(_PARAM_USE_LINKER_ONLY)
				SET(flagtest "")
			ENDIF()
		ENDIF()

		CHECK_CXX_COMPILER_FLAG("${flagtest} ${errorflag}" ${flagname})
		IF(${flagname})
			FOREACH(var ${_PARAM_VAR})
				IF (${var} MATCHES "^AusweisApp")
					SET_PROPERTY(TARGET ${var} APPEND_STRING PROPERTY COMPILE_FLAGS " ${flag}")
				ELSEIF(_PARAM_NOQUOTES)
					SET(${var} ${${var}} ${flag} PARENT_SCOPE)
				ELSE()
					SET(${var} "${${var}} ${flag}" PARENT_SCOPE)
				ENDIF()
			ENDFOREACH()

			RETURN()
		ENDIF()
	ENDFOREACH()
ENDFUNCTION()


FUNCTION(QUERY_QMAKE _out _var)
	IF(NOT TARGET Qt5::qmake)
		MESSAGE(WARNING "qmake not found")
		RETURN()
	ENDIF()

	GET_TARGET_PROPERTY(qmake_bin Qt5::qmake LOCATION)
	EXECUTE_PROCESS(COMMAND "${qmake_bin}" -query ${_var}
		RESULT_VARIABLE _result
		OUTPUT_VARIABLE _output
		ERROR_QUIET
		OUTPUT_STRIP_TRAILING_WHITESPACE)

	IF(_result EQUAL 0)
		SET(${_out} "${_output}" PARENT_SCOPE)
	ENDIF()
ENDFUNCTION()


FUNCTION(GET_FILE_MATCHER _result_remove _result_keep)
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
	IF(APPLE)
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
		STRING(REPLACE "${CMAKE_SOURCE_DIR}" "" file_stripped "${file}")

		SET(keep FALSE)
		FOREACH(match ${matcher_keep})
			IF("${file_stripped}" MATCHES ${match})
				SET(keep TRUE)
				BREAK()
			ENDIF()
		ENDFOREACH()

		IF(NOT keep)
			FOREACH(match ${matcher_remove})
				IF("${file_stripped}" MATCHES ${match})
					LIST(REMOVE_ITEM FILES "${file}")
					BREAK()
				ELSEIF("${file_stripped}" MATCHES "_generic")
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
			STRING(REPLACE "${CMAKE_SOURCE_DIR}" "" file_stripped "${file}")
			IF("${file_stripped}" MATCHES "${generic_basename}" AND NOT "${file_stripped}" MATCHES ".h$")
				SET(found TRUE)
				BREAK()
			ENDIF()
		ENDFOREACH()

		IF(NOT found)
			LIST(APPEND FILES "${generic}")
		ENDIF()
	ENDFOREACH()

	ADD_LIBRARY(${_name} STATIC ${FILES})
	TARGET_INCLUDE_DIRECTORIES(${_name} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
	IF(IOS)
		SET_TARGET_PROPERTIES(${_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	ENDIF()
ENDFUNCTION()


FUNCTION(ADD_OBJECT_LIBRARY TargetName Files)
	SET(_name ${TargetName}_ObjectLib)
	ADD_LIBRARY(${_name} OBJECT ${Files})

	FOREACH(MODULE ${ARGN})
		TARGET_INCLUDE_DIRECTORIES(${_name} SYSTEM PRIVATE $<TARGET_PROPERTY:${MODULE},INTERFACE_INCLUDE_DIRECTORIES>)
		TARGET_COMPILE_DEFINITIONS(${_name} PRIVATE $<TARGET_PROPERTY:${MODULE},INTERFACE_COMPILE_DEFINITIONS>)
	ENDFOREACH()

	IF(Qt5_POSITION_INDEPENDENT_CODE)
		SET_TARGET_PROPERTIES(${_name} PROPERTIES POSITION_INDEPENDENT_CODE ON)
	ENDIF()

	ADD_LIBRARY(${TargetName} INTERFACE)
	TARGET_SOURCES(${TargetName} INTERFACE $<TARGET_OBJECTS:${_name}>)
	TARGET_INCLUDE_DIRECTORIES(${TargetName} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
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


FUNCTION(GET_ANDROID_TOOLCHAIN_VARS _prefix _machine)
	GET_FILENAME_COMPONENT(ANDROID_TOOLCHAIN_MACHINE_NAME "${CMAKE_CXX_ANDROID_TOOLCHAIN_PREFIX}" NAME)
	STRING(REGEX REPLACE "-$" "" ANDROID_TOOLCHAIN_MACHINE_NAME "${ANDROID_TOOLCHAIN_MACHINE_NAME}")
	STRING(REGEX MATCH "/toolchains/(.*)/prebuilt/" _unused "${CMAKE_CXX_ANDROID_TOOLCHAIN_PREFIX}")
	STRING(REGEX REPLACE "-${ANDROID_NDK_TOOLCHAIN_VERSION}$" "" ANDROID_TOOLCHAIN_PREFIX "${CMAKE_MATCH_1}")

	SET(${_prefix} ${ANDROID_TOOLCHAIN_PREFIX} PARENT_SCOPE)

	IF(ANDROID_USE_LLVM)
		SET(${_machine} llvm PARENT_SCOPE)
	ELSE()
		SET(${_machine} ${ANDROID_TOOLCHAIN_MACHINE_NAME} PARENT_SCOPE)
	ENDIF()
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

IF((WIN32 AND NOT WINDOWS_STORE) OR LINUX OR MAC OR CYGWIN OR BSD)
	SET(DESKTOP true)
ENDIF()


IF(CMAKE_VERSION VERSION_LESS "3.14") # Use FILE(SIZE)
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
ENDIF()

IF(NOT COMMAND FIND_HOST_PACKAGE)
	MACRO(FIND_HOST_PACKAGE)
		FIND_PACKAGE(${ARGN})
	ENDMACRO()
ENDIF()

FUNCTION(FETCH_TARGET_LOCATION _destination _target)
	SET(options NAME)
	SET(oneValueArgs)
	SET(multiValueArgs)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	IF(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
		SET(BUILD_TYPE DEBUG)
	ELSE()
		SET(BUILD_TYPE RELEASE)
	ENDIF()

	GET_TARGET_PROPERTY(tmp "${_target}" IMPORTED_LOCATION_${BUILD_TYPE})

	IF(NOT tmp)
		GET_TARGET_PROPERTY(tmp "${_target}" IMPORTED_LOCATION)
	ENDIF()

	IF(_PARAM_NAME)
		GET_FILENAME_COMPONENT(realpath "${tmp}" REALPATH)
		GET_FILENAME_COMPONENT(tmp "${realpath}" NAME)
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
				STRING(REPLACE "\\" "/" WIN_SIGN_KEYSTORE "${WIN_SIGN_KEYSTORE}")
				SET(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /f ${WIN_SIGN_KEYSTORE} /p ${WIN_SIGN_KEYSTORE_PSW})
				MESSAGE(STATUS "Files will be signed using: ${WIN_SIGN_KEYSTORE}")
			ENDIF()

			IF(WIN_TIMESTAMP)
				IF(NOT WIN_TIMESTAMP_URL)
					# http://rfc3161timestamp.globalsign.com/advanced
					SET(WIN_TIMESTAMP_URL http://timestamp.digicert.com)
				ENDIF()
				SET(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /tr ${WIN_TIMESTAMP_URL} /td ${WIN_SIGN_HASHALGO})
			ENDIF()
		ELSE()
			MESSAGE(STATUS "Could NOT find signtool.exe to sign files")
		ENDIF()
	ENDIF()
ENDIF()
