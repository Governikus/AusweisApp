################################## http_parser
#########################################################################
FUNCTION(PRINT_HTTP_PARSER _include_dir _lib_dir)
	SET(_file "${_include_dir}/http_parser.h")

	FILE(READ "${_file}" _file_content)
	IF(_file_content)
		STRING(REGEX REPLACE ".*#define[\t ]+HTTP_PARSER_VERSION_MAJOR[\t ]+([0-9]+).*" "\\1" _major "${_file_content}")
		STRING(REGEX REPLACE ".*#define[\t ]+HTTP_PARSER_VERSION_MINOR[\t ]+([0-9]+).*" "\\1" _minor "${_file_content}")
		STRING(REGEX REPLACE ".*#define[\t ]+HTTP_PARSER_VERSION_PATCH[\t ]+([0-9]+).*" "\\1" _patch "${_file_content}")
		SET(_version "${_major}.${_minor}.${_patch}")
	ENDIF()

	IF(NOT EXISTS "${_lib_dir}")
		SET(_lib_dir "INTERNAL")
	ENDIF()

	MESSAGE(STATUS "HttpParser: ${_lib_dir} (${_version})")
ENDFUNCTION()

IF(NOT CMAKE_VERSION VERSION_LESS "3.10")
	FIND_PATH(HTTP_PARSER_INCLUDE_DIR NAMES http_parser.h)
	FIND_LIBRARY(HTTP_PARSER_LIBRARY NAMES http_parser libhttp_parser)
	INCLUDE(FindPackageHandleStandardArgs)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(HTTP_PARSER REQUIRED_VARS HTTP_PARSER_INCLUDE_DIR HTTP_PARSER_LIBRARY)
	MARK_AS_ADVANCED(HTTP_PARSER_INCLUDE_DIR HTTP_PARSER_LIBRARY)
ENDIF()

IF(HTTP_PARSER_FOUND)
	ADD_LIBRARY(AusweisAppExternal::HttpParser UNKNOWN IMPORTED GLOBAL)
	SET_TARGET_PROPERTIES(AusweisAppExternal::HttpParser PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${HTTP_PARSER_INCLUDE_DIR}")
	SET_TARGET_PROPERTIES(AusweisAppExternal::HttpParser PROPERTIES IMPORTED_LINK_INTERFACE_LANGUAGES "C" IMPORTED_LOCATION "${HTTP_PARSER_LIBRARY}")

ELSE()
	SET(HTTP_PARSER_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/http_parser")
	FILE(GLOB_RECURSE EXTERNAL_HTTP_PARSER_FILES ${HTTP_PARSER_INCLUDE_DIR}/*.cpp)
	ADD_LIBRARY(AusweisAppExternalHttpParser ${EXTERNAL_HTTP_PARSER_FILES})
	TARGET_INCLUDE_DIRECTORIES(AusweisAppExternalHttpParser INTERFACE "$<BUILD_INTERFACE:${HTTP_PARSER_INCLUDE_DIR}>")

	IF("${CMAKE_CXX_FLAGS}" MATCHES "-Wcovered-switch-default")
		SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-covered-switch-default")
	ENDIF()

	IF("${CMAKE_CXX_FLAGS}" MATCHES "-Wconversion")
		SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-conversion")
	ENDIF()

	IF("${CMAKE_CXX_FLAGS}" MATCHES "-Wold-style-cast")
		SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-old-style-cast")
	ENDIF()

	IF(NOT MSVC)
		ADD_FLAG(-Wno-gnu-statement-expression VAR AusweisAppExternalHttpParser NAME HttpParser)
	ENDIF()

	ADD_LIBRARY(AusweisAppExternal::HttpParser ALIAS AusweisAppExternalHttpParser)
ENDIF()


PRINT_HTTP_PARSER("${HTTP_PARSER_INCLUDE_DIR}" "${HTTP_PARSER_LIBRARY}")
