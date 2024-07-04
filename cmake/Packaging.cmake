# CPack
# https://gitlab.kitware.com/cmake/community/-/wikis/doc/cpack/Configuration

set(PACKAGE_VERSION ${PROJECT_VERSION})

if(ANDROID AND NOT INTEGRATED_SDK)
	set(PACKAGE_VERSION ${PACKAGE_VERSION}-${CMAKE_ANDROID_ARCH_ABI})
endif()

if(DEFINED dvcs_distance)
	set(PACKAGE_VERSION ${PACKAGE_VERSION}+${dvcs_distance})
endif()

if(DEFINED dvcs_branch)
	set(PACKAGE_VERSION ${PACKAGE_VERSION}-${dvcs_branch})
endif()

if(DEFINED dvcs_phase)
	set(PACKAGE_VERSION ${PACKAGE_VERSION}-${dvcs_phase})
endif()

if(DEFINED dvcs_revision)
	set(PACKAGE_VERSION ${PACKAGE_VERSION}-${dvcs_revision})
endif()

if(ANDROID AND INTEGRATED_SDK)
	string(TOLOWER "${PROJECT_NAME}-" AAR_PROJECT_NAME)
	string(REGEX REPLACE "[0-9]*-" "-" AAR_PROJECT_NAME "${AAR_PROJECT_NAME}")
	set(FILENAME ${AAR_PROJECT_NAME}${PACKAGE_VERSION})
elseif(IOS AND INTEGRATED_SDK)
	set(FILENAME ${PROJECT_NAME}2-${PACKAGE_VERSION})
else()
	set(FILENAME ${PROJECT_NAME}-${PACKAGE_VERSION})
endif()

set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_TWEAK ${PROJECT_VERSION_TWEAK})
set(CPACK_PACKAGE_VENDOR "${VENDOR}")
set(CPACK_PACKAGE_CONTACT "support@ausweisapp.de")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "AusweisApp")
set(CPACK_PACKAGE_FILE_NAME ${FILENAME})
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

if(CONV AND ENSCRIPT AND GS)
	set(LICENSE_FILE ${FILENAME}-Lizenz.pdf)

	set(CONV_CMD ${CONV} -f utf-8 -t iso-8859-1 --output license.tmpfile.conv ${CPACK_RESOURCE_FILE_LICENSE})
	set(ENSCRIPT_CMD ${ENSCRIPT} -q -X 88591 -B --word-wrap -t ${PROJECT_NAME} --output license.tmpfile.ps license.tmpfile.conv)
	set(GS_CMD ${GS} -q -sPAPERSIZE=a4 -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=${LICENSE_FILE} license.tmpfile.ps)

	add_custom_command(OUTPUT ${LICENSE_FILE} COMMAND ${CONV_CMD} COMMAND ${ENSCRIPT_CMD} COMMAND ${GS_CMD})
	add_custom_target(license DEPENDS ${LICENSE_FILE})
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
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${FILENAME} CACHE INTERNAL "tarball basename")

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
	set(CPACK_GENERATOR WIX)
	set(CPACK_WIX_UPGRADE_GUID 4EE0E467-EAB7-483E-AB45-87BD1DB6B037)
	set(CPACK_WIX_PRODUCT_ICON ${RESOURCES_DIR}/images/desktop/npa.ico)
	set(CPACK_WIX_CULTURES de-DE en-US)
	# disable above line, enable beneath line to build MSI for english
	# set(CPACK_WIX_CULTURES en-US)
	set(CPACK_WIX_TEMPLATE ${PACKAGING_DIR}/win/WIX.template.in)
	set(CPACK_WIX_EXTRA_SOURCES ${PACKAGING_DIR}/win/executable.wxs ${PACKAGING_DIR}/win/install_settings.wxs ${PACKAGING_DIR}/win/runtime_settings.wxs ${PACKAGING_DIR}/win/gui.wxs)
	set(CPACK_WIX_UI_BANNER ${RESOURCES_DIR}/images/wix_banner.jpg)
	set(CPACK_WIX_UI_DIALOG ${RESOURCES_DIR}/images/wix_dialog.jpg)
	set(CPACK_WIX_EXTENSIONS WixUtilExtension WixFirewallExtension)
	set(CPACK_WIX_LIGHT_EXTRA_FLAGS -loc ${PACKAGING_DIR}/win/WIX.Texts.de-DE.wxl -loc ${PACKAGING_DIR}/win/WIX.Texts.en-US.wxl)
	# suppress errors related to the shortcuts (HKCU/HKLM)
	set(CPACK_WIX_LIGHT_EXTRA_FLAGS -sice:ICE38 -sice:ICE43 -sice:ICE57 ${CPACK_WIX_LIGHT_EXTRA_FLAGS})

	configure_file(${CMAKE_DIR}/PrepareProxy.cmake.in ${CMAKE_BINARY_DIR}/PrepareProxy.cmake @ONLY)
	set(CPACK_PRE_BUILD_SCRIPTS "${CMAKE_BINARY_DIR}/PrepareProxy.cmake")

	if(SIGNTOOL_CMD)
		message(STATUS "MSI can be signed with 'make package.sign'")
		set(MSI ${PROJECT_BINARY_DIR}/${CPACK_PACKAGE_FILE_NAME}.msi)
		add_custom_target(package.sign COMMAND ${SIGNTOOL_CMD} ${SIGNTOOL_PARAMS} ${MSI}
										COMMAND ${SIGNTOOL_CMD} verify /v /pa ${MSI})
	endif()

elseif(IOS)
	configure_file("${CMAKE_DIR}/iOS.bundles.cmake.in" "${CMAKE_BINARY_DIR}/iOS.bundles.cmake" @ONLY)
	if(INTEGRATED_SDK)
		set(Bundle_Target zip)
	else()
		set(Bundle_Target ipa)
	endif()
	add_custom_target(${Bundle_Target} COMMAND ${CMAKE_COMMAND} -DINTEGRATED_SDK=${INTEGRATED_SDK} -DCONFIG=$<CONFIGURATION> -DIOS=${IOS} -P ${CMAKE_BINARY_DIR}/iOS.bundles.cmake)

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


include(CPack)
