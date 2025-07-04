block()
	include(Libraries)
endblock()

if("Integrated" IN_LIST NAMES)
	set(INTEGRATED ON)
endif()

if("DMG" IN_LIST NAMES OR "PKG" IN_LIST NAMES OR RELEASE)
	set(PACKAGES ON)
endif()

if(PACKAGES)
	set(PRESET ci-macos-release)
elseif(INTEGRATED)
	set(PRESET ci-macos-integrated)
else()
	set(PRESET ci-macos)
	set(CTEST_CFG -C Debug)
endif()

step(security unlock-keychain $ENV{KEYCHAIN_CREDENTIALS} $ENV{HOME}/Library/Keychains/login.keychain-db)

step(${T_CFG} --preset ${PRESET})

if(PACKAGES)
	step(${T_TARGET} package --config MinSizeRel)
	step(${CMAKE_COMMAND} -E tar cf ../../AusweisApp.app.dSYM.zip --format=zip AusweisApp.app.dSYM CHDIR ${T_BUILD_DIR}/src/MinSizeRel)

	file(GLOB_RECURSE apps LIST_DIRECTORIES ON "${T_BUILD_DIR}/_CPack_Packages/Darwin")
	list(FILTER apps INCLUDE REGEX "\\.app$")
	set(dragndrop ${apps})
	list(FILTER dragndrop INCLUDE REGEX "/DragNDrop/")
	if(NOT dragndrop OR NOT apps)
		message(FATAL_ERROR "no *.app directory found")
	endif()
	foreach(app ${apps})
		step(codesign -vvvv ${app})
	endforeach()
	foreach(app ${dragndrop})
		step(spctl -a -vv ${app})
	endforeach()

	if(NOT REVIEW)
		step(${CMAKE_COMMAND} -P ${CMAKE_DIR}/Notarization.cmake CHDIR ${T_BUILD_DIR})
	endif()

	file(GLOB FILES "${T_BUILD_DIR}/*.dmg")
	hashsum(${FILES})
else()
	step(${T_BUILD})
	step(${T_CTEST} ${CTEST_CFG} ENV QT_PLUGIN_PATH=${WORKSPACE}/libs/dist/plugins QML2_IMPORT_PATH=${WORKSPACE}/libs/dist/qml)
endif()

if(RELEASE AND DEFINED ENV{USE_DISTRIBUTION_PROFILE})
	if($ENV{USE_DISTRIBUTION_PROFILE})
		file(GLOB pkgfile "${T_BUILD_DIR}/*.pkg")
		step(xcrun altool -t osx --upload-app -u $ENV{APPSTORE_USER} -p @env:APPSTORE_PSW -f ${pkgfile})
	endif()
endif()
