FUNCTION(GET_CAPITALIZED _out string)
	STRING(SUBSTRING "${string}" 0 1 _tmp1)
	STRING(SUBSTRING "${string}" 1 -1 _tmp2)
	STRING(TOUPPER "${_tmp1}" _tmp1)
	SET(${_out} ${_tmp1}${_tmp2} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(GET_SNAKE_TO_CAMEL_CASE _out string)
	STRING(REPLACE "_" ";" string "${string}")
	FOREACH(entry ${string})
		GET_CAPITALIZED(entry "${entry}")
		SET(_tmp "${_tmp}${entry}")
	ENDFOREACH()

	SET(${_out} "${_tmp}" PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(EXTRACT_MODULES _out_module _out_submodule filename)
	GET_FILENAME_COMPONENT(dir "${filename}" DIRECTORY)

	# make unit test directory to cmake list
	STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" dir "${dir}")
	STRING(REPLACE "/" ";" dir "${dir}")

	# extract first subdirectory as module
	LIST(GET dir 0 MODULE)
	GET_SNAKE_TO_CAMEL_CASE(MODULE "${MODULE}")

	# extract second subdirectory as submodule if it exists
	LIST(LENGTH dir dir_len)
	IF(dir_len GREATER 1)
		LIST(GET dir 1 SUBMODULE)
		GET_CAPITALIZED(SUBMODULE "${SUBMODULE}")
	ENDIF()

	# return values: module is required, submodule optional
	SET(${_out_module} ${MODULE} PARENT_SCOPE)
	SET(${_out_submodule} ${SUBMODULE} PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(GET_MODULE _out filename)
	EXTRACT_MODULES(MODULE SUBMODULE "${filename}")

	IF(TARGET "AusweisApp${MODULE}${SUBMODULE}")
		SET(${_out} "AusweisApp${MODULE}${SUBMODULE}" PARENT_SCOPE)
	ELSEIF(TARGET "AusweisApp${MODULE}")
		SET(${_out} "AusweisApp${MODULE}" PARENT_SCOPE)
	ELSE()
		MESSAGE(FATAL_ERROR "Cannot detect module: ${filename}")
	ENDIF()
ENDFUNCTION()


FUNCTION(ADD_TEST_EXECUTABLE testname)
	IF(ANDROID)
		ADD_LIBRARY(${testname} SHARED ${ARGN})
	ELSE()
		ADD_EXECUTABLE(${testname} ${ARGN})
	ENDIF()

	GET_MODULE(MODULE "${ARGN}")
	TARGET_LINK_LIBRARIES(${testname} Qt5::Test AusweisAppTestHelper QRC_FIXTURE_OBJ ${MODULE})
ENDFUNCTION()


FUNCTION(SHOULD_SKIP_TEST _out filename)
	SET(${_out} FALSE PARENT_SCOPE)
	STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" test "${sourcefile}")

	IF(INTEGRATED_SDK AND (test MATCHES "ui/qml"
		OR test MATCHES "ui/widget"))
		SET(${_out} TRUE PARENT_SCOPE)
		RETURN()
	ENDIF()

	IF(IOS OR ANDROID)
		IF(test MATCHES "ui/widget"
			OR test MATCHES "card/pcsc")
			SET(${_out} TRUE PARENT_SCOPE)
			RETURN()
		ENDIF()
	ENDIF()
ENDFUNCTION()


FUNCTION(ADD_TEST_EXECUTABLE_SUBDIR)
	FILE(GLOB_RECURSE TEST_FILES "*.cpp")

	FOREACH(sourcefile ${TEST_FILES})
		SHOULD_SKIP_TEST(SKIP "${sourcefile}")
		IF(SKIP)
			CONTINUE()
		ENDIF()

		EXTRACT_TESTNAME(TESTNAME ${sourcefile})
		ADD_TEST_EXECUTABLE(${TESTNAME} ${sourcefile})
		GET_TEST_CMDLINE(TEST_CMDLINE ${TESTNAME})
		ADD_TEST(${TESTNAME} ${TESTNAME} ${TEST_CMDLINE})
		SET_TESTS_PROPERTIES(${TESTNAME} PROPERTIES LABELS "ausweisapp")
	ENDFOREACH()
ENDFUNCTION()



CONFIGURE_DEFAULT_FILES(${CMAKE_CURRENT_BINARY_DIR})

# Build single test binaries for each cpp file
ADD_TEST_EXECUTABLE_SUBDIR()
