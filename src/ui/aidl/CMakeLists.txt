#####################################################################
# The aidl plugin implements the ui interface for Android's AIDL.
#
# It provides a background service for the SDK.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiAidl)

target_link_libraries(AusweisAppUiAidl ${Qt}::Core ${Qt}::Gui AusweisAppUiJson AusweisAppGlobal)
if(TARGET ${Qt}::WebSockets)
	if(INTEGRATED_SDK)
		target_link_libraries(AusweisAppUiAidl AusweisAppIfdLocal)
	else()
		target_link_libraries(AusweisAppUiAidl AusweisAppIfdRemote)
	endif()
endif()

target_compile_definitions(AusweisAppUiAidl PRIVATE QT_STATICPLUGIN)
