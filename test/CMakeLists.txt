qt5_add_resources(QRC_FIXTURE "fixture/fixture.qrc")
ADD_OBJECT_LIBRARY(QRC_FIXTURE_OBJ ${QRC_FIXTURE} Qt5::Core)


FUNCTION(EXTRACT_TESTNAME result filepath)
	STRING(REPLACE ".cpp" "" testname ${filepath})
	STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" testname ${testname})
	STRING(REPLACE "/" "_" testname ${testname})
	STRING(REPLACE "_test_" "_" testname ${testname})
	SET(${result} "Test_${testname}" PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(GET_TEST_CMDLINE cmdline testname)
	IF(NOT WIN32  AND "${testname}" MATCHES "gui" OR "${testname}" MATCHES "\\.qml")
		SET(PLATFORM -platform offscreen)
	ENDIF()

	SET(${cmdline} ${PLATFORM} -v2 -o ${CMAKE_CURRENT_BINARY_DIR}/results.${testname}.log.xml,xml PARENT_SCOPE)
ENDFUNCTION()


FUNCTION(ADD_QML_TEST_FILES _import_path)
	FILE(GLOB_RECURSE TEST_SUBFILES "${CMAKE_CURRENT_SOURCE_DIR}/test_*.qml")
	FOREACH(sourcefile ${TEST_SUBFILES})
		EXTRACT_TESTNAME(TESTNAME ${sourcefile})
		GET_TEST_CMDLINE(CMD_PARAMS ${TESTNAME})
		ADD_TEST(NAME ${TESTNAME} COMMAND $<TARGET_FILE:QmlTestRunner> ${CMD_PARAMS} -input ${sourcefile} -import "${_import_path}")
		SET_TESTS_PROPERTIES(${TESTNAME} PROPERTIES LABELS "qml")
	ENDFOREACH()
ENDFUNCTION()


ADD_SUBDIRECTORY(helper)
ADD_SUBDIRECTORY(qml)
ADD_SUBDIRECTORY(qml_stationary)
ADD_SUBDIRECTORY(qt)
