#####################################################################
# The module card drivers.
#
# This module provides an interface to scan for connected card
# readers and their specific drivers.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardDrivers)

TARGET_LINK_LIBRARIES(AusweisAppCardDrivers Qt5::Core AusweisAppGlobal AusweisAppConfiguration)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppCardDrivers ${WIN_DEFAULT_LIBS})
ENDIF()

IF(MAC)
	TARGET_LINK_LIBRARIES(AusweisAppCardDrivers ${IOKIT})
ENDIF()

IF(LINUX)
	IF(LIBUDEV)
		TARGET_LINK_LIBRARIES(AusweisAppCardDrivers ${LIBUDEV})
		SET_PROPERTY(SOURCE "ReaderDetector_linux.cpp" APPEND_STRING PROPERTY COMPILE_FLAGS " -DHAVE_LIBUDEV")
	ELSE()
		MESSAGE(STATUS "Hardware detection disabled - Missing libudev")
	ENDIF()
ENDIF()
