#####################################################################
# The main component that will link all necessary modules and plugins
# into AusweisApp2 executable for the specific platform.
#
# This component includes a main entry point and command line
# parser only. Everything else will be included from sub-modules.
#####################################################################

add_subdirectory(external)
add_subdirectory(global)
add_subdirectory(secure_storage)
add_subdirectory(settings)

add_subdirectory(file_provider)
add_subdirectory(configuration)
add_subdirectory(network)

add_subdirectory(export)
add_subdirectory(card)
add_subdirectory(services)

add_subdirectory(ui)
add_subdirectory(activation)
add_subdirectory(remote_device)
add_subdirectory(workflows)
add_subdirectory(core)
add_subdirectory(init)

add_subdirectory(whitelist_client)

file(GLOB TRANSLATION_FILES ${RESOURCES_DIR}/translations/*.ts)

foreach(filepath ${TRANSLATION_FILES})
	get_filename_component(filename "${filepath}" NAME)
	string(REGEX MATCH "_(.*)\.ts$" _unused "${filename}")
	list(APPEND USED_TRANSLATIONS ${CMAKE_MATCH_1})
endforeach()

set_property(SOURCE ${TRANSLATION_FILES} PROPERTY OUTPUT_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/translations)
foreach(locale ${USED_TRANSLATIONS})
	file(COPY ${QT_TRANSLATIONS_DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR} FILES_MATCHING REGEX "qtbase_${locale}")
endforeach()

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

CONFIGURE_DEFAULT_FILES(${CMAKE_CURRENT_BINARY_DIR})


if(IOS)
	if(NOT INTEGRATED_SDK)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/qml)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/packaging/ios/de.lproj)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/packaging/ios/launchscreen.storyboard)
		list(APPEND IOS_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/translations)
		list(APPEND IOS_RESOURCES ${CMAKE_CURRENT_BINARY_DIR}/config.json)

		# Attention: the file names correspond to values in the Info.plist
		if(BUILD_PREVIEW)
			message(FATAL_ERROR "iOS preview not implemented")
		elseif(IS_DEVELOPER_VERSION)
			set(IOS_APPICON_PATH "beta/")
		else()
			set(IOS_APPICON_PATH "")
		endif()

		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/images/ios/appIcons/${IOS_APPICON_PATH}Images.xcassets)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/packaging/ios/Settings.bundle)
		list(APPEND IOS_RESOURCES ${RCC})
		set_source_files_properties(${RCC} PROPERTIES GENERATED TRUE)
	endif()
endif()


set(MAIN_FILE main.cpp)
if(IOS)
	if(INTEGRATED_SDK)
		add_library(AusweisApp SHARED ${MAIN_FILE} ${IOS_RESOURCES} ${QM_FILES})
	else()
		add_executable(AusweisApp MACOSX_BUNDLE ${MAIN_FILE} ${IOS_RESOURCES} ${QM_FILES})
	endif()
elseif(ANDROID OR INTEGRATED_SDK)
	add_library(AusweisApp SHARED ${MAIN_FILE} ${QM_FILES})
else()
	add_executable(AusweisApp WIN32 ${MAIN_FILE} windows.rc ${QM_FILES})
endif()

add_dependencies(AusweisApp AusweisAppRcc)

target_link_libraries(AusweisApp PRIVATE AusweisAppInit)
if(ANDROID)
	set_target_properties(AusweisApp PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${CMAKE_ANDROID_ARCH_ABI}")
else()
	set_target_properties(AusweisApp PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
endif()

if(INTEGRATED_SDK AND NOT ANDROID)
	ADD_CONFIG_JSON_RESOURCE(CONFIG_JSON_SOURCE)
	ADD_OBJECT_LIBRARY(AusweisAppConfig ${CONFIG_JSON_SOURCE})
	target_link_libraries(AusweisApp PRIVATE AusweisAppConfig)
endif()


if(APPLE)
	set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
	set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
	set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
	set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
	set(MACOSX_BUNDLE_GUI_IDENTIFIER "${BUNDLE_IDENTIFIER}")
	set(MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT_TEXT}")

	if(IOS AND INTEGRATED_SDK)
		set(MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION})
		set(MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION})
		set_target_properties(AusweisApp PROPERTIES MACOSX_FRAMEWORK_NAME ${PROJECT_NAME})
		set_target_properties(AusweisApp PROPERTIES MACOSX_FRAMEWORK_IDENTIFIER "${BUNDLE_IDENTIFIER}")
		set_target_properties(AusweisApp PROPERTIES MACOSX_FRAMEWORK_INFO_PLIST "${PACKAGING_DIR}/ios/Info.framework.plist.in")
	elseif(IOS)
		set_target_properties(AusweisApp PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${PACKAGING_DIR}/ios/Info.plist.in")
	else()
		configure_file(${PACKAGING_DIR}/macos/Info.plist.in ${CMAKE_BINARY_DIR}/Info.plist @ONLY)
	endif()
endif()


if(IOS)
	get_target_property(QT_BUILD_TYPE ${Qt}::Core IMPORTED_CONFIGURATIONS)
	if(QT_BUILD_TYPE STREQUAL "DEBUG")
		set(qt_suffix "_debug")
	endif()

	target_link_libraries(AusweisApp PRIVATE OpenSSL::Crypto OpenSSL::SSL) # remove this if iOS uses shared libraries
	target_link_libraries(AusweisApp PRIVATE -L${QT_HOST_PREFIX}/plugins/platforms)
	target_link_libraries(AusweisApp PRIVATE -L${QT_HOST_PREFIX}/plugins/imageformats)
	target_link_libraries(AusweisApp PRIVATE -L${QT_HOST_PREFIX}/lib)
	target_link_libraries(AusweisApp PRIVATE ${Qt}::Core ${Qt}::Network)
	target_link_libraries(AusweisApp PRIVATE -lqtpcre2${qt_suffix} -lqios${qt_suffix})
	target_link_libraries(AusweisApp PRIVATE -l${Qt}ClipboardSupport${qt_suffix})

	target_link_libraries(AusweisApp PRIVATE "-lc++ -lz -lm")
	target_link_libraries(AusweisApp PRIVATE ${IOS_ASSETSLIBRARY} ${IOS_UIKIT} ${IOS_COREFOUNDATION} ${IOS_OPENGLES} ${IOS_FOUNDATION} ${IOS_QUARTZCORE} ${IOS_CORETEXT} ${IOS_COREGRAPHICS} ${IOS_SECURITY} ${IOS_NETWORK} ${IOS_MOBILECORESERVICES} ${IOS_AUDIOTOOLBOX} ${IOS_IMAGEIO} ${IOS_CORENFC} ${IOS_MESSAGEUI} ${IOS_STOREKIT})
	target_link_libraries(AusweisApp PRIVATE -Wl,-e,_qt_main_wrapper)

	if(TARGET ${Qt}::Qml)
		target_link_libraries(AusweisApp PRIVATE ${Qt}::Gui ${Qt}::Svg ${Qt}::Qml ${Qt}::Quick ${Qt}::QuickControls2 ${Qt}::QuickTemplates2 ${Qt}::QuickShapes)
		target_link_libraries(AusweisApp PRIVATE -lqtlibpng${qt_suffix} -lqsvg${qt_suffix} -lqjpeg${qt_suffix} -lqtfreetype${qt_suffix})
		target_link_libraries(AusweisApp PRIVATE -l${Qt}GraphicsSupport${qt_suffix} -l${Qt}FontDatabaseSupport${qt_suffix})
		qt_import_qml_plugins(AusweisApp PATH_TO_SCAN "${RESOURCES_DIR}/qml")
	endif()

	set_target_properties(AusweisApp PROPERTIES RESOURCE "${IOS_RESOURCES}")
	set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_SKIP_INSTALL "NO")
	set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
	set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
	set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${PACKAGING_DIR}/ios/AusweisApp2.entitlements")

	if(INTEGRATED_SDK)
		GET_PUBLIC_HEADER(AusweisAppUiFunctional PUBLIC_HEADER)
		target_sources(AusweisApp PUBLIC ${PUBLIC_HEADER})
		set_target_properties(AusweisApp PROPERTIES FRAMEWORK TRUE FRAMEWORK_VERSION C MACOSX_FRAMEWORK_IDENTIFIER ${BUNDLE_IDENTIFIER})
		set_target_properties(AusweisApp PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION} PUBLIC_HEADER ${PUBLIC_HEADER})
		set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "@rpath")
	else()
		if(USE_DISTRIBUTION_PROFILE)
			set(PROVISIONING_PROFILE_SPECIFIER "iOS Release (Distribution)")
			set(EXPORT_METHOD app-store)
			set(XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Distribution: Governikus GmbH & Co. KG (G7EQCJU4BR)")
		else()
			set(PROVISIONING_PROFILE_SPECIFIER "iOS Development")
			set(EXPORT_METHOD development)
			set(XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer: Marco von der Puetten (46ZK7WV8QR)")
		endif()
		set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ${PROVISIONING_PROFILE_SPECIFIER})
		set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)")
		set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY})
	endif()

	# prevent xcode to convert multiple png files to tiff
	set_target_properties(AusweisApp PROPERTIES XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO")

	configure_file(${PACKAGING_DIR}/ios/exportOptions.plist.in ${CMAKE_BINARY_DIR}/exportOptions.plist @ONLY)
endif()

if(MAC)
	target_link_libraries(AusweisApp PRIVATE ${OSX_APPKIT})

	set(AUTOSTART_HELPER_NAME AutostartHelper)
	set(AUTOSTART_HELPER_FULL_NAME ${PROJECT_NAME}${AUTOSTART_HELPER_NAME})
	set(AUTOSTART_HELPER_SOURCES autostart_helper/main.mm)
	set(AUTOSTART_HELPER_BUNDLE_ID "com.governikus.AusweisApp2.${AUTOSTART_HELPER_NAME}")
	add_executable(${AUTOSTART_HELPER_FULL_NAME} MACOSX_BUNDLE ${AUTOSTART_HELPER_SOURCES})
	target_link_libraries(${AUTOSTART_HELPER_FULL_NAME} ${OSX_APPKIT})
	configure_file(${PACKAGING_DIR}/macos/autostart_helper/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/AutostartHelperInfo.plist @ONLY)
	set_target_properties(${AUTOSTART_HELPER_FULL_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/AutostartHelperInfo.plist)

endif()

if(ANDROID)
	target_link_libraries(AusweisApp PRIVATE ${Qt}::AndroidExtras)
	target_link_libraries(AusweisApp PRIVATE AusweisAppCardNfc)
	if(NOT INTEGRATED_SDK)
		target_link_libraries(AusweisApp PRIVATE AusweisAppActivationIntent)
	endif()
endif()

if(IOS)
	target_link_libraries(AusweisApp PRIVATE AusweisAppCardIos)
	if(NOT INTEGRATED_SDK)
		target_link_libraries(AusweisApp PRIVATE AusweisAppActivationCustomScheme)
	endif()
endif()

if(WIN32)
	target_link_libraries(AusweisApp PRIVATE ${WIN_DEFAULT_LIBS})
endif()

if(TARGET ${Qt}::Qml)
	target_link_libraries(AusweisApp PRIVATE AusweisAppUiQml)
endif()

if(ANDROID)
	target_link_libraries(AusweisApp PRIVATE AusweisAppUiAidl)
else()
	target_link_libraries(AusweisApp PRIVATE debug AusweisAppUiAidl)
endif()

if(DESKTOP)
	target_link_libraries(AusweisApp PRIVATE AusweisAppCardPcsc AusweisAppCardDrivers AusweisAppActivationWebservice)
endif()

if(INTEGRATED_SDK AND NOT ANDROID)
	target_link_libraries(AusweisApp PRIVATE AusweisAppUiFunctional)
endif()

if(NOT ANDROID AND NOT IOS AND NOT INTEGRATED_SDK)
	target_link_libraries(AusweisApp PRIVATE AusweisAppUiWebsocket)
endif()


include(Install)
include(FeatureSummary)
FEATURE_SUMMARY(WHAT ALL)
