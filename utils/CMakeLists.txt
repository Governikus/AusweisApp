# Do not compile for mobile: QProcess is not supported under iOS.
IF(DESKTOP AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/testbedtool")
	ADD_SUBDIRECTORY(testbedtool)
ENDIF()

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tlscheck")
	ADD_SUBDIRECTORY(tlscheck)
ENDIF()

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/fuzzing")
	ADD_SUBDIRECTORY(fuzzing)
ENDIF()
