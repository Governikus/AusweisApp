include(CheckCXXCompilerFlag)

# Check if a compiler flag is supported by current compiler.
#
# Options
#   NOQUOTES:            Do not add quotes to the variable (not used if it is a TARGET).
#   USE_SAME_FOR_LINKER: Use flag value for linker, too.
#   USE_LINKER_ONLY:     Use flag for linker only. Only recognized for USE_SAME_FOR_LINKER.
#   LINK_GLOBAL:         Add flags specified by LINK to global linker options.
#
# Parameter
#   NAME:     Add a human readable name. This is for configure output only to
#             group multiple flags or to indicate the usage.
#   LINK:     Use these linker flags to try the compiler flag. The linker
#             flags won't be added unless LINK_GLOBAL is enabled. Otherwise they are for testing only.
#   LINK_VAR: Checked link flags will be added to these variables. (default: CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS)
#   VAR:      Checked compiler flags will be added to these variables. (default: CMAKE_CXX_FLAGS)
#             It is possible to add multiple VAR parameter.
#             If VAR parameter is a cmake TARGET the compiler flag will be added
#             to the COMPILE_FLAGS property of this TARGET only.
function(ADD_FLAG)
	set(options NOQUOTES USE_SAME_FOR_LINKER USE_LINKER_ONLY LINK_GLOBAL)
	set(oneValueArgs NAME)
	set(multiValueArgs LINK LINK_VAR VAR)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT _PARAM_VAR)
		set(_PARAM_VAR CMAKE_CXX_FLAGS)
	endif()

	if(NOT _PARAM_LINK_VAR)
		set(_PARAM_LINK_VAR CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS)
	endif()

	if(_PARAM_LINK)
		set(CMAKE_REQUIRED_LIBRARIES ${_PARAM_LINK})
	endif()

	foreach(flag ${_PARAM_UNPARSED_ARGUMENTS})
		set(flagname ${flag})
		if(_PARAM_NAME)
			set(flagname "${flagname}_${_PARAM_NAME}")
		endif()
		string(REPLACE "-" "_" flagname ${flagname})
		string(REPLACE " " "_" flagname ${flagname})
		string(REPLACE "," "_" flagname ${flagname})
		string(REPLACE ":" "_" flagname ${flagname})

		# GCC will ignore unknown warning options when used in the -Wno- form. It will complain
		# about it though, if something else goes wrong. To check if this is a warning which can be
		# disabled, we remove the negation for the test only:
		string(REPLACE "-Wno-" "-W" flagtest ${flag})

		# This enforces warnings like "-Wunused-command-line-argument" to fail
		# the check.
		set(errorflag "")
		if(NOT MSVC)
			set(errorflag "-Werror")
		endif()

		if(_PARAM_USE_SAME_FOR_LINKER)
			if(CMAKE_VERSION VERSION_LESS "3.14")
				set(CMAKE_REQUIRED_LIBRARIES ${_PARAM_LINK} ${flagtest})
			else()
				set(CMAKE_REQUIRED_LINK_OPTIONS ${_PARAM_LINK} ${flagtest})
			endif()
			if(_PARAM_USE_LINKER_ONLY)
				set(flagtest "")
			endif()
		endif()

		CHECK_CXX_COMPILER_FLAG("${flagtest} ${errorflag}" ${flagname})
		if(${flagname})
			foreach(var ${_PARAM_VAR})
				if (${var} MATCHES "^AusweisApp")
					set_property(TARGET ${var} APPEND_STRING PROPERTY COMPILE_FLAGS " ${flag}")
				elseif(_PARAM_NOQUOTES)
					set(${var} ${${var}} ${flag} PARENT_SCOPE)
				else()
					if (${var})
						set(${var} "${${var}} ${flag}" PARENT_SCOPE)
					else()
						set(${var} "${flag}" PARENT_SCOPE)
					endif()
				endif()
			endforeach()

			if (_PARAM_LINK_GLOBAL)
				foreach(var ${_PARAM_LINK_VAR})
					set(${var} "${${var}} ${_PARAM_LINK}" PARENT_SCOPE)
				endforeach()
			endif()
			return()
		endif()
	endforeach()
endfunction()

function(SET_PUBLIC_HEADER _target)
	foreach(entry ${ARGN})
		list(APPEND header ${CMAKE_CURRENT_SOURCE_DIR}/${entry})
	endforeach()
	set_target_properties(${_target} PROPERTIES PUBLIC_HEADER "${header}")
endfunction()

function(GET_PUBLIC_HEADER _target _out)
	target_get_linked_libraries(${_target} Libraries)

	foreach(libTarget ${Libraries})
		get_target_property(header ${libTarget} PUBLIC_HEADER)
		if(header)
			list(APPEND result ${header})
		endif()
	endforeach()

	set(${_out} "${result}" PARENT_SCOPE)
endfunction()

function(GET_FILE_MATCHER _result_remove _result_keep)
	if(NOT ANDROID)
		list(APPEND matcher_remove "_android")
	else()
		list(APPEND matcher_keep "_android")
	endif()

	if(NOT IOS)
		list(APPEND matcher_remove "_ios")
	else()
		list(APPEND matcher_keep "_ios")
	endif()

	if(NOT MAC)
		list(APPEND matcher_remove "_osx")
	else()
		list(APPEND matcher_keep "_osx")
	endif()

	if(NOT WIN32)
		list(APPEND matcher_remove "_win")
	else()
		list(APPEND matcher_keep "_win")
	endif()

	if(NOT LINUX)
		list(APPEND matcher_remove "_linux")
	else()
		list(APPEND matcher_keep "_linux")
	endif()

	if(NOT BSD)
		list(APPEND matcher_remove "_bsd")
	else()
		list(APPEND matcher_keep "_bsd")
	endif()

	set(${_result_remove} ${matcher_remove} PARENT_SCOPE)
	set(${_result_keep} ${matcher_keep} PARENT_SCOPE)
endfunction()

function(GET_FILE_EXTENSIONS _result)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs PATH)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	set(FILE_EXTENSIONS *.cpp)
	if(APPLE)
		list(APPEND FILE_EXTENSIONS *.m *.mm)
	endif()

	if(_PARAM_PATH)
		foreach(path ${_PARAM_PATH})
			foreach(ext ${FILE_EXTENSIONS})
				list(APPEND tmpExt "${path}/${ext}")
			endforeach()
		endforeach()
		list(APPEND FILE_EXTENSIONS ${tmpExt})
	endif()

	set(${_result} ${FILE_EXTENSIONS} PARENT_SCOPE)
endfunction()

function(ADD_PLATFORM_LIBRARY _name)
	set(options)
	set(oneValueArgs)
	set(multiValueArgs PATH EXCLUDE)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	GET_FILE_EXTENSIONS(FILE_EXTENSIONS PATH ${_PARAM_PATH})
	file(GLOB_RECURSE FILES ${FILE_EXTENSIONS})

	GET_FILE_MATCHER(matcher_remove matcher_keep)

	foreach(file ${FILES})
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" file_stripped "${file}")

		if(_PARAM_EXCLUDE)
			set(skip_file FALSE)
			foreach(exclude ${_PARAM_EXCLUDE})
				if(file_stripped MATCHES "${exclude}")
					set(skip_file TRUE)
					break()
				endif()
			endforeach()

			if(skip_file)
				list(REMOVE_ITEM FILES "${file}")
				continue()
			endif()
		endif()

		set(keep FALSE)
		foreach(match ${matcher_keep})
			if("${file_stripped}" MATCHES ${match})
				set(keep TRUE)
				break()
			endif()
		endforeach()

		if(NOT keep)
			foreach(match ${matcher_remove})
				if("${file_stripped}" MATCHES ${match})
					list(REMOVE_ITEM FILES "${file}")
					break()
				elseif("${file_stripped}" MATCHES "_generic")
					list(REMOVE_ITEM FILES "${file}")
					list(APPEND GENERICS "${file}")
					break()
				endif()
			endforeach()
		endif()
	endforeach()

	foreach(generic ${GENERICS})
		string(FIND "${generic}" "generic" position REVERSE)
		string(SUBSTRING "${generic}" 0 ${position} generic_basename)
		string(REPLACE "${CMAKE_SOURCE_DIR}" "" generic_basename "${generic_basename}")

		set(found FALSE)
		foreach(file ${FILES})
			string(REPLACE "${CMAKE_SOURCE_DIR}" "" file_stripped "${file}")
			if("${file_stripped}" MATCHES "${generic_basename}" AND NOT "${file_stripped}" MATCHES ".h$")
				set(found TRUE)
				break()
			endif()
		endforeach()

		if(NOT found)
			list(APPEND FILES "${generic}")
		endif()
	endforeach()

	add_library(${_name} ${FILES})
	target_include_directories(${_name} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
	if(IOS)
		set_target_properties(${_name} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	if(INCOMPATIBLE_QT_COMPILER_FLAGS)
		set_source_files_properties("${_name}_autogen/mocs_compilation.cpp" PROPERTIES COMPILE_OPTIONS "${INCOMPATIBLE_QT_COMPILER_FLAGS}")
	endif()
endfunction()


function(ADD_OBJECT_LIBRARY TargetName Files)
	set(_name ${TargetName}_ObjectLib)
	add_library(${_name} OBJECT ${Files})

	foreach(MODULE ${ARGN})
		target_include_directories(${_name} SYSTEM PRIVATE $<TARGET_PROPERTY:${MODULE},INTERFACE_INCLUDE_DIRECTORIES>)
		target_compile_definitions(${_name} PRIVATE $<TARGET_PROPERTY:${MODULE},INTERFACE_COMPILE_DEFINITIONS>)
	endforeach()

	add_library(${TargetName} INTERFACE)
	target_sources(${TargetName} INTERFACE $<TARGET_OBJECTS:${_name}>)
	target_include_directories(${TargetName} INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
endfunction()


function(SUBDIRLIST _result _currentdir)
	file(GLOB children RELATIVE ${_currentdir} ${_currentdir}/*)
	set(dirlist "")
	foreach(child ${children})
		if(IS_DIRECTORY ${_currentdir}/${child})
			list(APPEND dirlist ${child})
		endif()
	endforeach()
	set(${_result} ${dirlist} PARENT_SCOPE)
endfunction()


function(DIRLIST_OF_FILES _result _files)
	file(GLOB_RECURSE children ${_files})
	set(dirlist "")
	foreach(child ${children})
		get_filename_component(dir ${child} DIRECTORY)
		list(APPEND dirlist ${dir})
	endforeach()
	list(REMOVE_DUPLICATES dirlist)
	set(${_result} ${dirlist} PARENT_SCOPE)
endfunction()


if("${CMAKE_SYSTEM_NAME}" MATCHES "BSD|DragonFly")
	set(BSD true)
endif()

if(APPLE AND NOT IOS)
	set(MAC true)
endif()

if(UNIX AND NOT ANDROID AND NOT APPLE AND NOT CYGWIN AND NOT BSD)
	set(LINUX true)
endif()

if((WIN32 AND NOT WINDOWS_STORE) OR LINUX OR MAC OR CYGWIN OR BSD)
	set(DESKTOP true)
endif()


if(NOT COMMAND FIND_HOST_PACKAGE)
	macro(FIND_HOST_PACKAGE)
		find_package(${ARGN})
	endmacro()
endif()

function(FETCH_TARGET_LOCATION _destination _target)
	set(options NAME)
	set(oneValueArgs)
	set(multiValueArgs)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
		set(BUILD_TYPE DEBUG)
	else()
		set(BUILD_TYPE RELEASE)
	endif()

	get_target_property(tmp "${_target}" IMPORTED_LOCATION_${BUILD_TYPE})

	if(NOT tmp)
		get_target_property(tmp "${_target}" IMPORTED_LOCATION)
	endif()

	if(NOT tmp AND BUILD_TYPE STREQUAL "DEBUG")
		get_target_property(tmp "${_target}" IMPORTED_LOCATION_RELEASE)
	endif()

	if(_PARAM_NAME)
		get_filename_component(realpath "${tmp}" REALPATH)
		get_filename_component(tmp "${realpath}" NAME)
	endif()

	set(${_destination} ${tmp} PARENT_SCOPE)
endfunction()

function(list_add_if_not_present list elem)
	list(FIND "${list}" "${elem}" exists)
	if(exists EQUAL -1)
		list(APPEND "${list}" "${elem}")
		set("${list}" "${${list}}" PARENT_SCOPE)
	endif()
endfunction()

macro(target_get_linked_libraries_internal _target _outlist)
	list_add_if_not_present("${_outlist}" "${_target}")

	get_target_property(target_type "${_target}" TYPE)
	if(${target_type} STREQUAL "INTERFACE_LIBRARY")
		get_target_property(libs "${_target}" INTERFACE_LINK_LIBRARIES)
	else()
		get_target_property(libs "${_target}" LINK_LIBRARIES)
	endif()

	foreach(lib IN LISTS libs)
		# Required for libs linked as "debug" like "$<$<CONFIG:DEBUG>:$<TARGET_NAME:AusweisAppUiAidl>>"
		string(REGEX REPLACE ".*(AusweisApp[a-zA-Z:]+).*" "\\1" lib "${lib}")
		string(REPLACE "::" "" lib "${lib}")

		if(NOT TARGET "${lib}")
			continue()
		endif()

		if(NOT "${lib}" MATCHES "AusweisApp")
			continue()
		endif()

		get_target_property(lib_type "${lib}" TYPE)
		if(${lib_type} STREQUAL "INTERFACE_LIBRARY" OR ${lib_type} STREQUAL "OBJECT_LIBRARY")
			continue()
		endif()

		list(FIND "${_outlist}" "${lib}" exists)
		if(NOT exists EQUAL -1)
			continue()
		endif()

		target_get_linked_libraries_internal("${lib}" "${_outlist}")
	endforeach()
endmacro()

function(target_get_linked_libraries _target _outlist)
	set(${_outlist} "${_target}")
	target_get_linked_libraries_internal(${_target} ${_outlist})
	set(${_outlist} ${${_outlist}} PARENT_SCOPE)
endfunction()

function(map_set_value _map _key _value)
	set(${_map}_${_key} "${_value}" PARENT_SCOPE)
endfunction()

function(map_get_value _var _map _key)
	set(${_var} ${${_map}_${_key}} PARENT_SCOPE)
endfunction()

function(configure_files _path _pattern _destination)
	set(options FLATTEN)
	set(oneValueArgs)
	set(multiValueArgs)
	cmake_parse_arguments(_PARAM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	foreach(pattern ${_pattern})
		file(GLOB_RECURSE files RELATIVE "${_path}" "${_path}/${pattern}")
		foreach(entry ${files})
			if(_PARAM_FLATTEN)
				get_filename_component(destEntry "${entry}" NAME)
			else()
				set(destEntry ${entry})
			endif()

			configure_file("${_path}/${entry}" "${_destination}/${destEntry}" COPYONLY)
		endforeach()
	endforeach()
endfunction()

if(WIN32)
	if(WIN_SIGN_SUBJECT_NAME OR (WIN_SIGN_KEYSTORE AND WIN_SIGN_KEYSTORE_PSW))
		find_program(SIGNTOOL_CMD signtool.exe)
		if(SIGNTOOL_CMD)
			option(WIN_TIMESTAMP "Timestamp the MSI/Files" ON)
			if(NOT WIN_SIGN_HASHALGO)
				set(WIN_SIGN_HASHALGO SHA256)
			endif()
			set(SIGNTOOL_PARAMS sign /v /fd ${WIN_SIGN_HASHALGO} /d AusweisApp /du https://www.ausweisapp.bund.de)

			if(WIN_SIGN_SUBJECT_NAME)
				set(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /a /n ${WIN_SIGN_SUBJECT_NAME})
				message(STATUS "Files will be signed using: ${WIN_SIGN_SUBJECT_NAME}")
			else()
				string(REPLACE "\\" "/" WIN_SIGN_KEYSTORE "${WIN_SIGN_KEYSTORE}")
				set(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /f ${WIN_SIGN_KEYSTORE} /p ${WIN_SIGN_KEYSTORE_PSW})
				message(STATUS "Files will be signed using: ${WIN_SIGN_KEYSTORE}")
			endif()

			if(WIN_TIMESTAMP)
				if(NOT WIN_TIMESTAMP_URL)
					# http://rfc3161timestamp.globalsign.com/advanced
					set(WIN_TIMESTAMP_URL http://timestamp.digicert.com)
				endif()
				set(SIGNTOOL_PARAMS ${SIGNTOOL_PARAMS} /tr ${WIN_TIMESTAMP_URL} /td ${WIN_SIGN_HASHALGO})
			endif()
		else()
			message(STATUS "Could NOT find signtool.exe to sign files")
		endif()
	endif()

elseif(APPLE)
	find_program(SIGNTOOL_CMD codesign)
endif()
