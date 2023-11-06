if(TARGET AusweisAppUiJson)
	add_subdirectory(json)
endif()

if(TARGET AusweisAppUiWebsocket)
	add_subdirectory(websocket)
endif()

if(TARGET AusweisAppUiQml)
	add_subdirectory(qml)
endif()
