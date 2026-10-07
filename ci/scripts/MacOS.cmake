block()
	include(Libraries)
endblock()

if("Integrated" IN_LIST NAMES)
	set(INTEGRATED ON)
endif()

if("DMG" IN_LIST NAMES OR "PKG" IN_LIST NAMES)
	set(CONFIG MinSizeRel)
	set(PRESET ci-macos-release)
else()
	set(CONFIG Debug)

	if(INTEGRATED)
		set(PRESET ci-macos-integrated)
	else()
		set(PRESET ci-macos-debug)
	endif()
endif()

step(security unlock-keychain $ENV{KEYCHAIN_CREDENTIALS} $ENV{HOME}/Library/Keychains/login.keychain-db)

step(${T_CFG} --preset ${PRESET})

if(INTEGRATED)
	step(${T_BUILD})
else()
	step(${T_TARGET} package --config ${CONFIG} -- -quiet)

	step(${CMAKE_COMMAND} -E tar cf "../../${ARTIFACT_FILENAME}.app.dSYM.zip" --format=zip AusweisApp.app.dSYM CHDIR ${T_BUILD_DIR}/src/${CONFIG})

	file(GLOB_RECURSE apps LIST_DIRECTORIES ON "${T_BUILD_DIR}/_CPack_Packages/Darwin")
	list(FILTER apps INCLUDE REGEX "\\.app$")
	if(NOT apps)
		message(FATAL_ERROR "no *.app directory for codesigning found")
	endif()
	foreach(app ${apps})
		step(codesign -vvvv --deep --strict ${app})
	endforeach()

	if(NOT "$ENV{USE_APPSTORE_PROFILE}")
		set(dragndrop ${apps})
		list(FILTER dragndrop INCLUDE REGEX "/DragNDrop/")
		if(NOT dragndrop)
			message(FATAL_ERROR "no *.app directory for Notarization found")
		endif()

		foreach(app ${dragndrop})
			step(spctl -a -vv ${app})
		endforeach()

		if(DEFINED ENV{NOTARIZATION})
			set(NOTARIZATION $ENV{NOTARIZATION})
		elseif(NOT REVIEW)
			set(NOTARIZATION ON)
		endif()
		if(NOTARIZATION)
			step(${CMAKE_COMMAND} -P ${CMAKE_DIR}/Notarization.cmake CHDIR ${T_BUILD_DIR})
		endif()

		file(GLOB FILES "${T_BUILD_DIR}/*.dmg")
		hashsum(${FILES})
	endif()

	step(codesign --remove-signature AusweisApp.app CHDIR ${T_BUILD_DIR}/src/${CONFIG})
endif()

step(${T_CTEST} -C ${CONFIG})
