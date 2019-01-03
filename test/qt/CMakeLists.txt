FUNCTION(ADD_TEST_EXECUTABLE testname)
	IF(ANDROID)
		ADD_LIBRARY(${testname} SHARED ${ARGN})
	ELSE()
		ADD_EXECUTABLE(${testname} ${ARGN})
	ENDIF()

	TARGET_LINK_LIBRARIES(${testname} Qt5::Network Qt5::Test OpenSSL::Crypto)
	TARGET_LINK_LIBRARIES(${testname} AusweisAppTestHelper AusweisAppCore AusweisAppCard AusweisAppGlobal AusweisAppCardDrivers AusweisAppServices AusweisAppSettings AusweisAppNetwork)
	TARGET_LINK_LIBRARIES(${testname} AusweisAppActivationInternal AusweisAppUiJsonApi AusweisAppUiAidl AusweisAppUiQml)
	TARGET_LINK_LIBRARIES(${testname} AusweisAppRemoteDevice AusweisAppExport)
	TARGET_LINK_LIBRARIES(${testname} AusweisAppSecureStorage AusweisAppConfiguration AusweisAppFileProvider)
	TARGET_LINK_LIBRARIES(${testname} QRC_FIXTURE_OBJ)

	IF(DESKTOP)
		TARGET_LINK_LIBRARIES(${testname} Qt5::Widgets AusweisAppUiWidget AusweisAppCardPcsc AusweisAppActivationWebservice)
	ENDIF()

	IF(LINUX OR ANDROID OR IOS)
		TARGET_LINK_LIBRARIES(${testname} AusweisAppCardBluetooth)
	ENDIF()

	TARGET_LINK_LIBRARIES(${testname} AusweisAppWhitelistClient)

	IF(WIN32)
		TARGET_LINK_LIBRARIES(${testname} ${WIN_DEFAULT_LIBS})
	ELSEIF(MAC)
		TARGET_LINK_LIBRARIES(${testname} ${OSX_APPKIT})
	ELSEIF(ANDROID)
		TARGET_LINK_LIBRARIES(${testname} Qt5::AndroidExtras)
	ENDIF()
ENDFUNCTION()


FUNCTION(ADD_TEST_EXECUTABLE_SUBDIR)
	SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
	IF(IOS OR ANDROID)
		LIST(REMOVE_ITEM SUBDIRS widget)
		LIST(REMOVE_ITEM SUBDIRS cli)
		LIST(REMOVE_ITEM SUBDIRS pcsc)
		LIST(REMOVE_ITEM SUBDIRS drivers)
		LIST(REMOVE_ITEM SUBDIRS activation_webservice)
	ENDIF()

	FOREACH(SUBDIR ${SUBDIRS})
		FILE(GLOB_RECURSE TEST_SUBFILES "${SUBDIR}/*.cpp")

		FOREACH(sourcefile ${TEST_SUBFILES})
			STRING(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" sourcefile_relative "${sourcefile}")
			IF(NOT (((APPLE OR WIN32 OR BSD) AND "${sourcefile_relative}" MATCHES "card/bluetooth") OR
				((IOS OR ANDROID) AND "${sourcefile_relative}" MATCHES "card/pcsc")))
				EXTRACT_TESTNAME(TESTNAME ${sourcefile})
				ADD_TEST_EXECUTABLE(${TESTNAME} ${sourcefile})
				GET_TEST_CMDLINE(TEST_CMDLINE ${TESTNAME})
				ADD_TEST(${TESTNAME} ${TESTNAME} ${TEST_CMDLINE})
				SET_TESTS_PROPERTIES(${TESTNAME} PROPERTIES LABELS "ausweisapp")
			ENDIF()
		ENDFOREACH()
	ENDFOREACH()
ENDFUNCTION()



CONFIGURE_DEFAULT_FILES(${CMAKE_CURRENT_BINARY_DIR})

# Build single test binaries for each cpp file
ADD_TEST_EXECUTABLE_SUBDIR()
