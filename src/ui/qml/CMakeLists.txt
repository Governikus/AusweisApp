############################################################################
# The qml plugin implements the ui interface for desktop and mobile systems.
############################################################################

add_subdirectory(modules)

ADD_PLATFORM_LIBRARY(AusweisAppUiQml EXCLUDE "/modules/")

target_link_libraries(AusweisAppUiQml PUBLIC ${Qt}::Core ${Qt}::Svg ${Qt}::Qml ${Qt}::Quick ${Qt}::QuickControls2)
target_link_libraries(AusweisAppUiQml PUBLIC AusweisAppGlobal AusweisAppUi AusweisAppIfdRemote AusweisAppServices AusweisAppWorkflowsSelfAuth AusweisAppUiQmlModules)

target_link_libraries(AusweisAppUiQml PUBLIC ${Qt}::CorePrivate)

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

if(TARGET AusweisAppWorkflowsPersonalization)
	if((ANDROID OR IOS) AND NOT INTEGRATED_SDK)
		target_link_libraries(AusweisAppUiQml PUBLIC AusweisAppWorkflowsPersonalization)
	else()
		target_link_libraries(AusweisAppUiQml PUBLIC debug AusweisAppWorkflowsPersonalization)
	endif()
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

if(QT_VERSION VERSION_GREATER_EQUAL "6.7.1"
		OR (QT_VERSION VERSION_GREATER_EQUAL "6.5.6" AND QT_VERSION VERSION_LESS "6.6")
	)
	if(TARGET AusweisAppDiagnosis)
		qt_generate_foreign_qml_types(AusweisAppDiagnosis AusweisAppUiQml)
	endif()
	qt_generate_foreign_qml_types(AusweisAppGlobal AusweisAppUiQml)
	qt_generate_foreign_qml_types(AusweisAppUi AusweisAppUiQml)
	qt_generate_foreign_qml_types(AusweisAppCard AusweisAppUiQml)
else()
	set_property(SOURCE UiPluginQml.cpp PROPERTY COMPILE_DEFINITIONS USE_CUSTOM_REGISTRATION)
endif()

if(QT_VERSION VERSION_LESS "6.5")
	set(COMPONENT_BEHAVIOR "pragma ComponentBehavior: Bound")
	file(GLOB_RECURSE INCOMPATIBLE_QML_FILES "modules/*.qml")
	foreach(ABSOLUTE_FILE_PATH ${INCOMPATIBLE_QML_FILES})
		file(READ "${ABSOLUTE_FILE_PATH}" FILE_CONTENT)
		if (FILE_CONTENT MATCHES ${COMPONENT_BEHAVIOR})
			string(REPLACE "${COMPONENT_BEHAVIOR}" "" FILE_CONTENT "${FILE_CONTENT}")
			message(STATUS "Removed \"${COMPONENT_BEHAVIOR}\" from ${ABSOLUTE_FILE_PATH} because of QTBUG-108684")
			file(WRITE "${ABSOLUTE_FILE_PATH}" "${FILE_CONTENT}")
		endif()
	endforeach()
endif()
