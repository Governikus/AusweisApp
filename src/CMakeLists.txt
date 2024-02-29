#####################################################################
# The main component that will link all necessary modules and plugins
# into AusweisApp executable for the specific platform.
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

add_subdirectory(card)
add_subdirectory(services)

add_subdirectory(diagnosis)
add_subdirectory(ifd)
add_subdirectory(workflows)
add_subdirectory(ui)
add_subdirectory(core)
add_subdirectory(init)
add_subdirectory(whitelist_client)


if(IOS)
	if(NOT INTEGRATED_SDK)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/qml)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/packaging/ios/de.lproj)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/packaging/ios/launchscreen.storyboard)

		# Attention: the file names correspond to values in the Info.plist
		if(BUILD_PREVIEW)
			message(FATAL_ERROR "iOS preview not implemented")
		elseif(IS_BETA_VERSION)
			set(IOS_APPICON_PATH "beta/")
		else()
			set(IOS_APPICON_PATH "")
		endif()

		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/images/ios/appIcons/${IOS_APPICON_PATH}Images.xcassets)
		list(APPEND IOS_RESOURCES ${RESOURCES_DIR}/packaging/ios/Settings.bundle)
		list(APPEND IOS_RESOURCES ${RCC})
		set_source_files_properties(${RCC} PROPERTIES GENERATED TRUE)
	endif()
elseif(MAC)
	if(IS_BETA_VERSION)
		set(MACOS_APPICON_PATH "beta/")
	else()
		set(MACOS_APPICON_PATH "")
	endif()

	list(APPEND MAC_RESOURCES "${RESOURCES_DIR}/images/macos/${MACOS_APPICON_PATH}${PROJECT_NAME}.icns")
	list(APPEND MAC_RESOURCES ${RCC})
	set_source_files_properties(${RCC} PROPERTIES GENERATED TRUE)
endif()

if(APPLE)
	set(CMAKE_XCODE_GENERATE_SCHEME ON)
	set(CMAKE_XCODE_SCHEME_DISABLE_MAIN_THREAD_CHECKER ON)
endif()

set(MAIN_FILE main.cpp)
if(IOS)
	if(INTEGRATED_SDK)
		add_library(AusweisAppBinary SHARED ${MAIN_FILE} ${IOS_RESOURCES})
	else()
		add_executable(AusweisAppBinary MACOSX_BUNDLE ${MAIN_FILE} ${IOS_RESOURCES})
	endif()
elseif(ANDROID OR (INTEGRATED_SDK AND NOT CONTAINER_SDK))
	add_library(AusweisAppBinary SHARED ${MAIN_FILE})
elseif(MAC)
	add_executable(AusweisAppBinary MACOSX_BUNDLE ${MAIN_FILE} ${MAC_RESOURCES})
else()
	add_executable(AusweisAppBinary WIN32 ${MAIN_FILE} windows.rc)
endif()

if(TARGET AusweisAppRcc)
	add_dependencies(AusweisAppBinary AusweisAppRcc)
endif()

target_link_libraries(AusweisAppBinary PRIVATE AusweisAppInit)
if(ANDROID)
	set_target_properties(AusweisAppBinary PROPERTIES OUTPUT_NAME "${PROJECT_NAME}_${CMAKE_ANDROID_ARCH_ABI}")
elseif(IOS AND INTEGRATED_SDK)
	set_target_properties(AusweisAppBinary PROPERTIES OUTPUT_NAME "${PROJECT_NAME}2")
else()
	set_target_properties(AusweisAppBinary PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
endif()

if(TARGET AusweisAppConfig)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppConfig)
endif()

ADD_SHADERS_TO_TARGET(AusweisAppBinary)

if(APPLE)
	set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION})
	set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
	set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
	set(MACOSX_BUNDLE_GUI_IDENTIFIER "${BUNDLE_IDENTIFIER}")
	set(MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT_TEXT}")

	if(IOS AND INTEGRATED_SDK)
		set(MACOSX_FRAMEWORK_BUNDLE_VERSION ${PROJECT_VERSION})
		set(MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${PROJECT_VERSION})
		set_target_properties(AusweisAppBinary PROPERTIES MACOSX_FRAMEWORK_NAME ${PROJECT_NAME})
		set_target_properties(AusweisAppBinary PROPERTIES MACOSX_FRAMEWORK_IDENTIFIER "${BUNDLE_IDENTIFIER}")
		set_target_properties(AusweisAppBinary PROPERTIES MACOSX_FRAMEWORK_INFO_PLIST "${PACKAGING_DIR}/ios/Info.framework.plist.in")
	elseif(IOS)
		set_target_properties(AusweisAppBinary PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${PACKAGING_DIR}/ios/Info.plist.in")
	elseif(MAC AND NOT INTEGRATED_SDK)
		set_target_properties(AusweisAppBinary PROPERTIES RESOURCE "${MAC_RESOURCES}")
		set_target_properties(AusweisAppBinary PROPERTIES MACOSX_BUNDLE_ICON_FILE "${PROJECT_NAME}.icns")
		set_target_properties(AusweisAppBinary PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${PACKAGING_DIR}/macos/Info.plist.in")
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")
	endif()
endif()


if(IOS)
	get_target_property(QT_BUILD_TYPE ${Qt}::Core IMPORTED_CONFIGURATIONS)
	if(QT_BUILD_TYPE STREQUAL "DEBUG")
		set(qt_suffix "_debug")
	endif()

	target_link_libraries(AusweisAppBinary PRIVATE OpenSSL::Crypto OpenSSL::SSL) # remove this if iOS uses shared libraries
	target_link_libraries(AusweisAppBinary PRIVATE -L${QT_INSTALL_ARCHDATA}/plugins/platforms)
	target_link_libraries(AusweisAppBinary PRIVATE -L${QT_INSTALL_ARCHDATA}/plugins/imageformats)
	target_link_libraries(AusweisAppBinary PRIVATE -L${QT_INSTALL_ARCHDATA}/lib)
	target_link_libraries(AusweisAppBinary PRIVATE ${Qt}::Core ${Qt}::Network)
	target_link_libraries(AusweisAppBinary PRIVATE -lqios${qt_suffix})

	target_link_libraries(AusweisAppBinary PRIVATE "-lc++ -lz -lm")
	target_link_libraries(AusweisAppBinary PRIVATE ${IOS_ASSETSLIBRARY} ${IOS_UIKIT} ${IOS_COREFOUNDATION} ${IOS_OPENGLES} ${IOS_FOUNDATION} ${IOS_QUARTZCORE} ${IOS_CORETEXT} ${IOS_COREGRAPHICS} ${IOS_SECURITY} ${IOS_NETWORK} ${IOS_MOBILECORESERVICES} ${IOS_AUDIOTOOLBOX} ${IOS_IMAGEIO} ${IOS_CORENFC} ${IOS_MESSAGEUI} ${IOS_STOREKIT})
	target_link_libraries(AusweisAppBinary PRIVATE -Wl,-e,_qt_main_wrapper)

	if(TARGET ${Qt}::Qml)
		target_link_libraries(AusweisAppBinary PRIVATE ${Qt}::Gui ${Qt}::Svg ${Qt}::Qml ${Qt}::Quick ${Qt}::QuickControls2 ${Qt}::QuickTemplates2)
		target_link_libraries(AusweisAppBinary PRIVATE -lqsvg${qt_suffix} -lqjpeg${qt_suffix})
		target_link_libraries(AusweisAppBinary PRIVATE AusweisAppQmlPlugins)
	endif()

	set_target_properties(AusweisAppBinary PROPERTIES RESOURCE "${IOS_RESOURCES}")
	set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_SKIP_INSTALL "NO")
	set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon")
	set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
	set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${PACKAGING_DIR}/ios/AusweisApp.entitlements")

	if(INTEGRATED_SDK)
		GET_PUBLIC_HEADER(AusweisAppUiFunctional PUBLIC_HEADER)
		target_sources(AusweisAppBinary PUBLIC ${PUBLIC_HEADER})
		set_target_properties(AusweisAppBinary PROPERTIES FRAMEWORK TRUE FRAMEWORK_VERSION C MACOSX_FRAMEWORK_IDENTIFIER ${BUNDLE_IDENTIFIER})
		set_target_properties(AusweisAppBinary PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION ${PROJECT_VERSION} PUBLIC_HEADER ${PUBLIC_HEADER})
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "@rpath")
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
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER ${PROVISIONING_PROFILE_SPECIFIER})
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH "$(LOCAL_APPS_DIR)")
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY})
	endif()

	# prevent xcode to convert multiple png files to tiff
	set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_COMBINE_HIDPI_IMAGES "NO")

	configure_file(${PACKAGING_DIR}/ios/exportOptions.plist.in ${CMAKE_BINARY_DIR}/exportOptions.plist @ONLY)
endif()

if(MAC)
	target_link_libraries(AusweisAppBinary PRIVATE ${OSX_APPKIT})

	set(AUTOSTART_HELPER_NAME AutostartHelper)
	set(AUTOSTART_HELPER_FULL_NAME ${PROJECT_NAME}${AUTOSTART_HELPER_NAME})
	set(AUTOSTART_HELPER_SOURCES autostart_helper/main.mm)
	set(AUTOSTART_HELPER_BUNDLE_ID "com.governikus.AusweisApp2.${AUTOSTART_HELPER_NAME}")
	add_executable(${AUTOSTART_HELPER_FULL_NAME} MACOSX_BUNDLE ${AUTOSTART_HELPER_SOURCES})
	target_link_libraries(${AUTOSTART_HELPER_FULL_NAME} ${OSX_APPKIT})
	set_target_properties(${AUTOSTART_HELPER_FULL_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${PACKAGING_DIR}/macos/autostart_helper/Info.plist.in)

	if(USE_DISTRIBUTION_PROFILE)
		configure_file(${PACKAGING_DIR}/macos/entitlements.distribution.plist.in ${CMAKE_BINARY_DIR}/entitlements.plist @ONLY)
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_ENTITLEMENTS "${CMAKE_BINARY_DIR}/entitlements.plist")

		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "G7EQCJU4BR")
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_PROVISIONING_PROFILE_SPECIFIER "macOS Release (Distribution)")
		set_target_properties(AusweisAppBinary PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "3rd Party Mac Developer Application: Governikus GmbH & Co. KG (G7EQCJU4BR)")
	else()
		configure_file(${PACKAGING_DIR}/macos/entitlements.development.plist ${CMAKE_BINARY_DIR}/entitlements.plist @COPYONLY)
	endif()
endif()

if(WIN32)
	target_link_libraries(AusweisAppBinary PRIVATE ${WIN_DEFAULT_LIBS})
endif()

#####################################################################
# ui
#####################################################################

if(ANDROID)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiAidl)
	if(NOT INTEGRATED_SDK AND USE_SMARTEID)
		target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiLocalIfd)
	endif()
else()
	target_link_libraries(AusweisAppBinary PRIVATE debug AusweisAppUiAidl)
endif()

if(TARGET ${Qt}::Qml)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiQml)
endif()

if(INTEGRATED_SDK AND NOT ANDROID AND NOT CONTAINER_SDK)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiFunctional)
endif()

if((NOT ANDROID AND NOT IOS AND NOT INTEGRATED_SDK) OR CONTAINER_SDK)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiWebsocket)
endif()

if(CONTAINER_SDK OR DESKTOP)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiAutomatic)
endif()

if(DESKTOP)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiWebservice)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiProxy)
endif()

if((ANDROID OR IOS) AND NOT INTEGRATED_SDK)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppUiScheme)
endif()

#####################################################################
# card
#####################################################################

if(ANDROID)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppCardNfc)
	if(INTEGRATED_SDK AND USE_SMARTEID)
		target_link_libraries(AusweisAppBinary PRIVATE AusweisAppIfdLocal)
	endif()
endif()

if(IOS)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppCardNfc)
endif()

if(TARGET AusweisAppCardSmart)
	if(((ANDROID OR IOS) AND NOT INTEGRATED_SDK) OR CONTAINER_SDK)
		target_link_libraries(AusweisAppBinary PRIVATE AusweisAppCardSmart)
	else()
		target_link_libraries(AusweisAppBinary PRIVATE debug AusweisAppCardSmart)
	endif()
endif()

if(DESKTOP)
	target_link_libraries(AusweisAppBinary PRIVATE AusweisAppCardPcsc AusweisAppCardDrivers)
endif()

target_link_libraries(AusweisAppBinary PRIVATE AusweisAppCardSimulator)

#####################################################################

include(Install)
