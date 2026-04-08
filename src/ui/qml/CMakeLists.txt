############################################################################
# The qml plugin implements the ui interface for desktop and mobile systems.
############################################################################

add_subdirectory(modules)

ADD_PLATFORM_LIBRARY(AusweisAppUiQml EXCLUDE "/modules/")

target_link_libraries(AusweisAppUiQml PUBLIC ${Qt}::Core ${Qt}::Svg ${Qt}::Qml ${Qt}::Quick ${Qt}::QuickControls2)
target_link_libraries(AusweisAppUiQml PUBLIC AusweisAppGlobal AusweisAppUi AusweisAppIfdRemote AusweisAppServices AusweisAppWorkflowsSelfAuth AusweisAppUiQmlModules)

if(ANDROID)
	target_link_libraries(AusweisAppUiQml PUBLIC ${Qt}::CorePrivate)
endif()

if(TARGET ${Qt}::Widgets)
	target_link_libraries(AusweisAppUiQml PUBLIC ${Qt}::Widgets) # QSystemTrayIcon
endif()

if(TARGET ${Qt}::QmlWorkerScript)
	target_link_libraries(AusweisAppUiQml PUBLIC ${Qt}::QmlWorkerScript)
endif()

if(TARGET AusweisAppDiagnosis)
	target_link_libraries(AusweisAppUiQml PUBLIC AusweisAppDiagnosis)
endif()

if(TARGET ${Qt}::WebSockets)
	target_link_libraries(AusweisAppUiQml PUBLIC AusweisAppWorkflowsIfd)
endif()

if(MAC)
	target_link_libraries(AusweisAppUiQml PUBLIC ${OSX_USERNOTIFICATIONS})
endif()

target_compile_definitions(AusweisAppUiQml PRIVATE QT_STATICPLUGIN)


qt_add_qml_module(AusweisAppUiQml
	URI "Governikus.Type"
	VERSION "${PROJECT_VERSION}"
	OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/modules/common/Governikus/Type
	RESOURCE_PREFIX /qml
	NO_CACHEGEN
	NO_IMPORT_SCAN
	DEPENDENCIES QtQml QtQml.Models QtQuick
)

get_target_property(target_type AusweisAppUiQmlplugin TYPE)
if(target_type STREQUAL STATIC_LIBRARY)
	target_link_libraries(AusweisAppUiQml INTERFACE AusweisAppUiQmlplugin)
endif()

if(TARGET AusweisAppDiagnosis)
	qt_generate_foreign_qml_types(AusweisAppDiagnosis AusweisAppUiQml)
endif()
qt_generate_foreign_qml_types(AusweisAppGlobal AusweisAppUiQml)
qt_generate_foreign_qml_types(AusweisAppUi AusweisAppUiQml)
qt_generate_foreign_qml_types(AusweisAppCard AusweisAppUiQml)
