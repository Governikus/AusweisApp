ADD_PLATFORM_LIBRARY(AusweisAppCore)

TARGET_LINK_LIBRARIES(AusweisAppCore Qt5::Network Qt5::Xml AusweisAppCard AusweisAppCardDrivers AusweisAppGlobal AusweisAppActivation AusweisAppSettings AusweisAppNetwork AusweisAppServices)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppCore ${WIN_DEFAULT_LIBS})
ENDIF()
