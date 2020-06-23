FUNCTION(WRITE_QRC _dest_file _dir _prefix)
	FILE(GLOB_RECURSE files "${_dir}/*")

	FILE(WRITE "${_dest_file}" "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource prefix=\"/${_prefix}/\">\n")
	FOREACH(file ${files})
		STRING(REPLACE "${_dir}/" "" file_alias "${file}")
		FILE(APPEND "${_dest_file}" "<file alias=\"${file_alias}\">${file}</file>\n")
	ENDFOREACH()
	FILE(APPEND "${_dest_file}" "</qresource></RCC>")
ENDFUNCTION()


SET(QRC_FILES "ausweisapp.qrc")

IF(IOS OR ANDROID OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	LIST(APPEND QRC_FILES "ausweisapp_mobile.qrc")
ENDIF()

IF(DESKTOP)
	LIST(APPEND QRC_FILES "ausweisapp_desktop.qrc")
ENDIF()

IF(TARGET Qt5::Qml)
	SET(ausweisapp_qml.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_qml.qrc")
	WRITE_QRC("${ausweisapp_qml.qrc}" "${CMAKE_CURRENT_SOURCE_DIR}/qml" "qml")
	LIST(APPEND QRC_FILES "${ausweisapp_qml.qrc}")

	SET(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml CACHE STRING "qml files" FORCE)
ENDIF()

SET(RCC ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}.rcc)
SET(RCC ${RCC} PARENT_SCOPE)
qt5_add_binary_resources(AusweisAppRcc "${QRC_FILES}" DESTINATION ${RCC})
