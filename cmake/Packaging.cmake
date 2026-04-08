# CPack
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Configuration

if(ANDROID AND INTEGRATED_SDK)
	string(TOLOWER "${ARTIFACT_FILENAME}" ARTIFACT_FILENAME)
elseif(IOS AND INTEGRATED_SDK)
	string(REPLACE "${PROJECT_NAME}" "${PROJECT_NAME}2" ARTIFACT_FILENAME "${ARTIFACT_FILENAME}")
endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
set(CPACK_PACKAGE_VENDOR "${VENDOR}")
set(CPACK_PACKAGE_CONTACT "support@ausweisapp.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME}")
set(CPACK_PACKAGE_FILE_NAME ${ARTIFACT_FILENAME})
if(USE_DISTRIBUTION_PROFILE)
	set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_FILE_NAME}-appstore)
endif()
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${PROJECT_NAME})
if(NOT CONTAINER_SDK)
	set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.rst")
	set(CPACK_RESOURCE_FILE_LICENSE "${LICENSE_FILE}")
	set(CPACK_DMG_SLA_USE_RESOURCE_FILE_LICENSE ON)
endif()


find_program(CONV uconv)
if(NOT CONV)
	find_program(CONV iconv)
endif()
find_program(ENSCRIPT enscript)
find_program(GS gs)

set(LICENSE_FILE_DIST ${ARTIFACT_FILENAME}-Lizenz)
set(LICENSE_FILE_TXT ${LICENSE_FILE_DIST}.txt)
add_custom_command(OUTPUT ${LICENSE_FILE_TXT} COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CPACK_RESOURCE_FILE_LICENSE} ${LICENSE_FILE_TXT})
add_custom_target(license DEPENDS ${LICENSE_FILE_TXT})
if(CONV AND ENSCRIPT AND GS)
	set(LICENSE_FILE_PDF ${LICENSE_FILE_DIST}.pdf)

	set(CONV_CMD ${CONV} -f utf-8 -t iso-8859-1 --output license.tmpfile.conv ${CPACK_RESOURCE_FILE_LICENSE})
	set(ENSCRIPT_CMD ${ENSCRIPT} -q -X 88591 -B --word-wrap -t ${PROJECT_NAME} --output license.tmpfile.ps license.tmpfile.conv)
	set(GS_CMD ${GS} -q -sPAPERSIZE=a4 -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=${LICENSE_FILE_PDF} license.tmpfile.ps)

	add_custom_command(OUTPUT ${LICENSE_FILE_PDF} COMMAND ${CONV_CMD} COMMAND ${ENSCRIPT_CMD} COMMAND ${GS_CMD})
	add_custom_target(license.pdf DEPENDS ${LICENSE_FILE_PDF})
	add_dependencies(license license.pdf)
endif()

if(APPLE AND NOT IOS)
	if(NOT CONV)
		message(FATAL_ERROR "Cannot find 'iconv' to convert LICENSE.txt")
	endif()

	execute_process(COMMAND ${CONV} -f UTF-8 -t MAC ${CPACK_RESOURCE_FILE_LICENSE} OUTPUT_FILE "${PROJECT_BINARY_DIR}/LICENSE.txt")
	set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_BINARY_DIR}/LICENSE.txt")
endif()

if(NOT CMAKE_BUILD_TYPE STREQUAL "DEBUG" AND NOT CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
	set(CPACK_STRIP_FILES TRUE)
endif()

set(CPACK_SOURCE_GENERATOR TGZ)
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${ARTIFACT_FILENAME} CACHE INTERNAL "tarball basename")

set(CPACK_SOURCE_IGNORE_FILES "\\\\.hgignore" "\\\\.hgtags" "/\\\\.hg/" "\\\\.hgchurn")
list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.gitignore" "/\\\\.git/")
list(APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_CURRENT_BINARY_DIR}")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeCache.txt")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeFiles")
list(APPEND CPACK_SOURCE_IGNORE_FILES "CMakeLists\\\\.txt\\\\.user")
list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.project")
list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.cproject")
list(APPEND CPACK_SOURCE_IGNORE_FILES "\\\\.reviewboardrc")
list(APPEND CPACK_SOURCE_IGNORE_FILES ".*\\\\.internal\\\\.cmake$")
list(APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_SOURCE_DIR}/utils/")

set(CPACK_MONOLITHIC_INSTALL true)


if(WIN32)
	set(WIX_TOOLSET_MIN_VERSION 6)
	function(wixtoolset_validator validator_result binary)
		execute_process(COMMAND ${binary} --version OUTPUT_VARIABLE WIX_TOOLSET_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

		message(STATUS "WiX Toolset Version: ${WIX_TOOLSET_VERSION}")
		if(WIX_TOOLSET_VERSION VERSION_LESS WIX_TOOLSET_MIN_VERSION)
			message(STATUS "WiX Toolset Version ${WIX_TOOLSET_MIN_VERSION} is required to build target package.")
			set(${validator_result} FALSE PARENT_SCOPE)
		endif()
	endfunction()

	find_program(WIX_TOOLSET wix VALIDATOR wixtoolset_validator)
	if(WIX_TOOLSET)
		set(CPACK_GENERATOR WIX)
		set(CPACK_WIX_VERSION 4) # Package using WiX .NET Tools (v4 and above)
		set(CPACK_WIX_UPGRADE_GUID 4EE0E467-EAB7-483E-AB45-87BD1DB6B037)
		set(CPACK_WIX_PRODUCT_ICON ${RESOURCES_DIR}/images/desktop/npa.ico)
		set(CPACK_WIX_CULTURES de-DE en-US)
		# disable above line, enable beneath line to build MSI for english
		# set(CPACK_WIX_CULTURES en-US)
		set(CPACK_WIX_TEMPLATE ${PACKAGING_DIR}/win/WIX.template.in)
		set(CPACK_WIX_EXTRA_SOURCES ${PACKAGING_DIR}/win/executable.wxs ${PACKAGING_DIR}/win/install_settings.wxs ${PACKAGING_DIR}/win/runtime_settings.wxs ${PACKAGING_DIR}/win/gui.wxs)
		set(CPACK_WIX_UI_BANNER ${RESOURCES_DIR}/images/wix_banner.jpg)
		set(CPACK_WIX_UI_DIALOG ${RESOURCES_DIR}/images/wix_dialog.jpg)
		set(CPACK_WIX_EXTENSIONS WixToolset.Util.wixext WixToolset.Firewall.wixext)
		set(CPACK_WIX_BUILD_EXTRA_FLAGS -loc ${PACKAGING_DIR}/win/WIX.Texts.de-DE.wxl -loc ${PACKAGING_DIR}/win/WIX.Texts.en-US.wxl)
		if(CMAKE_VERSION VERSION_LESS "4.1")
			set(CPACK_WIX_BUILD_EXTRA_FLAGS -i ${CMAKE_BINARY_DIR}/_CPack_Packages/win64/WIX ${CPACK_WIX_BUILD_EXTRA_FLAGS})
		endif()

		configure_file(${CMAKE_DIR}/PrepareExecutable.cmake.in ${CMAKE_BINARY_DIR}/PrepareExecutable.cmake @ONLY)
		set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_BINARY_DIR}/PrepareExecutable.cmake")

		if(SIGNTOOL_CMD)
			message(STATUS "MSI can be signed with 'cmake --build . --target package.sign'")
			set(MSI ${PROJECT_BINARY_DIR}/${CPACK_PACKAGE_FILE_NAME}.msi)
			add_custom_target(package.sign COMMAND ${SIGNTOOL_CMD} ${SIGNTOOL_PARAMS} ${MSI}
											COMMAND ${SIGNTOOL_CMD} verify /v /pa ${MSI})
		endif()

	else()
		unset(CPACK_GENERATOR)
	endif()

elseif(IOS)
	configure_file("${CMAKE_DIR}/iOS.bundles.cmake.in" "${CMAKE_BINARY_DIR}/iOS.bundles.cmake" @ONLY)
	if(INTEGRATED_SDK)
		set(Bundle_Target zip)
		list(APPEND Bundle_Suffix ${CMAKE_OSX_SYSROOT} ${CMAKE_OSX_ARCHITECTURES})
		list(JOIN Bundle_Suffix "_" Bundle_Suffix)
	else()
		set(Bundle_Target ipa)
	endif()
	add_custom_target(${Bundle_Target} COMMAND ${CMAKE_COMMAND} -DINTEGRATED_SDK=${INTEGRATED_SDK} -DSUFFIX=${Bundle_Suffix} -DCONFIG=$<CONFIGURATION> -DIOS=${IOS} -P ${CMAKE_BINARY_DIR}/iOS.bundles.cmake)

elseif(APPLE)
	set(CPACK_GENERATOR External DragNDrop)

	set(CPACK_EXTERNAL_ENABLE_STAGING ON)
	set(CPACK_EXTERNAL_PACKAGE_SCRIPT ${CMAKE_DIR}/macOS.pkg.cmake)

	set(MACOS_PACKAGING_DIR ${PACKAGING_DIR}/macos)
	set(CPACK_DMG_VOLUME_NAME ${CPACK_PACKAGE_NAME})
	set(CPACK_DMG_FORMAT UDBZ)
	set(CPACK_DMG_BACKGROUND_IMAGE ${RESOURCES_DIR}/images/macos/dmg_background.png)
	set(CPACK_DMG_SLA_DIR ${MACOS_PACKAGING_DIR}/sla)
	set(CPACK_DMG_SLA_LANGUAGES English German)

	# Temporarily toggle the following lines to generate a new DS_Store, preferably with CI.
	# set(CPACK_DMG_DS_STORE_SETUP_SCRIPT ${MACOS_PACKAGING_DIR}/prepare-ds_store.applescript)
	set(CPACK_DMG_DS_STORE ${MACOS_PACKAGING_DIR}/DS_Store)

elseif(ANDROID)
	include(Packaging.android)

elseif(UNIX)
	set(CPACK_GENERATOR STGZ)
endif()

if(NOT "${CPACK_GENERATOR}" STREQUAL "")
	include(CPack)
endif()
