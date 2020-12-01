#####################################################################
# The qml plugin implements the ui interface for mobile systems.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiQml)

target_link_libraries(AusweisAppUiQml ${Qt}::Core ${Qt}::Svg ${Qt}::Qml ${Qt}::Quick ${Qt}::QuickControls2)
target_link_libraries(AusweisAppUiQml AusweisAppGlobal AusweisAppCore AusweisAppUi AusweisAppRemoteDevice AusweisAppUiCommon AusweisAppExport)

if(TARGET ${Qt}::QmlWorkerScript)
	target_link_libraries(AusweisAppUiQml ${Qt}::QmlWorkerScript)
endif()

if(NOT DESKTOP)
	target_link_libraries(AusweisAppUiQml ${Qt}::QuickShapes)
endif()

if(ANDROID)
	target_link_libraries(AusweisAppUiQml AusweisAppWhitelistClient)
else()
	target_link_libraries(AusweisAppUiQml debug AusweisAppWhitelistClient)
endif()

target_compile_definitions(AusweisAppUiQml PRIVATE QT_STATICPLUGIN)
