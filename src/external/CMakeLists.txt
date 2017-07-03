################################## http_parser
#########################################################################
FILE(GLOB_RECURSE EXTERNAL_HTTP_PARSER_FILES http_parser/*.cpp)
ADD_LIBRARY(AusweisAppExternalHttpParser ${EXTERNAL_HTTP_PARSER_FILES})
TARGET_INCLUDE_DIRECTORIES(AusweisAppExternalHttpParser INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

IF("${CMAKE_CXX_FLAGS}" MATCHES "-Wcovered-switch-default")
	SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-covered-switch-default")
ENDIF()

IF("${CMAKE_CXX_FLAGS}" MATCHES "-Wconversion")
	SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-conversion")
ENDIF()

IF(NOT MSVC)
	SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-gnu-statement-expression")
ENDIF()

