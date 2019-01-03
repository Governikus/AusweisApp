#####################################################################
# The module core is responsible to start up the application and main
# event loop. It will initialize any necessary module and control
# any workflow and communication between the modules.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCore)

TARGET_LINK_LIBRARIES(AusweisAppCore Qt5::Network AusweisAppCard AusweisAppGlobal AusweisAppUi AusweisAppActivation AusweisAppSettings AusweisAppNetwork AusweisAppRemoteDevice AusweisAppServices AusweisAppWhitelistClient)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppCore ${WIN_DEFAULT_LIBS})
ENDIF()

IF(DESKTOP)
	TARGET_LINK_LIBRARIES(AusweisAppCore AusweisAppCardDrivers)
ENDIF()
