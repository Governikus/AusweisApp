if(NAME MATCHES "_APK_")
	set(APK ON)
	set(PRESET ci-android-apk)
elseif(NAME MATCHES "_AAR_")
	set(AAR ON)
	set(PRESET ci-android-aar)
else()
	step(${CMAKE_COMMAND} -DDIST_DIR=${T_DIST_DIR} -P ${CMAKE_DIR}/Merge.cmake)

	if(RELEASE)
		set(files *-sources.jar *.aar *.pom)
		foreach(file ${files})
			file(GLOB file "${T_DIST_DIR}/${file}")
			step(gpg --batch --passphrase $ENV{GPG_PSW} --pinentry-mode loopback -a --detach-sig -u $ENV{GPG_ID} ${file})
		endforeach()
	endif()

	if(NOT REVIEW)
		step(${CMAKE_COMMAND} -DCMD=DEPLOY_NEXUS -DPUBLISH=$ENV{PUBLISH} -P ${CMAKE_DIR}/cmd.cmake CHDIR ${T_DIST_DIR})
	endif()
	return()
endif()

if(REVIEW)
	set(PRESET ${PRESET}-review)
endif()

step(${T_CFG} --preset ${PRESET})
step(${T_BUILD})

if(APK)
	step(${T_TARGET} apk)
	step(${T_TARGET} verify.signature)
	step(${T_TARGET} dump.apk)
	if(RELEASE)
		step(${T_TARGET} aab ENV
			QT_ANDROID_KEYSTORE_PATH=$ENV{APK_SIGN_KEYSTORE_UPLOAD}
			QT_ANDROID_KEYSTORE_ALIAS=$ENV{APK_SIGN_KEYSTORE_ALIAS_UPLOAD}
			QT_ANDROID_KEYSTORE_STORE_PASS=$ENV{APK_SIGN_KEYSTORE_PSW_UPLOAD})
	else()
		step(${T_TARGET} aab)
	endif()
	file(GLOB FILES "${T_DIST_DIR}/*.apk" "${T_DIST_DIR}/*.aab")
elseif(AAR)
	step(${T_TARGET} aar)
	file(GLOB FILES "${T_DIST_DIR}/*.aar")
endif()

hashsum(${FILES})

if(NOT RELEASE)
	step(${T_CTEST})
endif()
