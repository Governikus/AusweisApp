function(GET_CAPITALIZED _out string)
	string(SUBSTRING "${string}" 0 1 _tmp1)
	string(SUBSTRING "${string}" 1 -1 _tmp2)
	string(TOUPPER "${_tmp1}" _tmp1)
	set(${_out} ${_tmp1}${_tmp2} PARENT_SCOPE)
endfunction()


function(GET_SNAKE_TO_CAMEL_CASE _out string)
	string(REPLACE "_" ";" string "${string}")
	foreach(entry ${string})
		GET_CAPITALIZED(entry "${entry}")
		set(_tmp "${_tmp}${entry}")
	endforeach()

	set(${_out} "${_tmp}" PARENT_SCOPE)
endfunction()


function(EXTRACT_MODULES _out_module _out_submodule filename)
	get_filename_component(dir "${filename}" DIRECTORY)

	# make unit test directory to cmake list
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" dir "${dir}")
	string(REPLACE "/" ";" dir "${dir}")

	# extract first subdirectory as module
	list(GET dir 0 MODULE)
	GET_SNAKE_TO_CAMEL_CASE(MODULE "${MODULE}")

	# extract second subdirectory as submodule if it exists
	list(LENGTH dir dir_len)
	if(dir_len GREATER 1)
		list(GET dir 1 SUBMODULE)
		GET_CAPITALIZED(SUBMODULE "${SUBMODULE}")
	endif()

	# return values: module is required, submodule optional
	set(${_out_module} ${MODULE} PARENT_SCOPE)
	set(${_out_submodule} ${SUBMODULE} PARENT_SCOPE)
endfunction()


function(GET_MODULE _out prefix filename)
	EXTRACT_MODULES(MODULE SUBMODULE "${filename}")

	if(TARGET "${prefix}${MODULE}${SUBMODULE}")
		set(${_out} "${prefix}${MODULE}${SUBMODULE}" PARENT_SCOPE)
	elseif(TARGET "${prefix}${MODULE}")
		set(${_out} "${prefix}${MODULE}" PARENT_SCOPE)
	endif()
endfunction()


function(ADD_TEST_EXECUTABLE testname)
	if(ANDROID)
		add_library(${testname} SHARED ${ARGN})
	else()
		add_executable(${testname} ${ARGN})
	endif()

	GET_MODULE(TESTMODULE "AusweisAppTestHelper" "${ARGN}")
	GET_MODULE(MODULE "AusweisApp" "${ARGN}")
	if(NOT MODULE)
		message(FATAL_ERROR "Cannot detect module: ${ARGN}")
	endif()

	target_link_libraries(${testname} ${Qt}::Test AusweisAppTestHelper QRC_FIXTURE_OBJ ${MODULE} ${TESTMODULE})

	if(INCOMPATIBLE_QT_COMPILER_FLAGS)
		set_source_files_properties(${ARGN} PROPERTIES COMPILE_OPTIONS ${INCOMPATIBLE_QT_COMPILER_FLAGS})
	endif()
endfunction()


function(SHOULD_SKIP_TEST _out filename)
	set(${_out} FALSE PARENT_SCOPE)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" test "${sourcefile}")

	if(INTEGRATED_SDK AND (test MATCHES "ui/qml"
		OR test MATCHES "ui/websocket"
		OR test MATCHES "init/"
		OR test MATCHES "remote_device"
		OR test MATCHES "export"))
		set(${_out} TRUE PARENT_SCOPE)
		return()
	endif()

	if(IOS OR ANDROID)
		if(test MATCHES "card/pcsc")
			set(${_out} TRUE PARENT_SCOPE)
			return()
		endif()
	endif()
endfunction()


function(ADD_TEST_EXECUTABLE_SUBDIR)
	file(GLOB_RECURSE TEST_FILES "*.cpp")

	foreach(sourcefile ${TEST_FILES})
		SHOULD_SKIP_TEST(SKIP "${sourcefile}")
		if(SKIP)
			continue()
		endif()

		EXTRACT_TESTNAME(TESTNAME ${sourcefile})
		ADD_TEST_EXECUTABLE(${TESTNAME} ${sourcefile})
		GET_TEST_CMDLINE(TEST_CMDLINE ${TESTNAME})
		add_test(${TESTNAME} ${TESTNAME} ${TEST_CMDLINE})
		set_tests_properties(${TESTNAME} PROPERTIES FAIL_REGULAR_EXPRESSION "nullptr parameter")

		if(TESTNAME MATCHES "UIPlugIn")
			set_tests_properties(${TESTNAME} PROPERTIES LABELS "ausweisapp" TIMEOUT 300)
		else()
			set_tests_properties(${TESTNAME} PROPERTIES LABELS "ausweisapp" TIMEOUT 120)
		endif()
	endforeach()
endfunction()



CONFIGURE_DEFAULT_FILES(${CMAKE_CURRENT_BINARY_DIR})

# Build single test binaries for each cpp file
ADD_TEST_EXECUTABLE_SUBDIR()
