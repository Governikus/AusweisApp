################################################
# Implements install target!
# It will be included by ./src/CMakeLists.txt
################################################

if(MAC)
	set(DEFAULT_FILE_DESTINATION ${PROJECT_NAME}.app/Contents/Resources)
else()
	set(DEFAULT_FILE_DESTINATION .)
endif()


if(CMAKE_PREFIX_PATH)
	string(REPLACE "\\" "/" TOOLCHAIN_PATH ${CMAKE_PREFIX_PATH})
	set(TOOLCHAIN_BIN_PATH ${TOOLCHAIN_PATH}/bin)
	set(TOOLCHAIN_LIB_PATH ${TOOLCHAIN_PATH}/lib)
endif()


set(SEARCH_ADDITIONAL_DIRS "
			set(CMAKE_MODULE_PATH \"${CMAKE_MODULE_PATH}\")
			include(Helper)
			DIRLIST_OF_FILES(ADDITIONAL_DIRS ${CMAKE_BINARY_DIR}/src/*${CMAKE_SHARED_LIBRARY_SUFFIX})
")

set(DEPENDENCY_CHECK "
	function(gp_resolved_file_type_override file type)
		message(STATUS \"Collect dependency: \${file}\")

		if(file MATCHES \"libstdc.+\.dll\"
			OR file MATCHES \"libwinpthread-.+\.dll\"
			OR file MATCHES \"libgcc_s_.+\.dll\"
			OR file MATCHES \"libssp-.+\.dll\")

			get_filename_component(path \"${CMAKE_CXX_COMPILER}\" DIRECTORY)
			get_filename_component(pathDest \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\" DIRECTORY)
			if(NOT file MATCHES \"\${path}\" AND NOT file MATCHES \"\${pathDest}\")
				message(FATAL_ERROR \"Wrong source path detected: \${file} | Should be: \${path} or \${pathDest}\")
			endif()

		endif()
	endfunction()
")


# qt qml plugins (fixup_bundle needs to know this to fetch their dependencies)
if((WIN32 OR MAC) AND TARGET ${Qt}::Qml)
	set(modules QtQuick QtQml Qt)

	foreach(entry ${modules})
		set(_lib_dir ${QT_INSTALL_ARCHDATA}/qml/${entry})
		file(GLOB_RECURSE _libs "${_lib_dir}/*.dll" "${_lib_dir}/*.dylib")
		list(APPEND LIBS ${_libs})
		install(DIRECTORY ${_lib_dir} DESTINATION ${DEFAULT_FILE_DESTINATION}/qml COMPONENT Runtime PATTERN "*.dylib" EXCLUDE)
	endforeach()
endif()


if(WIN32)
	if(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
		set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
		set(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
		include(InstallRequiredSystemLibraries)
	endif()

	if(TARGET ${Qt}::Qml)
		FETCH_TARGET_LOCATION(libQuickControls2 "${Qt}::QuickControls2")
		install(FILES ${libQuickControls2} DESTINATION . COMPONENT Runtime)
		list(APPEND LIBS ${libQuickControls2})
		if(TARGET ${Qt}::QmlWorkerScript)
			FETCH_TARGET_LOCATION(libQmlWorkerScript "${Qt}::QmlWorkerScript")
			install(FILES ${libQmlWorkerScript} DESTINATION . COMPONENT Runtime)
		endif()

		if(TARGET ${Qt}::Gui_EGL AND TARGET ${Qt}::Gui_GLESv2)
			FETCH_TARGET_LOCATION(libEgl "${Qt}::Gui_EGL")
			FETCH_TARGET_LOCATION(libGLES "${Qt}::Gui_GLESv2")
			install(FILES ${libEgl} DESTINATION . COMPONENT Runtime)
			install(FILES ${libGLES} DESTINATION . COMPONENT Runtime)
		endif()
	endif()

	FETCH_TARGET_LOCATION(openSslBackend "${Qt}::QTlsBackendOpenSSLPlugin")
	install(FILES ${openSslBackend} DESTINATION tls COMPONENT Runtime)
	list(APPEND LIBS ${openSslBackend})

	FETCH_TARGET_LOCATION(pluginSvg "${Qt}::QSvgPlugin")
	FETCH_TARGET_LOCATION(pluginGif "${Qt}::QGifPlugin")
	FETCH_TARGET_LOCATION(pluginJpeg "${Qt}::QJpegPlugin")
	FETCH_TARGET_LOCATION(platformWin "${Qt}::QWindowsIntegrationPlugin")

	if(QT_VERSION VERSION_LESS 6.7)
		FETCH_TARGET_LOCATION(styleWin "${Qt}::QWindowsVistaStylePlugin")
	else()
		FETCH_TARGET_LOCATION(styleWin "${Qt}::QModernWindowsStylePlugin")
	endif()

	install(TARGETS AusweisAppBinary DESTINATION . COMPONENT Application)
	install(FILES ${pluginSvg} DESTINATION imageformats COMPONENT Runtime)
	install(FILES ${pluginGif} DESTINATION imageformats COMPONENT Runtime)
	install(FILES ${pluginJpeg} DESTINATION imageformats COMPONENT Runtime)
	install(FILES ${platformWin} DESTINATION platforms COMPONENT Runtime)
	install(FILES ${styleWin} DESTINATION styles COMPONENT Runtime)
	list(APPEND LIBS ${pluginSvg} ${pluginGif} ${pluginJpeg} ${platformWin} ${styleWin})

	install(CODE
		"
		${DEPENDENCY_CHECK}
		${SEARCH_ADDITIONAL_DIRS}
		include(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\" \"${LIBS}\" \"${TOOLCHAIN_BIN_PATH};\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)



elseif(MAC)
	set(MACOS_BUNDLE_MACOS_DIR ${DEFAULT_FILE_DESTINATION}/../MacOS)
	set(MACOS_BUNDLE_PLUGINS_DIR ${DEFAULT_FILE_DESTINATION}/../PlugIns)
	set(MACOS_BUNDLE_FRAMEWORKS_DIR ${DEFAULT_FILE_DESTINATION}/../Frameworks)
	set(MACOS_BUNDLE_RESOURCES_DIR ${DEFAULT_FILE_DESTINATION}/../Resources)
	set(MACOS_BUNDLE_LOGIN_ITEMS_DIR ${DEFAULT_FILE_DESTINATION}/../Library/LoginItems)

	install(TARGETS AusweisAppBinary BUNDLE DESTINATION . COMPONENT Application)
	install(TARGETS AusweisAppAutostartHelper BUNDLE DESTINATION ${MACOS_BUNDLE_LOGIN_ITEMS_DIR} COMPONENT Application)

	function(install_mac_plugins plugins)
		foreach(plugin ${plugins})
			get_target_property(pluginPath ${plugin} LOCATION)
			get_filename_component(pluginDir ${pluginPath} DIRECTORY)
			get_filename_component(pluginName ${pluginPath} NAME)
			get_filename_component(pluginDirName ${pluginDir} NAME)

			if(pluginDirName STREQUAL "platforms" AND NOT plugin STREQUAL "${Qt}::QCocoaIntegrationPlugin")
				continue()
			endif()

			install(FILES ${pluginPath} DESTINATION ${MACOS_BUNDLE_PLUGINS_DIR}/${pluginDirName} COMPONENT Runtime)
		endforeach()
	endfunction(install_mac_plugins)

	# We need to include the following (i.e. all) image format plug-ins,
	# since those seem to be loaded upon program start-up. Not including
	# them would cause the respective add-on from a Qt installation (if
	# any) to be loaded, which would in turn cause the Qt libraries they
	# depend on to be loaded as well, thus resulting in two sets of Qt
	# libraries being loaded (ours from the bundle and the ones from the
	# installation) and the program misbehaving (crashing).
	# Workaround for QTBUG-94066
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QTuioTouchPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QSvgIconPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QGifPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QICNSPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QICOPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QJpegPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QMacHeifPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QMacJp2PluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QSvgPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Gui/Qt6QCocoaIntegrationPluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Widgets/Qt6QMacStylePluginTargets.cmake")
	include("${QT_INSTALL_ARCHDATA}/lib/cmake/Qt6Network/Qt6QTlsBackendOpenSSLPluginTargets.cmake")
	set(plugins ${Qt}::QTuioTouchPlugin ${Qt}::QSvgIconPlugin ${Qt}::QGifPlugin ${Qt}::QICNSPlugin ${Qt}::QICOPlugin ${Qt}::QJpegPlugin ${Qt}::QMacHeifPlugin ${Qt}::QMacJp2Plugin ${Qt}::QSvgPlugin ${Qt}::QCocoaIntegrationPlugin ${Qt}::QMacStylePlugin ${Qt}::QTlsBackendOpenSSLPlugin)
	install_mac_plugins("${plugins}")

	if(TARGET ${Qt}::Qml)
		foreach(entry QtQuick QtQuick.2 QtQml Qt)
			set(_dir "${QT_INSTALL_ARCHDATA}/qml")
			file(GLOB_RECURSE DYLIB "${_dir}/${entry}/*.dylib")
			foreach(_lib ${DYLIB})
				file(RELATIVE_PATH _lib_dest "${_dir}" "${_lib}")
				if(NOT _lib_dest MATCHES "XmlListModel|Particles.2|LocalStorage|Shapes") # not needed stuff
					get_filename_component(_lib_dest_dir ${_lib_dest} DIRECTORY)
					install(FILES ${_lib} DESTINATION ${MACOS_BUNDLE_RESOURCES_DIR}/qml/${_lib_dest_dir} COMPONENT Runtime)
				endif()
			endforeach()
		endforeach()
	endif()

	install(CODE
		"
		${DEPENDENCY_CHECK}
		${SEARCH_ADDITIONAL_DIRS}
		file(GLOB_RECURSE QTPLUGINS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_PLUGINS_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		file(GLOB_RECURSE QtQuickPLUGINS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_RESOURCES_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		include(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_MACOS_DIR}/${PROJECT_NAME}\" \"\${QTPLUGINS};\${QtQuickPLUGINS}\" \"${TOOLCHAIN_LIB_PATH};\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)

elseif(IOS)


elseif(ANDROID)
	if(INTEGRATED_SDK)
		add_custom_command(TARGET AusweisAppBinary POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:AusweisAppBinary>" "${ANDROID_BUILD_DIR}/libs/${CMAKE_ANDROID_ARCH_ABI}/$<TARGET_FILE_NAME:AusweisAppBinary>"
		)
	endif()


elseif(UNIX)
	if(BUILD_SHARED_LIBS)
		set(CMAKE_INSTALL_RPATH "\$ORIGIN")
	endif()

	set(DEFAULT_FILE_DESTINATION ${CMAKE_INSTALL_DATADIR}/${VENDOR}/AusweisApp)
	install(TARGETS AusweisAppBinary DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Application)
	if(NOT CONTAINER_SDK)
		configure_file(${PACKAGING_DIR}/linux/${BUNDLE_IDENTIFIER}.metainfo.xml.in ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.metainfo.xml @ONLY)
		configure_file(${PACKAGING_DIR}/linux/${BUNDLE_IDENTIFIER}.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.desktop @ONLY)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.metainfo.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo COMPONENT Application)
		install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${BUNDLE_IDENTIFIER}.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications COMPONENT Application)
		install(FILES ${RESOURCES_DIR}/images/npa.svg DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps COMPONENT Application RENAME AusweisApp.svg)
		install(FILES ${RESOURCES_DIR}/images/npa.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/96x96/apps COMPONENT Application RENAME AusweisApp.png)
		install(FILES ${DOCS_DIR}/AusweisApp.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 COMPONENT Application)
	endif()

	# This is for internal use only! It is not recommended to split the AusweisApp into multiple libs!
	if(BUILD_SHARED_LIBS)
		target_get_linked_libraries(AusweisAppBinary libraries)
		foreach(libTarget ${libraries})
			install(TARGETS ${libTarget} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Application)
		endforeach()
	endif()
endif()




if(LINUX OR WIN32 OR MAC)
	option(SELFPACKER "Compress executable with self packer like UPX")
	if(SELFPACKER)
		find_package(SelfPackers)
		if(SELF_PACKER_FOR_EXECUTABLE)
			message(STATUS "Using SelfPacker: ${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS}")
		else()
			message(FATAL_ERROR "Cannot find self packer")
		endif()

		install(CODE
			"
			execute_process(COMMAND
				${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\")
			" COMPONENT Application)
	endif()
endif()


if((NOT INTEGRATED_SDK OR CONTAINER_SDK) AND NOT ANDROID)
	# resources file
	install(FILES ${RCC} DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
endif()


if(SIGNTOOL_CMD)
	if(IOS)
		set(SIGNTOOL_PARAMS "-s \"${XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY}\"")
	endif()

	configure_file(${CMAKE_DIR}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)

	if(MAC)
		option(OSX_TIMESTAMP "Timestamp the application bundle" ON)

		install(CODE
			"
			execute_process(COMMAND \"${CMAKE_COMMAND}\" -DOSX_TIMESTAMP=${OSX_TIMESTAMP} -P \"${CMAKE_BINARY_DIR}/SignFiles.cmake\" WORKING_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\")
			" COMPONENT Application)
	elseif(WIN32)
		install(CODE
			"
			execute_process(COMMAND \"${CMAKE_COMMAND}\" -DSIGN_EXT=*.exe -P \"${CMAKE_BINARY_DIR}/SignFiles.cmake\" WORKING_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}\")
			" COMPONENT Application)
	endif()
endif()
