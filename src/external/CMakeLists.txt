################################## http_parser
#########################################################################
FILE(GLOB_RECURSE EXTERNAL_HTTP_PARSER_FILES http_parser/*.cpp)
ADD_LIBRARY(AusweisAppExternalHttpParser ${EXTERNAL_HTTP_PARSER_FILES})
TARGET_INCLUDE_DIRECTORIES(AusweisAppExternalHttpParser INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")

IF("${CMAKE_CXX_FLAGS}" MATCHES "-Wcovered-switch-default")
	SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS "-Wno-covered-switch-default")
ENDIF()

IF(NOT MSVC)
	SET_PROPERTY(TARGET AusweisAppExternalHttpParser APPEND_STRING PROPERTY COMPILE_FLAGS " -Wno-gnu-statement-expression")
ENDIF()

################################## qhttpserver
#########################################################################
FILE(GLOB_RECURSE EXTERNAL_QHTTPSERVER_FILES qhttpserver/*.cpp)
ADD_LIBRARY(AusweisAppExternalQHttpServer ${EXTERNAL_QHTTPSERVER_FILES})

TARGET_INCLUDE_DIRECTORIES(AusweisAppExternalQHttpServer INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
TARGET_LINK_LIBRARIES(AusweisAppExternalQHttpServer Qt5::Network AusweisAppGlobal AusweisAppExternalHttpParser)


################################## fervor
#########################################################################
IF(DESKTOP)
	FILE(GLOB_RECURSE EXTERNAL_FERVOR_FILES fervor/*.cpp)

	ADD_LIBRARY(AusweisAppExternalFervor ${EXTERNAL_FERVOR_FILES})
	TARGET_INCLUDE_DIRECTORIES(AusweisAppExternalFervor INTERFACE "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>")
	TARGET_LINK_LIBRARIES(AusweisAppExternalFervor Qt5::Network Qt5::Xml Qt5::Widgets AusweisAppNetwork AusweisAppSettings)
ENDIF()
