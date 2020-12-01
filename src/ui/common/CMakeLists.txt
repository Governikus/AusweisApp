#####################################################################
# Common UI functions used by multiple UI implementations
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiCommon)

target_link_libraries(AusweisAppUiCommon ${Qt}::Core ${Qt}::Gui AusweisAppGlobal AusweisAppCard AusweisAppRemoteDevice)

if(DESKTOP)
	target_link_libraries(AusweisAppUiCommon ${Qt}::Widgets)
endif()

if(MAC)
	target_link_libraries(AusweisAppUiCommon ${OSX_APPKIT})
endif()
