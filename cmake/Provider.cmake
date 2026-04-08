cmake_minimum_required(VERSION 3.25)

macro(provide_dependency method name)
	set(ARGS ${ARGN})

	if("${name}" MATCHES "Qt" OR "${name}" MATCHES "OpenSSL" OR "${name}" MATCHES "Governikus" OR "${name}" MATCHES "llhttp")
		set(PREFIX_PATH "${CMAKE_BINARY_DIR}/provider")
		if(NOT EXISTS "${PREFIX_PATH}")
			execute_process(COMMAND ${CMAKE_COMMAND} -DNAME=Provider -DPROVIDER=${PROVIDER} -P ${CMAKE_SOURCE_DIR}/ci.cmake
							WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
		endif()
		list(APPEND ARGS NO_DEFAULT_PATH PATHS ${PREFIX_PATH})
		set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${PREFIX_PATH}")
	endif()

	find_package(${name} ${ARGS} BYPASS_PROVIDER)
endmacro()

cmake_language(
	SET_DEPENDENCY_PROVIDER provide_dependency
	SUPPORTED_METHODS FIND_PACKAGE
)

message(STATUS "Governikus dependency provider enabled!")
