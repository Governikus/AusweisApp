#####################################################################
# The websockets plugin implements the ui interface for a websocket.
#
# This websocket can be used as a SDK for desktop systems as it
# uses the json api.
#####################################################################

IF(TARGET Qt5::WebSockets)
	ADD_PLATFORM_LIBRARY(AusweisAppUiWebsocket)

	TARGET_LINK_LIBRARIES(AusweisAppUiWebsocket Qt5::Core Qt5::WebSockets AusweisAppUi AusweisAppUiJsonApi AusweisAppNetwork AusweisAppGlobal)
	TARGET_COMPILE_DEFINITIONS(AusweisAppUiWebsocket PRIVATE QT_STATICPLUGIN)
ENDIF()
