set(TESTNAME test_sdk_Integrated)
add_executable(${TESTNAME} test_Integrated.cpp)
target_link_libraries(${TESTNAME} PRIVATE Threads::Threads)
target_link_libraries(${TESTNAME} PRIVATE AusweisAppBinary AusweisAppUiFunctional AusweisAppTestHelper)

add_test(NAME ${TESTNAME} COMMAND ${TESTNAME})
set_tests_properties(${TESTNAME} PROPERTIES LABELS "integrated" TIMEOUT 60)
set_tests_properties(${TESTNAME} PROPERTIES FAIL_REGULAR_EXPRESSION "WARNING: QApplication was not created in the main")

if(NOT GOVERNIKUS_QT)
	list(APPEND ENV "ASAN_OPTIONS=detect_leaks=0")
	set_tests_properties(${TESTNAME} PROPERTIES ENVIRONMENT "${ENV}")
endif()
