function(ADD_INTEGRATION_TEST_EXECUTABLES)
	file(GLOB_RECURSE TEST_FILES "*.cpp")

	foreach(sourcefile ${TEST_FILES})
		EXTRACT_TESTNAME(TESTNAME ${sourcefile})

		if(INCOMPATIBLE_QT_COMPILER_FLAGS)
			set_source_files_properties(${sourcefile} PROPERTIES COMPILE_OPTIONS "${INCOMPATIBLE_QT_COMPILER_FLAGS}")
		endif()

		add_executable(${TESTNAME} ${sourcefile})
		target_link_libraries(${TESTNAME} ${Qt}::Test AusweisAppTestHelper)
		target_compile_definitions(${TESTNAME} PUBLIC AUSWEISAPP_BINARY_DIR="$<TARGET_FILE_DIR:AusweisApp>/")

		GET_TEST_ENV(TESTENV ${TESTNAME})

		GET_TEST_CMDLINE(TEST_CMDLINE ${TESTNAME})
		add_test(${TESTNAME} ${TESTNAME} ${TEST_CMDLINE})
		set_tests_properties(${TESTNAME} PROPERTIES ENVIRONMENT "${TESTENV}")
		set_tests_properties(${TESTNAME} PROPERTIES LABELS "integration" TIMEOUT 300)
	endforeach()
endfunction()

ADD_INTEGRATION_TEST_EXECUTABLES()
