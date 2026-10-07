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

if(MACOS)
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

function(STRIP_FROM_QML_FILES _string _reason)
	file(GLOB_RECURSE QML_FILES "modules/*.qml")
	foreach(ABSOLUTE_FILE_PATH ${QML_FILES})
		file(READ "${ABSOLUTE_FILE_PATH}" FILE_CONTENT)
		if (FILE_CONTENT MATCHES ${_string})
			string(REGEX REPLACE "${_string}" "" FILE_CONTENT "${FILE_CONTENT}")
			message(STATUS "Removed \"${_string}\" from ${ABSOLUTE_FILE_PATH}, reason: ${_reason}")
			file(WRITE "${ABSOLUTE_FILE_PATH}" "${FILE_CONTENT}")
		endif()
	endforeach()
endfunction()

if(QT_VERSION VERSION_LESS "6.8.3")
	STRIP_FROM_QML_FILES("\t*Accessible\.id: [^\n]*\n" "Qt version >= 6.8.3 required for Accessible.id")
endif()

if(NOT LIBS_GOVERNIKUS OR QT_VERSION VERSION_LESS "6.11")
	STRIP_FROM_QML_FILES("\t*Accessible\.onShowOnScreenAction: [^\n]*\n" "Presumably Qt version >= 6.12 releases Accessible.onShowOnScreenAction")
endif()

if(NOT LIBS_GOVERNIKUS AND QT_VERSION VERSION_LESS "6.12")
	STRIP_FROM_QML_FILES("\t*Accessible\.expandable: [^\n]*\n" "Presumably Qt version >= 6.12 releases Accessible.expandable")
	STRIP_FROM_QML_FILES("\t*Accessible\.expanded: [^\n]*\n" "Presumably Qt version >= 6.12 releases Accessible.expanded")
endif()
