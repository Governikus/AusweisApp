#####################################################################
# The module core is responsible to start up the application and main
# event loop. It will initialize any necessary module and control
# any workflow and communication between the modules.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCore)

target_link_libraries(AusweisAppCore AusweisAppActivation AusweisAppCard AusweisAppGlobal AusweisAppNetwork AusweisAppServices AusweisAppSettings AusweisAppUi AusweisAppWorkflows)