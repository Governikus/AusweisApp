#####################################################################
# The module core is responsible to start up the application and main
# event loop. It will initialize any necessary module and control
# any workflow and communication between the modules.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCore)

target_link_libraries(AusweisAppCore ${Qt}::Network AusweisAppCard AusweisAppGlobal AusweisAppUi AusweisAppActivation AusweisAppSettings AusweisAppNetwork AusweisAppWorkflows AusweisAppServices)

if(TARGET ${Qt}::WebSockets)
	target_link_libraries(AusweisAppCore AusweisAppWorkflowsRemoteDevice AusweisAppRemoteDevice)
endif()

if(NOT INTEGRATED_SDK)
	if(DESKTOP)
		target_link_libraries(AusweisAppCore AusweisAppCardDrivers)
	endif()
endif()
