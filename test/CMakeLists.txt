function(EXTRACT_TESTNAME result filepath)
	string(REPLACE ".cpp" "" testname ${filepath})
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" testname ${testname})
	string(REPLACE "/" "_" testname ${testname})
	string(REPLACE "+" "" testname ${testname})
	string(REPLACE "_test_" "_" testname ${testname})
	set(${result} "Test_${testname}" PARENT_SCOPE)
endfunction()


function(GET_TEST_CMDLINE cmdline testname)
	cmake_parse_arguments(_PARAM "" "" "SELECTORS" ${ARGN})

	if(NOT WIN32
		AND (
		"${testname}" MATCHES "export"
		OR "${testname}" MATCHES "\\.qml"))

		set(PLATFORM -platform offscreen)
	endif()

	foreach(SELECTOR ${_PARAM_SELECTORS})
		set(testname "${testname}_${SELECTOR}")
	endforeach()

	set(${cmdline} ${PLATFORM} -v2 -o ${CMAKE_CURRENT_BINARY_DIR}/results.${testname}.log.xml,xml -o -,txt PARENT_SCOPE)
endfunction()


function(ADD_QML_TEST _sourcefile)
	cmake_parse_arguments(_PARAM "" "" "SELECTORS" ${ARGN})

	EXTRACT_TESTNAME(TESTNAME ${_sourcefile})
	GET_TEST_CMDLINE(CMD_PARAMS ${TESTNAME} SELECTORS ${_PARAM_SELECTORS})
	set(CMD $<TARGET_FILE:QmlTestRunner> ${CMD_PARAMS} -input ${sourcefile} -import "qrc:///qml/")
	set(ENV_FAIL_ON_WARNING "QT_FATAL_WARNINGS=true")
	# Emasculate ASAN since QtQuick causes problems
	set(ENV_EMASCULATE_ASAN "ASAN_OPTIONS=detect_leaks=0,new_delete_type_mismatch=0")

	if(_PARAM_SELECTORS)
		foreach(_PARAM_SELECTOR_LIST_ENTRY ${_PARAM_SELECTORS})
			set(TESTNAME ${TESTNAME}_${_PARAM_SELECTOR_LIST_ENTRY})
			set(CMD ${CMD} -file-selector ${_PARAM_SELECTOR_LIST_ENTRY})
		endforeach()
	endif()

	add_test(NAME ${TESTNAME} COMMAND ${CMD})
	set_tests_properties(${TESTNAME} PROPERTIES ENVIRONMENT "${ENV_FAIL_ON_WARNING}" ENVIRONMENT "${ENV_EMASCULATE_ASAN}" ENVIRONMENT "QT_QPA_OFFSCREEN_NO_GLX=1")
	set_tests_properties(${TESTNAME} PROPERTIES LABELS "qml" TIMEOUT 60)
endfunction()


function(ADD_QML_TEST_FILES)
	if(BSD)
		message(STATUS "Qml tests on BSD are not supported")
		return()
	endif()

	file(GLOB_RECURSE TEST_SUBFILES "${CMAKE_CURRENT_SOURCE_DIR}/test_*.qml")
	foreach(sourcefile ${TEST_SUBFILES})
		string(REGEX MATCHALL "\\+[a-z]+" PLATFORM_FILTER ${sourcefile})
		if(NOT "+mobile" IN_LIST PLATFORM_FILTER)
			if(NOT "+mac" IN_LIST PLATFORM_FILTER)
				ADD_QML_TEST(${sourcefile} SELECTORS "desktop;win")
			endif()
			if(NOT "+win" IN_LIST PLATFORM_FILTER)
				ADD_QML_TEST(${sourcefile} SELECTORS "desktop;mac")
			endif()
		endif()
		if(NOT "+desktop" IN_LIST PLATFORM_FILTER)
			if(NOT "+tablet" IN_LIST PLATFORM_FILTER)
				if(NOT "+ios" IN_LIST PLATFORM_FILTER)
					ADD_QML_TEST(${sourcefile} SELECTORS "mobile;phone;android")
				endif()
				if(NOT "+android" IN_LIST PLATFORM_FILTER)
					ADD_QML_TEST(${sourcefile} SELECTORS "mobile;phone;ios")
				endif()
			endif()
			if(NOT "+phone" IN_LIST PLATFORM_FILTER)
				if(NOT "+ios" IN_LIST PLATFORM_FILTER)
					ADD_QML_TEST(${sourcefile} SELECTORS "mobile;tablet;android")
				endif()
				if(NOT "+android" IN_LIST PLATFORM_FILTER)
					ADD_QML_TEST(${sourcefile} SELECTORS "mobile;tablet;ios")
				endif()
			endif()
		endif()
	endforeach()
endfunction()

if(TARGET ${Qt}::Test)
	qt_add_resources(QRC_FIXTURE "fixture/fixture.qrc")
	ADD_OBJECT_LIBRARY(QRC_FIXTURE_OBJ ${QRC_FIXTURE} ${Qt}::Core)

	add_subdirectory(helper)
	if(TARGET ${Qt}::Qml AND TARGET ${Qt}::QuickTest)
		add_subdirectory(qml)
	endif()
	add_subdirectory(qt)
	add_subdirectory(json)

	if(INTEGRATED_SDK AND NOT ANDROID)
		add_subdirectory(integrated)
	endif()
endif()
