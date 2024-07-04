ADD_PLATFORM_LIBRARY(AusweisAppWorkflows)

target_link_libraries(AusweisAppWorkflows ${Qt}::Network AusweisAppCard AusweisAppGlobal AusweisAppSettings AusweisAppNetwork)

if(TARGET ${Qt}::StateMachine)
	target_link_libraries(AusweisAppWorkflows ${Qt}::StateMachine)
endif()


if(WIN32)
	target_link_libraries(AusweisAppWorkflows ${WIN_DEFAULT_LIBS})
endif()

if(NOT INTEGRATED_SDK)
	if(DESKTOP)
		target_link_libraries(AusweisAppWorkflows AusweisAppCardDrivers)
	endif()
endif()
