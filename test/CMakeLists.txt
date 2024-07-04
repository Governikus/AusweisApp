function(EXTRACT_TESTNAME result filepath)
	string(REPLACE ".cpp" "" testname ${filepath})
	string(REPLACE ".mm" "" testname ${testname})
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" testname ${testname})
	string(REPLACE "/" "_" testname ${testname})
	string(REPLACE "+" "" testname ${testname})
	string(REPLACE "test_" "" testname ${testname})
	set(${result} "Test_${testname}" PARENT_SCOPE)
endfunction()


function(GET_TEST_CMDLINE cmdline testname)
	set(${cmdline} ${PLATFORM} -v2 -o ${CMAKE_CURRENT_BINARY_DIR}/results.${testname}.log.xml,xml -o -,txt PARENT_SCOPE)
endfunction()

function(GET_TEST_ENV testenv testname)
	set(IS_GUI_TEST "${testname}" MATCHES "qml")

	if(NOT WIN32 AND IS_GUI_TEST)
		list(APPEND ENV "QT_QPA_PLATFORM=offscreen")
		list(APPEND ENV "QT_QPA_OFFSCREEN_NO_GLX=1")
	endif()

	# Emasculate ASAN since Qt causes problems
	if(IS_GUI_TEST)
		list(APPEND ENV "ASAN_OPTIONS=detect_leaks=0")
	endif()

	if(IS_GUI_TEST)
		list(APPEND ENV "QML_DISABLE_DISK_CACHE=1")
	endif()

	list(APPEND ENV "QT_ENABLE_REGEXP_JIT=0")

	if((LINUX OR BSD) AND "${testname}" MATCHES "card_pcsc_PcscReader")
		list(APPEND ENV "ASAN_OPTIONS=verify_asan_link_order=0")
		list(APPEND ENV "LD_PRELOAD=${PROJECT_BINARY_DIR}/test/helper/pcsc/libAusweisAppTestHelperPcsc.so")
	endif()

	set(${testenv} "${ENV}" PARENT_SCOPE)
endfunction()


function(ADD_QML_TEST _sourcefile _platform)
	EXTRACT_TESTNAME(TESTNAME ${_sourcefile})
	if(NOT _platform STREQUAL "")
		set(TESTNAME ${TESTNAME}_${_platform})
	endif()

	GET_TEST_CMDLINE(CMD_PARAMS ${TESTNAME})
	if(QT_VERSION VERSION_LESS "6.5")
		set(CMD_PARAMS ${CMD_PARAMS} -import "qrc:/qt/qml")
	endif()
	set(CMD_PARAMS ${CMD_PARAMS} -import "qrc:/qml")
	set(CMD $<TARGET_FILE:QmlTestRunner> ${CMD_PARAMS} -input ${sourcefile})

	GET_TEST_ENV(TESTENV ${TESTNAME})
	list(APPEND TESTENV "OVERRIDE_PLATFORM=${_platform}")

	add_test(NAME ${TESTNAME} COMMAND ${CMD})
	set_tests_properties(${TESTNAME} PROPERTIES ENVIRONMENT "${TESTENV}")
	set_tests_properties(${TESTNAME} PROPERTIES LABELS "qml" TIMEOUT 90)
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
			ADD_QML_TEST(${sourcefile} "")
		endif()
		if(NOT "+desktop" IN_LIST PLATFORM_FILTER)
			if(NOT "+ios" IN_LIST PLATFORM_FILTER)
				ADD_QML_TEST(${sourcefile} "android")
			endif()
			if(NOT "+android" IN_LIST PLATFORM_FILTER)
				ADD_QML_TEST(${sourcefile} "ios")
			endif()
		endif()
	endforeach()
endfunction()

add_subdirectory(tools)

if(NOT TARGET ${Qt}::Test
		OR IOS
		OR ANDROID
		OR NOT CMAKE_BUILD_TYPE STREQUAL DEBUG)
	return()
endif()

qt_add_resources(QRC_FIXTURE "fixture/fixture.qrc" OPTIONS -compress-algo zlib)
ADD_OBJECT_LIBRARY(QRC_FIXTURE_OBJ ${QRC_FIXTURE} ${Qt}::Core)

add_subdirectory(helper)
add_subdirectory(qt)

if(TARGET ${Qt}::Qml AND TARGET ${Qt}::QuickTest)
	add_subdirectory(qml)
endif()

if(NOT INTEGRATED_SDK)
	add_subdirectory(integration)
endif()

if(INTEGRATED_SDK AND NOT CONTAINER_SDK)
	add_subdirectory(integrated)
endif()
