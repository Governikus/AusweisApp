ADD_PLATFORM_LIBRARY(AusweisAppCardDrivers)

TARGET_LINK_LIBRARIES(AusweisAppCardDrivers Qt5::Core AusweisAppGlobal AusweisAppCard AusweisAppSettings)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppCardDrivers ${WIN_DEFAULT_LIBS})
ENDIF()

IF(MAC)
	TARGET_LINK_LIBRARIES(AusweisAppCardDrivers ${IOKIT})
ENDIF()

IF(LINUX)
	IF(LIBUDEV)
		TARGET_LINK_LIBRARIES(AusweisAppCardDrivers ${LIBUDEV})
		ADD_DEFINITION("HAVE_LIBUDEV" "ReaderDetector_linux.cpp")
	ELSE()
		MESSAGE(STATUS "Hardware detection disabled - Missing libudev")
	ENDIF()
ENDIF()
