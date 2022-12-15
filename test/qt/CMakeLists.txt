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

	GET_SNAKE_TO_CAMEL_CASE(SUBMODULE "${SUBMODULE}")

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

	if(TARGET AusweisAppConfig)
		target_link_libraries(${testname} AusweisAppConfig)
	endif()

	GET_MODULE(TESTMODULE "AusweisAppTestHelper" "${ARGN}")
	GET_MODULE(MODULE "AusweisApp" "${ARGN}")
	if(NOT MODULE)
		message(FATAL_ERROR "Cannot detect module: ${ARGN}")
	endif()

	target_link_libraries(${testname} ${Qt}::Test AusweisAppTestHelper QRC_FIXTURE_OBJ ${MODULE} ${TESTMODULE})

	if(INCOMPATIBLE_QT_COMPILER_FLAGS)
		set_source_files_properties(${ARGN} PROPERTIES COMPILE_OPTIONS "${INCOMPATIBLE_QT_COMPILER_FLAGS}")
	endif()

	if(CMAKE_GENERATOR MATCHES "Xcode")
		set_target_properties(${testname} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${CMAKE_BUILD_TYPE} "${CMAKE_CURRENT_BINARY_DIR}")
	endif()
endfunction()


function(SHOULD_SKIP_TEST _out filename)
	set(${_out} FALSE PARENT_SCOPE)
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" test "${sourcefile}")

	if(NOT USE_SMARTEID AND (test MATCHES "card/smart"
			OR test MATCHES "ui/qml/smart"
			OR test MATCHES "workflows/personalization"
			OR test MATCHES "ui/json/test_MsgHandlerPersonalization"))
		set(${_out} TRUE PARENT_SCOPE)
		return()
	endif()

	if(INTEGRATED_SDK AND (test MATCHES "ui/qml"
			OR test MATCHES "ui/scheme"
			OR test MATCHES "ifd"
			OR test MATCHES "export"
			OR test MATCHES "diagnosis"))
		set(${_out} TRUE PARENT_SCOPE)
		return()
	endif()

	if(IOS OR ANDROID)
		if(test MATCHES "card/pcsc"
				OR test MATCHES "card/drivers"
				OR test MATCHES "diagnosis")
			set(${_out} TRUE PARENT_SCOPE)
			return()
		endif()
	endif()
endfunction()


function(ADD_TEST_EXECUTABLE_SUBDIR)
	if(TARGET AusweisAppRcc)
		add_custom_target(AusweisAppTestRcc DEPENDS AusweisAppRcc)
		add_custom_command(TARGET AusweisAppTestRcc POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RCC}" "${CMAKE_CURRENT_BINARY_DIR}")
	endif()

	if(APPLE)
		file(GLOB_RECURSE TEST_FILES "*.cpp" "*.mm")
	else()
		file(GLOB_RECURSE TEST_FILES "*.cpp")
	endif()

	foreach(sourcefile ${TEST_FILES})
		SHOULD_SKIP_TEST(SKIP "${sourcefile}")
		if(SKIP)
			continue()
		endif()

		EXTRACT_TESTNAME(TESTNAME ${sourcefile})
		ADD_TEST_EXECUTABLE(${TESTNAME} ${sourcefile})
		GET_TEST_CMDLINE(TEST_CMDLINE ${TESTNAME})
		add_test(${TESTNAME} ${TESTNAME} ${TEST_CMDLINE})
		if(TARGET AusweisAppTestRcc)
			add_dependencies(${TESTNAME} AusweisAppTestRcc)
		endif()

		GET_TEST_ENV(TESTENV ${TESTNAME})

		set_tests_properties(${TESTNAME} PROPERTIES FAIL_REGULAR_EXPRESSION "nullptr parameter")
		set_tests_properties(${TESTNAME} PROPERTIES ENVIRONMENT "${TESTENV}")
		set_tests_properties(${TESTNAME} PROPERTIES LABELS "qt" TIMEOUT 120)

		# Create a target for each folder and add each test as a dependency
		get_filename_component(TEST_FOLDER_TARGET ${sourcefile} DIRECTORY)
		EXTRACT_TESTNAME(TEST_FOLDER_TARGET ${TEST_FOLDER_TARGET})
		set(TEST_FOLDER_TARGET "ALL_${TEST_FOLDER_TARGET}")
		if(NOT TARGET ${TEST_FOLDER_TARGET})
			add_custom_target(${TEST_FOLDER_TARGET})
		endif()
		add_dependencies(${TEST_FOLDER_TARGET} ${TESTNAME})
	endforeach()
endfunction()



# Build single test binaries for each cpp file
ADD_TEST_EXECUTABLE_SUBDIR()
