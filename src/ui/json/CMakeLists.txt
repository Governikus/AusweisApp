#####################################################################
# The json plugin implements the ui interface for internal usage.
#
# This plugin will be used by other ui plugins like AIDL or WebSocket
# to provide the SDK functionality. It provides no possibility for
# direct user control.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiJson)

target_link_libraries(AusweisAppUiJson PUBLIC ${Qt}::Core AusweisAppGlobal AusweisAppUi AusweisAppWorkflows AusweisAppCard)
target_compile_definitions(AusweisAppUiJson PRIVATE QT_STATICPLUGIN)

if(TARGET AusweisAppWorkflowsPersonalization)
	if(CONTAINER_SDK)
		target_link_libraries(AusweisAppUiJson PUBLIC AusweisAppWorkflowsPersonalization)
	else()
		target_link_libraries(AusweisAppUiJson PUBLIC debug AusweisAppWorkflowsPersonalization)
	endif()
endif()

if(TARGET ${Qt}::Test)
	target_link_libraries(AusweisAppUiJson PRIVATE debug ${Qt}::Test)
endif()
