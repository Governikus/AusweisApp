#####################################################################
# The websockets plugin implements the ui interface for a websocket.
#
# This websocket can be used as a SDK for desktop systems as it
# uses the json api.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiWebsocket)

target_link_libraries(AusweisAppUiWebsocket ${Qt}::Core ${Qt}::WebSockets AusweisAppUi AusweisAppUiJson AusweisAppNetwork AusweisAppGlobal)
target_link_libraries(AusweisAppUiWebsocket AusweisAppWorkflowsIfd AusweisAppIfdRemote)
target_compile_definitions(AusweisAppUiWebsocket PRIVATE QT_STATICPLUGIN)
