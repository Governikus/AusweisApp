################################################
# Implements install target!
# It will be included by ./src/CMakeLists.txt
################################################

if(MACOS)
	set(DEFAULT_FILE_DESTINATION ${PROJECT_NAME}.app/Contents/Resources)
else()
	set(DEFAULT_FILE_DESTINATION .)
endif()


if(CMAKE_PREFIX_PATH)
	string(REPLACE "\\" "/" TOOLCHAIN_PATH ${CMAKE_PREFIX_PATH})
	set(TOOLCHAIN_BIN_PATH ${TOOLCHAIN_PATH}/bin)
	set(TOOLCHAIN_LIB_PATH ${TOOLCHAIN_PATH}/lib)
endif()

function(deploy_shared_app_libraries _destination)
	# This is for internal use only! It is not recommended to split the AusweisApp into multiple libs!
	if(BUILD_SHARED_LIBS)
		target_get_linked_libraries(AusweisAppBinary libraries)
		foreach(libTarget ${libraries})
			get_target_property(target_type "${libTarget}" TYPE)
			if(${target_type} STREQUAL "SHARED_LIBRARY")
				install(FILES $<TARGET_FILE:${libTarget}> DESTINATION ${_destination} COMPONENT Application)
			endif()
		endforeach()
	endif()
endfunction()


if(WIN32)
	if(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
		set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
		if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
			set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
			set(CMAKE_INSTALL_DEBUG_LIBRARIES TRUE)
			set(CMAKE_INSTALL_DEBUG_LIBRARIES_ONLY TRUE)
		endif()
		include(InstallRequiredSystemLibraries)

		set(COMPILER_RUNTIME --no-compiler-runtime)
	else()
		set(COMPILER_RUNTIME --compiler-runtime)
	endif()

	install(TARGETS AusweisAppBinary DESTINATION . COMPONENT Application)

	if(TARGET ${Qt}::Qml)
		qt_generate_deploy_qml_app_script(
			TARGET AusweisAppBinary
			EXCLUDE_PLUGINS qcertonlybackend
			OUTPUT_SCRIPT deploy_script
			DEPLOY_TOOL_OPTIONS --openssl-root "${TOOLCHAIN_PATH}" --no-system-d3d-compiler --no-system-dxc-compiler --no-translations ${COMPILER_RUNTIME}
		)
		install(CODE "set(QT_DEPLOY_BIN_DIR .)")
		install(SCRIPT ${deploy_script})
	endif()

	configure_file(${CMAKE_DIR}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)
	install(CODE
		"
		execute_process(COMMAND \"${CMAKE_COMMAND}\" -DSIGN_EXT=*.exe -P \"${CMAKE_BINARY_DIR}/SignFiles.cmake\" WORKING_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}\")
		" COMPONENT Application)


elseif(MACOS)
	set(MACOS_BUNDLE_MACOS_DIR ${DEFAULT_FILE_DESTINATION}/../MacOS)
	set(MACOS_BUNDLE_PLUGINS_DIR ${DEFAULT_FILE_DESTINATION}/../PlugIns)
	set(MACOS_BUNDLE_FRAMEWORKS_DIR ${DEFAULT_FILE_DESTINATION}/../Frameworks)
	set(MACOS_BUNDLE_RESOURCES_DIR ${DEFAULT_FILE_DESTINATION}/../Resources)
	set(MACOS_BUNDLE_LOGIN_ITEMS_DIR ${DEFAULT_FILE_DESTINATION}/../Library/LoginItems)

	install(TARGETS AusweisAppBinary BUNDLE DESTINATION . COMPONENT Application)
	install(TARGETS AusweisAppAutostartHelper BUNDLE DESTINATION ${MACOS_BUNDLE_LOGIN_ITEMS_DIR} COMPONENT Application)
	deploy_shared_app_libraries(${MACOS_BUNDLE_FRAMEWORKS_DIR})

	if(TARGET ${Qt}::Qml)
		set(DEPLOY_TOOL_OPTIONS "-hardened-runtime -timestamp \"-codesign=${CODE_SIGN_IDENTITY}\"")

		qt_generate_deploy_script(
			TARGET AusweisAppAutostartHelper
			OUTPUT_SCRIPT deploy_script_helper
			CONTENT "
				qt_deploy_runtime_dependencies(
					EXECUTABLE \"${MACOS_BUNDLE_LOGIN_ITEMS_DIR}/$<TARGET_FILE_NAME:AusweisAppAutostartHelper>.app\"
					DEPLOY_TOOL_OPTIONS ${DEPLOY_TOOL_OPTIONS}
					NO_PLUGINS
				)
			"
		)
		install(SCRIPT ${deploy_script_helper})

		if(QT_VERSION VERSION_LESS "6.12.0")
			set(MACOS_BUNDLE_POST_BUILD MACOS_BUNDLE_POST_BUILD)
		else()
			separate_arguments(DEPLOY_TOOL_OPTIONS NATIVE_COMMAND "${DEPLOY_TOOL_OPTIONS}")
		endif()
		qt_generate_deploy_qml_app_script(
			TARGET AusweisAppBinary
			${MACOS_BUNDLE_POST_BUILD}
			DEPLOY_TOOL_OPTIONS ${DEPLOY_TOOL_OPTIONS}
			OUTPUT_SCRIPT deploy_script
		)
		install(SCRIPT ${deploy_script})
	endif()


elseif(IOS)


elseif(ANDROID)


elseif(UNIX)
	if(BUILD_SHARED_LIBS)
		set(CMAKE_INSTALL_RPATH "\$ORIGIN")
	endif()

	set(DEFAULT_FILE_DESTINATION ${CMAKE_INSTALL_DATADIR}/${VENDOR}/AusweisApp)

	if(INTEGRATED_SDK AND NOT CONTAINER_SDK)
		GET_PUBLIC_HEADER(AusweisAppBinary PUBLIC_HEADER)
		if(PUBLIC_HEADER)
			install(FILES ${PUBLIC_HEADER} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
		endif()
		install(TARGETS AusweisAppBinary DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Application)
	else()
		install(TARGETS AusweisAppBinary DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Application)
	endif()

	if(NOT CONTAINER_SDK)
		configure_file(${PACKAGING_DIR}/linux/${BUNDLE_IDENTIFIER}.metainfo.xml.in ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.metainfo.xml @ONLY)
		configure_file(${PACKAGING_DIR}/linux/${BUNDLE_IDENTIFIER}.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.desktop @ONLY)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo COMPONENT Application)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications COMPONENT Application)
		install(FILES ${RESOURCES_DIR}/images/npa.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps COMPONENT Application RENAME AusweisApp.svg)
		install(FILES ${RESOURCES_DIR}/images/npa.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/96x96/apps COMPONENT Application RENAME AusweisApp.png)
		install(FILES ${DOCS_DIR}/AusweisApp.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 COMPONENT Application)
	endif()

	deploy_shared_app_libraries(${CMAKE_INSTALL_LIBDIR})
endif()


if((NOT INTEGRATED_SDK OR CONTAINER_SDK) AND NOT ANDROID)
	# resources file
	install(FILES ${RCC} DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
endif()
