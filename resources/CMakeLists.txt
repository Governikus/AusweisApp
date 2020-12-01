function(WRITE_QRC _dest_file _dir _prefix _wildcard)
	file(GLOB_RECURSE files "${_dir}/${_wildcard}")

	file(WRITE "${_dest_file}" "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource prefix=\"${_prefix}/\">\n")
	foreach(file ${files})
		string(REPLACE "${_dir}/" "" file_alias "${file}")
		file(APPEND "${_dest_file}" "<file alias=\"${file_alias}\">${file}</file>\n")
	endforeach()
	file(APPEND "${_dest_file}" "</qresource></RCC>")
endfunction()

function(ADD_CONFIG_JSON_RESOURCE _var)
	set(ausweisapp_config.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_config.qrc")
	WRITE_QRC("${ausweisapp_config.qrc}" "${CMAKE_CURRENT_BINARY_DIR}" "/" "config.json")
	qt_add_resources(${_var} ${ausweisapp_config.qrc})
	set(${_var} ${${_var}} PARENT_SCOPE)
endfunction()


set(QRC_FILES "ausweisapp.qrc")

if(IOS OR ANDROID OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	list(APPEND QRC_FILES "ausweisapp_mobile.qrc")
endif()

if (IOS)
	list(APPEND QRC_FILES "ausweisapp_ios.qrc")
elseif(ANDROID OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	list(APPEND QRC_FILES "ausweisapp_android.qrc")
endif()

if(DESKTOP)
	list(APPEND QRC_FILES "ausweisapp_desktop.qrc")
endif()

if(TARGET ${Qt}::Qml)
	set(ausweisapp_qml.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_qml.qrc")
	WRITE_QRC("${ausweisapp_qml.qrc}" "${CMAKE_CURRENT_SOURCE_DIR}/qml" "/qml" "*")
	list(APPEND QRC_FILES "${ausweisapp_qml.qrc}")

	set(ausweisapp_license.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_license.qrc")
	configure_file("${RESOURCES_DIR}/ausweisapp_license.qrc.in" "${ausweisapp_license.qrc}" @ONLY)
	list(APPEND QRC_FILES "${ausweisapp_license.qrc}")

	set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml CACHE STRING "qml files" FORCE)
endif()

set(RCC ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}.rcc)
set(RCC ${RCC} PARENT_SCOPE)
qt_add_binary_resources(AusweisAppRcc "${QRC_FILES}" DESTINATION ${RCC})
