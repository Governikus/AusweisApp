################################## http_parser
#########################################################################
function(PARSE_HTTP_PARSER_VERSION _include_dir _version)
	set(_file "${_include_dir}/http_parser.h")

	file(READ "${_file}" _file_content)
	if(_file_content)
		string(REGEX REPLACE ".*#define[\t ]+HTTP_PARSER_VERSION_MAJOR[\t ]+([0-9]+).*" "\\1" _major "${_file_content}")
		string(REGEX REPLACE ".*#define[\t ]+HTTP_PARSER_VERSION_MINOR[\t ]+([0-9]+).*" "\\1" _minor "${_file_content}")
		string(REGEX REPLACE ".*#define[\t ]+HTTP_PARSER_VERSION_PATCH[\t ]+([0-9]+).*" "\\1" _patch "${_file_content}")
		set(${_version} "${_major}.${_minor}.${_patch}" PARENT_SCOPE)
	endif()
endfunction()

function(PRINT_HTTP_PARSER _include_dir _lib_dir)
	PARSE_HTTP_PARSER_VERSION("${_include_dir}" _version)

	if(NOT EXISTS "${_lib_dir}")
		set(_lib_dir "INTERNAL")
	endif()

	message(STATUS "HttpParser: ${_lib_dir} (${_version})")
endfunction()

if(NOT CMAKE_VERSION VERSION_LESS "3.10")
	find_path(HTTP_PARSER_INCLUDE_DIR NAMES http_parser.h)
	find_library(HTTP_PARSER_LIBRARY NAMES http_parser libhttp_parser)

	if(HTTP_PARSER_INCLUDE_DIR)
		PARSE_HTTP_PARSER_VERSION("${HTTP_PARSER_INCLUDE_DIR}" _version)
		if(_version VERSION_LESS "2.8.0")
			message(STATUS "HttpParser (system) too old: ${_version}")
			unset(HTTP_PARSER_INCLUDE_DIR CACHE)
			unset(HTTP_PARSER_LIBRARY CACHE)
		else()
			include(FindPackageHandleStandardArgs)
			FIND_PACKAGE_HANDLE_STANDARD_ARGS(HTTP_PARSER REQUIRED_VARS HTTP_PARSER_INCLUDE_DIR HTTP_PARSER_LIBRARY)
			mark_as_advanced(HTTP_PARSER_INCLUDE_DIR HTTP_PARSER_LIBRARY)
		endif()
	endif()
endif()

if(HTTP_PARSER_FOUND)
	add_library(AusweisAppExternal::HttpParser UNKNOWN IMPORTED GLOBAL)
	set_target_properties(AusweisAppExternal::HttpParser PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${HTTP_PARSER_INCLUDE_DIR}")
	set_target_properties(AusweisAppExternal::HttpParser PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "C" IMPORTED_LOCATION "${HTTP_PARSER_LIBRARY}")

else()
	set(HTTP_PARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
	file(GLOB_RECURSE EXTERNAL_HTTP_PARSER_FILES ${HTTP_PARSER_INCLUDE_DIR}/*.cpp)
	add_library(AusweisAppExternalHttpParser ${EXTERNAL_HTTP_PARSER_FILES})
	target_include_directories(AusweisAppExternalHttpParser INTERFACE "$<BUILD_INTERFACE:${HTTP_PARSER_INCLUDE_DIR}>")

	if(IOS)
		set_target_properties(AusweisAppExternalHttpParser PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	if(MSVC)
		set_property(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " /w")
	else()
		set_property(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
	endif()

	add_library(AusweisAppExternal::HttpParser ALIAS AusweisAppExternalHttpParser)
endif()


PRINT_HTTP_PARSER("${HTTP_PARSER_INCLUDE_DIR}" "${HTTP_PARSER_LIBRARY}")
