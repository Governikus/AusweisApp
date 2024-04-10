############################################################################
# The qml plugin implements the ui interface for desktop and mobile systems.
############################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiQml)

target_link_libraries(AusweisAppUiQml ${Qt}::Core ${Qt}::Svg ${Qt}::Qml ${Qt}::Quick ${Qt}::QuickControls2)
target_link_libraries(AusweisAppUiQml AusweisAppGlobal AusweisAppUi AusweisAppIfdRemote AusweisAppServices AusweisAppWorkflowsSelfAuth)

target_link_libraries(AusweisAppUiQml ${Qt}::CorePrivate)

if(TARGET ${Qt}::Widgets)
	target_link_libraries(AusweisAppUiQml ${Qt}::Widgets) # QSystemTrayIcon
endif()

if(TARGET ${Qt}::QmlWorkerScript)
	target_link_libraries(AusweisAppUiQml ${Qt}::QmlWorkerScript)
endif()

if(TARGET AusweisAppDiagnosis)
	target_link_libraries(AusweisAppUiQml AusweisAppDiagnosis)
endif()

if(TARGET ${Qt}::WebSockets)
	target_link_libraries(AusweisAppUiQml AusweisAppWorkflowsIfd)
endif()

if(ANDROID)
	target_link_libraries(AusweisAppUiQml AusweisAppWhitelistClient)
else()
	target_link_libraries(AusweisAppUiQml debug AusweisAppWhitelistClient)
endif()

if(MAC)
	target_link_libraries(AusweisAppUiQml ${OSX_USERNOTIFICATIONS})
endif()

if(TARGET AusweisAppWorkflowsPersonalization)
	if((ANDROID OR IOS) AND NOT INTEGRATED_SDK)
		target_link_libraries(AusweisAppUiQml AusweisAppWorkflowsPersonalization)
	else()
		target_link_libraries(AusweisAppUiQml debug AusweisAppWorkflowsPersonalization)
	endif()
endif()

target_compile_definitions(AusweisAppUiQml PRIVATE QT_STATICPLUGIN)
