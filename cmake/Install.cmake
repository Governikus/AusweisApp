################################################
# Implements install target!
# It will be included by ./src/CMakeLists.txt
################################################

SET(TRANSLATION_DESTINATION translations)
SET(DEFAULT_FILE_DESTINATION .)

IF(CMAKE_PREFIX_PATH)
	STRING(REPLACE "\\" "/" TOOLCHAIN_PATH ${CMAKE_PREFIX_PATH})
	SET(TOOLCHAIN_BIN_PATH ${TOOLCHAIN_PATH}/bin)
	SET(TOOLCHAIN_LIB_PATH ${TOOLCHAIN_PATH}/lib)
ENDIF()


SET(SEARCH_ADDITIONAL_DIRS "
			SET(CMAKE_MODULE_PATH \"${CMAKE_MODULE_PATH}\")
			INCLUDE(Helper)
			DIRLIST_OF_FILES(ADDITIONAL_DIRS ${CMAKE_BINARY_DIR}/src/*${CMAKE_SHARED_LIBRARY_SUFFIX})
")

SET(DEPENDENCY_CHECK "
	FUNCTION(gp_resolved_file_type_override file type)
		MESSAGE(STATUS \"Collect dependency: \${file}\")

		IF(file MATCHES \"libstdc.+\.dll\"
			OR file MATCHES \"libwinpthread-.+\.dll\"
			OR file MATCHES \"libgcc_s_.+\.dll\"
			OR file MATCHES \"libssp-.+\.dll\")

			get_filename_component(path \"${CMAKE_CXX_COMPILER}\" DIRECTORY)
			get_filename_component(pathDest \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}\" DIRECTORY)
			IF(NOT file MATCHES \"\${path}\" AND NOT file MATCHES \"\${pathDest}\")
				MESSAGE(FATAL_ERROR \"Wrong source path detected: \${file} | Should be: \${path} or \${pathDest}\")
			ENDIF()

		ENDIF()
	ENDFUNCTION()
")


IF(WIN32)
	IF(MSVC)
		SET(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
		IF(NOT CMAKE_VERSION VERSION_LESS "3.6")
			SET(CMAKE_INSTALL_UCRT_LIBRARIES TRUE)
		ENDIF()
		INCLUDE(InstallRequiredSystemLibraries)
	ENDIF()

	IF(TARGET Qt5::Qml)
		FETCH_TARGET_LOCATION(libQuickControls2 "Qt5::QuickControls2")
		INSTALL(FILES ${libQuickControls2} DESTINATION . COMPONENT Runtime)
	ENDIF()
	FETCH_TARGET_LOCATION(libSvg "Qt5::Svg")
	FETCH_TARGET_LOCATION(pluginSvg "Qt5::QSvgPlugin")
	FETCH_TARGET_LOCATION(pluginGif "Qt5::QGifPlugin")
	FETCH_TARGET_LOCATION(pluginJpeg "Qt5::QJpegPlugin")
	IF(WINDOWS_STORE)
		FETCH_TARGET_LOCATION(platformWin "Qt5::QWinRTIntegrationPlugin")
	ELSE()
		FETCH_TARGET_LOCATION(platformWin "Qt5::QWindowsIntegrationPlugin")
	ENDIF()
	FETCH_TARGET_LOCATION(styleVista "Qt5::QWindowsVistaStylePlugin")

	INSTALL(TARGETS AusweisApp DESTINATION . COMPONENT Application)
	INSTALL(FILES ${libSvg} DESTINATION . COMPONENT Runtime)
	INSTALL(FILES ${pluginSvg} DESTINATION imageformats COMPONENT Runtime)
	INSTALL(FILES ${pluginGif} DESTINATION imageformats COMPONENT Runtime)
	INSTALL(FILES ${pluginJpeg} DESTINATION imageformats COMPONENT Runtime)
	INSTALL(FILES ${platformWin} DESTINATION platforms COMPONENT Runtime)
	INSTALL(FILES ${styleVista} DESTINATION styles COMPONENT Runtime)

	INSTALL(CODE
		"
		${DEPENDENCY_CHECK}
		${SEARCH_ADDITIONAL_DIRS}
		INCLUDE(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\" \"${libQuickControls2}\" \"${TOOLCHAIN_BIN_PATH};\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)



ELSEIF(APPLE AND NOT IOS)
	SET(MACOS_BUNDLE_MACOS_DIR ../MacOS)
	SET(MACOS_BUNDLE_PLUGINS_DIR ../PlugIns)
	SET(MACOS_BUNDLE_FRAMEWORKS_DIR ../Frameworks)
	SET(MACOS_BUNDLE_RESOURCES_DIR ../Resources)
	SET(MACOS_BUNDLE_LOGIN_ITEMS_DIR ../Library/LoginItems)

	# We need to include the following (i.e. all) image format plug-ins,
	# since those seem to be loaded upon program start-up. Not including
	# them would cause the respective add-on from a Qt installation (if
	# any) to be loaded, which would in turn cause the Qt libraries they
	# depend on to be loaded as well, thus resulting in two sets of Qt
	# libraries being loaded (ours from the bundle and the ones from the
	# installation) and the program misbehaving (crashing).
	FOREACH (qtComponent QtCore Qt5Gui Qt5Network Qt5Svg Qt5Widgets)
		FOREACH(plugin ${${qtComponent}_PLUGINS})
			GET_TARGET_PROPERTY(pluginPath ${plugin} LOCATION)
			GET_FILENAME_COMPONENT(pluginDir ${pluginPath} DIRECTORY)
			GET_FILENAME_COMPONENT(pluginName ${pluginPath} NAME)
			GET_FILENAME_COMPONENT(pluginDirName ${pluginDir} NAME)

			IF(pluginDirName STREQUAL "platforms" AND NOT plugin STREQUAL "Qt5::QCocoaIntegrationPlugin")
				CONTINUE()
			ENDIF()

			INSTALL(FILES ${pluginPath} DESTINATION ${MACOS_BUNDLE_PLUGINS_DIR}/${pluginDirName} COMPONENT Runtime)
			LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/PlugIns/${pluginDirName}/${pluginName}")
		ENDFOREACH()
	ENDFOREACH()

	IF(TARGET Qt5::Qml)
		FOREACH(entry QtQuick QtQuick.2 QtQml QtGraphicalEffects Qt)
			SET(_dir "${QT_HOST_PREFIX}/qml")
			FILE(GLOB_RECURSE DYLIB "${_dir}/${entry}/*.dylib")
			FOREACH(_lib ${DYLIB})
				FILE(RELATIVE_PATH _lib_dest "${_dir}" "${_lib}")
				IF(NOT _lib_dest MATCHES "XmlListModel|Particles.2|LocalStorage") # blacklist not needed stuff
					GET_FILENAME_COMPONENT(_lib_dest_dir ${_lib_dest} DIRECTORY)
					INSTALL(FILES ${_lib} DESTINATION ${MACOS_BUNDLE_RESOURCES_DIR}/qml/${_lib_dest_dir} COMPONENT Runtime)
					LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Resources/qml/${_lib_dest}")
				ENDIF()
			ENDFOREACH()
		ENDFOREACH()
	ENDIF()

	INSTALL(TARGETS AusweisApp DESTINATION ${MACOS_BUNDLE_MACOS_DIR} COMPONENT Application)

	INSTALL(TARGETS AusweisApp2AutostartHelper DESTINATION ${MACOS_BUNDLE_LOGIN_ITEMS_DIR} COMPONENT Application)

	INSTALL(CODE
		"
		${DEPENDENCY_CHECK}
		${SEARCH_ADDITIONAL_DIRS}
		file(GLOB_RECURSE QTPLUGINS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_PLUGINS_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		file(GLOB_RECURSE QtQuickPLUGINS \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_RESOURCES_DIR}/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
		INCLUDE(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${MACOS_BUNDLE_MACOS_DIR}/${PROJECT_NAME}\" \"\${QTPLUGINS};\${QtQuickPLUGINS}\" \"${TOOLCHAIN_LIB_PATH};\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)

	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Library/LoginItems/AusweisApp2AutostartHelper.app/Contents/MacOS/AusweisApp2AutostartHelper")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Library/LoginItems/AusweisApp2AutostartHelper.app")

	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtCore.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtGui.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtNetwork.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtSvg.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtWidgets.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtPrintSupport.framework") # remove if disabled in Qt
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtWebSockets.framework")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtConcurrent.framework")
	IF(TARGET Qt5::Qml)
		LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQml.framework")
		LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuick.framework")
		LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuickControls2.framework")
		LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtQuickTemplates2.framework")
	ENDIF()
	IF(TARGET Qt5::Bluetooth)
		LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "/Contents/Frameworks/QtBluetooth.framework")
	ENDIF()

	FETCH_TARGET_LOCATION(opensslCryptoName "OpenSSL::Crypto" NAME)
	FETCH_TARGET_LOCATION(opensslSslName "OpenSSL::SSL" NAME)
	IF(CMAKE_VERSION VERSION_LESS "3.13")
	    SET(OPENSSL_LIB_LOCATION "/Contents/MacOS")
	ELSE()
	    SET(OPENSSL_LIB_LOCATION "/Contents/Frameworks")
	ENDIF()
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "${OPENSSL_LIB_LOCATION}/${opensslCryptoName}")
	LIST(APPEND ADDITIONAL_BUNDLE_FILES_TO_SIGN "${OPENSSL_LIB_LOCATION}/${opensslSslName}")

	# set it to parent scope to be able to access it from Packaging.cmake
	SET(ADDITIONAL_BUNDLE_FILES_TO_SIGN ${ADDITIONAL_BUNDLE_FILES_TO_SIGN} PARENT_SCOPE)

ELSEIF(IOS)
	LIST(APPEND CMAKE_MODULE_PATH "${PACKAGING_DIR}/ios")



ELSEIF(ANDROID)
	SET(ANDROID_PACKAGE_SRC_DIR ${PROJECT_BINARY_DIR}/package-src-dir)
	SET(ANDROID_DEST libs/${CMAKE_ANDROID_ARCH_ABI})
	SET(PERMISSIONS PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
	INSTALL(TARGETS AusweisApp DESTINATION ${ANDROID_DEST} ${PERMISSIONS} COMPONENT Application)

	SET(RESOURCES_IMG_ANDROID_DIR ${RESOURCES_DIR}/images/android)
	IF(BUILD_PREVIEW)
		SET(ANDROID_LAUNCHER_ICON "npa_preview.png")
		SET(ANDROID_PACKAGE_NAME "com.governikus.ausweisapp2.dev")
	ELSEIF(IS_DEVELOPER_VERSION)
		SET(ANDROID_LAUNCHER_ICON "npa_beta.png")
		SET(ANDROID_PACKAGE_NAME "com.governikus.ausweisapp2")
	ELSE()
		SET(ANDROID_LAUNCHER_ICON "npa.png")
		SET(ANDROID_PACKAGE_NAME "com.governikus.ausweisapp2")
	ENDIF()

	IF(ANDROID_BUILD_AAR)
		SET(ANDROID_MANIFEST AndroidManifest.xml.aar.in)
		FOREACH(entry network/WifiInfo ui/aidl/AidlBinder activation/intent/AusweisApp2Service)
			SET(_java_file "${SRC_DIR}/${entry}.java")
			IF(NOT EXISTS "${_java_file}")
				MESSAGE(FATAL_ERROR "Cannot find file: ${_java_file}")
			ENDIF()
			LIST(APPEND JAVA_FILES "${_java_file}")
		ENDFOREACH()
	ELSE()
		SET(ANDROID_MANIFEST AndroidManifest.xml.apk.in)

		FOREACH(entry ldpi mdpi hdpi xhdpi xxhdpi xxxhdpi)
			INSTALL(FILES ${RESOURCES_IMG_ANDROID_DIR}/${entry}/${ANDROID_LAUNCHER_ICON} DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/res/drawable-${entry} COMPONENT Runtime RENAME npa.png)
		ENDFOREACH()

		INSTALL(FILES ${PACKAGING_DIR}/android/styles.xml DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/res/values COMPONENT Runtime)

		FILE(GLOB_RECURSE JAVA_FILES "${SRC_DIR}/*.java")
	ENDIF()

	INSTALL(FILES ${JAVA_FILES} DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/src COMPONENT Runtime)
	INSTALL(FILES ${PACKAGING_DIR}/android/IAusweisApp2Sdk.aidl DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2/ COMPONENT Runtime)
	INSTALL(FILES ${PACKAGING_DIR}/android/IAusweisApp2SdkCallback.aidl DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2/ COMPONENT Runtime)

	IF(VERSION_DVCS)
		SET(ANDROID_VERSION_NAME ${VERSION_DVCS})
	ELSE()
		SET(ANDROID_VERSION_NAME ${PROJECT_VERSION})
	ENDIF()
	CONFIGURE_FILE(${PACKAGING_DIR}/android/${ANDROID_MANIFEST} ${ANDROID_PACKAGE_SRC_DIR}/AndroidManifest.xml @ONLY)
	CONFIGURE_FILE(${PACKAGING_DIR}/android/fileprovider.xml ${ANDROID_PACKAGE_SRC_DIR}/res/xml/fileprovider.xml COPYONLY)

	SET(ANDROID_APP_BINARY "${CMAKE_INSTALL_PREFIX}/${ANDROID_DEST}/libAusweisApp2.so")
	SET(SYMBOL_FOLDER "${CMAKE_BINARY_DIR}/debug.symbols/${CMAKE_ANDROID_ARCH_ABI}")
	SET(ANDROID_APP_SYMBOLS "${SYMBOL_FOLDER}/libAusweisApp2.so")

	INSTALL(CODE
		"
		EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" -E make_directory \"${SYMBOL_FOLDER}\")
		EXECUTE_PROCESS(COMMAND \"${CMAKE_OBJCOPY}\" \"--only-keep-debug\" \"${ANDROID_APP_BINARY}\" \"${ANDROID_APP_SYMBOLS}\")
		" COMPONENT Runtime CONFIGURATIONS RelWithDebInfo)

	IF(CMAKE_COMPILER_IS_GNUCXX)
		SET(ANDROID_STL_PATH gnu-libstdc++/${CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION})
	ELSE()
		SET(ANDROID_STL_PATH llvm-libc++)
	ENDIF()

	SET(ANDROID_DEPLOYMENT_SETTINGS ${PROJECT_BINARY_DIR}/libAusweisApp2.so-deployment-settings.json CACHE INTERNAL "apk deployment" FORCE)
	CONFIGURE_FILE(${PACKAGING_DIR}/android/libAusweisApp2.so-deployment-settings.json.in ${ANDROID_DEPLOYMENT_SETTINGS} @ONLY)

	SET(TRANSLATION_DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/assets/translations)
	SET(DEFAULT_FILE_DESTINATION ${ANDROID_PACKAGE_SRC_DIR}/assets)

ELSEIF(UNIX)
	IF(BUILD_SHARED_LIBS)
		SET(CMAKE_INSTALL_RPATH "\$ORIGIN")
	ENDIF()

	SET(DEFAULT_FILE_DESTINATION bin)
	INSTALL(TARGETS AusweisApp DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Application)
	INSTALL(CODE
		"
		${DEPENDENCY_CHECK}
		${SEARCH_ADDITIONAL_DIRS}
		INCLUDE(BundleUtilities)
		FIXUP_BUNDLE(\"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}/${PROJECT_NAME}\" \"\" \"\${ADDITIONAL_DIRS}\")
		" COMPONENT Runtime)

	CONFIGURE_FILE(${PACKAGING_DIR}/linux/AusweisApp2.desktop.in ${CMAKE_CURRENT_BINARY_DIR}/AusweisApp2.desktop @ONLY)
	INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/AusweisApp2.desktop DESTINATION share/applications COMPONENT Application)
	#INSTALL(FILES ${RESOURCES_DIR}/images/AusweisApp2.svg DESTINATION share/icons/hicolor/scalable/apps COMPONENT Application)
ENDIF()




IF(LINUX OR WIN32 OR MAC)
	OPTION(SELFPACKER "Compress executable with self packer like UPX")
	IF(SELFPACKER)
		FIND_PACKAGE(SelfPackers)
		IF(SELF_PACKER_FOR_EXECUTABLE)
			MESSAGE(STATUS "Using SelfPacker: ${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS}")
		ELSE()
			MESSAGE(FATAL_ERROR "Cannot find self packer")
		ENDIF()

		INSTALL(CODE
			"
			EXECUTE_PROCESS(COMMAND
				${SELF_PACKER_FOR_EXECUTABLE} ${SELF_PACKER_FOR_EXECUTABLE_FLAGS} \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}/${PROJECT_NAME}${CMAKE_EXECUTABLE_SUFFIX}\")
			" COMPONENT Application)
	ENDIF()
ENDIF()

IF(WIN32)
	IF(SIGNTOOL_CMD)
		CONFIGURE_FILE(${CMAKE_MODULE_PATH}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)
		INSTALL(CODE
			"
			EXECUTE_PROCESS(COMMAND \"${CMAKE_COMMAND}\" -DSIGN_EXT=*.exe -P \"${CMAKE_BINARY_DIR}/SignFiles.cmake\" WORKING_DIRECTORY \"\$ENV{DESTDIR}\${CMAKE_INSTALL_PREFIX}/${DEFAULT_FILE_DESTINATION}\")
			" COMPONENT Application)
	ENDIF()
ENDIF()



IF(LINUX)
	INSTALL(FILES ${QM_FILES} DESTINATION ${TRANSLATION_DESTINATION} COMPONENT Translations)
ELSEIF(NOT ANDROID_BUILD_AAR)
	INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/translations/ DESTINATION ${TRANSLATION_DESTINATION} COMPONENT Translations)
ENDIF()

IF(NOT ANDROID_BUILD_AAR)
	# resources file
	INSTALL(FILES ${RCC} DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
ENDIF()

IF(NOT ANDROID AND NOT IOS)
	# qtlogging.ini
	INSTALL(FILES ${RESOURCES_DIR}/qtlogging.ini DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
ENDIF()

# qt qml plugins
IF(DESKTOP AND TARGET Qt5::Qml)
	FOREACH(entry QtQuick QtQuick.2 QtQml QtGraphicalEffects Qt)
		INSTALL(DIRECTORY ${QT_HOST_PREFIX}/qml/${entry} DESTINATION ${DEFAULT_FILE_DESTINATION}/qml COMPONENT Runtime PATTERN "*.dylib" EXCLUDE)
	ENDFOREACH()
ENDIF()

# secure storage file
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/config.json DESTINATION ${DEFAULT_FILE_DESTINATION} COMPONENT Runtime)
