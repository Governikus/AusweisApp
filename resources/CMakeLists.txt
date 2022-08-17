function(WRITE_QRC _dest_file _dir _prefix _files)
	file(WRITE "${_dest_file}" "<!DOCTYPE RCC><RCC version=\"1.0\"><qresource prefix=\"${_prefix}/\">\n")
	foreach(file ${_files})
		string(REPLACE "${_dir}/" "" file_alias "${file}")
		file(APPEND "${_dest_file}" "<file alias=\"${file_alias}\">${file}</file>\n")
	endforeach()
	file(APPEND "${_dest_file}" "</qresource></RCC>")
endfunction()

function(WRITE_GLOB_QRC _dest_file _dir _prefix _wildcard)
	file(GLOB_RECURSE files "${_dir}/${_wildcard}")
	WRITE_QRC("${_dest_file}" "${_dir}" "${_prefix}" "${files}")
endfunction()

function(ADD_SHADERS_TO_TARGET _target)
	if(TARGET ${Qt}::Qml AND QT6 AND NOT INTEGRATED_SDK)
		file(GLOB QT6_SHADERS "${RESOURCES_DIR}/shader/qt6/*.frag")
		message(STATUS "QT6_SHADERS " "${QT6_SHADERS}")
		foreach(shaderPath ${QT6_SHADERS})
			get_filename_component(shaderName ${shaderPath} NAME)
			list(APPEND QT6_SHADER_OUTPUTS ${shaderName})
		endforeach()
		message(STATUS "QT6_SHADER_OUTPUTS " "${QT6_SHADER_OUTPUTS}")
		qt6_add_shaders(${_target} "${_target}_shader"
				PRECOMPILE
				PREFIX "/shader/+qt6"
				FILES "${QT6_SHADERS}"
				OUTPUTS "${QT6_SHADER_OUTPUTS}")
	endif()
endfunction()


if(EXISTS "${CMAKE_DIR}/SmartService.internal.cmake")
	include(SmartService.internal)
endif()
configure_file(${RESOURCES_DIR}/config.json.in ${CMAKE_CURRENT_BINARY_DIR}/config.json @ONLY)


set(ausweisapp_config.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_config.qrc")
WRITE_QRC("${ausweisapp_config.qrc}" "${CMAKE_CURRENT_BINARY_DIR}" "" "${CMAKE_CURRENT_BINARY_DIR}/config.json")
if(INTEGRATED_SDK AND NOT CONTAINER_SDK)
	qt_add_resources(AusweisAppConfigQrc ${ausweisapp_config.qrc} OPTIONS -compress-algo zlib)
	ADD_OBJECT_LIBRARY(AusweisAppConfig ${AusweisAppConfigQrc})
else()
	list(APPEND QRC_FILES "${ausweisapp_config.qrc}")
endif()


if(NOT INTEGRATED_SDK OR BUILD_TESTING)
	list(APPEND QRC_FILES "ausweisapp.qrc")
endif()

if((IOS OR ANDROID) AND NOT INTEGRATED_SDK OR BUILD_TESTING)
	list(APPEND QRC_FILES "ausweisapp_mobile.qrc")
endif()

if (IOS)
	list(APPEND QRC_FILES "ausweisapp_ios.qrc")
elseif((ANDROID AND NOT INTEGRATED_SDK) OR BUILD_TESTING)
	list(APPEND QRC_FILES "ausweisapp_android.qrc")
endif()

if((DESKTOP AND NOT INTEGRATED_SDK) OR BUILD_TESTING)
	list(APPEND QRC_FILES "ausweisapp_desktop.qrc")
endif()

if((DESKTOP OR INTEGRATED_SDK) OR BUILD_TESTING)
	list(APPEND QRC_FILES "ausweisapp_webservice.qrc")
endif()


if(TARGET ${Qt}::Qml)
	file(GLOB_RECURSE QMLDIR_FILES "${CMAKE_CURRENT_SOURCE_DIR}/qml" qmldir)
	set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${QMLDIR_FILES})

	set(ausweisapp_qml.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_qml.qrc")
	WRITE_GLOB_QRC("${ausweisapp_qml.qrc}" "${CMAKE_CURRENT_SOURCE_DIR}/qml" "/qml" "*")
	list(APPEND QRC_FILES "${ausweisapp_qml.qrc}")

	set(ausweisapp_license.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_license.qrc")
	configure_file("${RESOURCES_DIR}/ausweisapp_license.qrc.in" "${ausweisapp_license.qrc}" @ONLY)
	list(APPEND QRC_FILES "${ausweisapp_license.qrc}")

	set(QML_IMPORT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/qml CACHE STRING "qml files" FORCE)

	add_subdirectory(qml)
endif()

if(NOT INTEGRATED_SDK)
	file(GLOB TRANSLATION_FILES ${RESOURCES_DIR}/translations/*.ts)

	foreach(filepath ${TRANSLATION_FILES})
		get_filename_component(filename "${filepath}" NAME)
		string(REGEX MATCH "_(.*)\.ts$" _unused "${filename}")
		list(APPEND USED_TRANSLATIONS ${CMAKE_MATCH_1})
	endforeach()

	set_property(SOURCE ${TRANSLATION_FILES} PROPERTY OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/translations)
	foreach(locale ${USED_TRANSLATIONS})
		file(COPY ${QT_INSTALL_TRANSLATIONS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILES_MATCHING REGEX "qtbase_${locale}")
	endforeach()
	file(GLOB EXISTING_QM_FILES ${CMAKE_CURRENT_BINARY_DIR}/translations/*.qm)

	option(UPDATE_TRANSLATIONS "Update translations/*.ts files (WARNING: make clean will delete the *.ts files!)")
	if(UPDATE_TRANSLATIONS)
		option(UPDATE_TRANSLATIONS_NO_OBSOLETE "Remove obsolete translations from translations/*.ts files )")

		set(LUPDATE_OPTIONS -extensions c,cpp,h,ui,m,mm,qml,js -no-ui-lines -locations none)
		if(UPDATE_TRANSLATIONS_NO_OBSOLETE)
			qt_create_translation(QM_FILES ${SRC_DIR} ${RESOURCES_DIR} ${TRANSLATION_FILES} OPTIONS ${LUPDATE_OPTIONS} -no-obsolete)
		else()
			qt_create_translation(QM_FILES ${SRC_DIR} ${RESOURCES_DIR} ${TRANSLATION_FILES} OPTIONS ${LUPDATE_OPTIONS})
		endif()

		add_custom_target(update.translations DEPENDS ${QM_FILES})
		message(WARNING "'make clean' will delete *.ts files!")
	else()
		qt_add_translation(QM_FILES ${TRANSLATION_FILES})
	endif()

	list(APPEND QM_FILES ${EXISTING_QM_FILES})
	list(REMOVE_DUPLICATES QM_FILES)

	set(ausweisapp_translations.qrc "${CMAKE_CURRENT_BINARY_DIR}/ausweisapp_translations.qrc")
	WRITE_QRC("${ausweisapp_translations.qrc}" "${CMAKE_CURRENT_BINARY_DIR}" "" "${QM_FILES}")
	list(APPEND QRC_FILES "${ausweisapp_translations.qrc}")
endif()


if(QRC_FILES)
	set(RCC ${CMAKE_BINARY_DIR}/src/${PROJECT_NAME}.rcc)
	set(RCC ${RCC} PARENT_SCOPE)
	qt_add_binary_resources(AusweisAppRcc "${QRC_FILES}" DESTINATION ${RCC} OPTIONS -compress-algo zlib)
endif()
