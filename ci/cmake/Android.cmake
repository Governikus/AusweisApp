if("APK" IN_LIST NAMES)
	set(APK ON)
	set(PRESET ci-android-apk)
elseif(NAME MATCHES "_AAR_" OR NAME MATCHES "_AAR:")
	set(AAR ON)
	set(PRESET ci-android-aar)
else()
	step(${CMAKE_COMMAND} -DDIST_DIR=${T_DIST_DIR} -P ${CMAKE_DIR}/Merge.cmake)

	block()
		include(Files)
	endblock()
	prepare_gpg(GPG_CMD)

	set(files *-sources.jar *.aar *.pom)
	foreach(file ${files})
		file(GLOB file "${T_DIST_DIR}/${file}")
		step(${GPG_CMD} -a --detach-sig -u $ENV{GPG_ID} ${file})
	endforeach()

	if(NOT REVIEW)
		step(${CMAKE_COMMAND} -DCMD=DEPLOY_NEXUS -DPUBLISH=$ENV{PUBLISH} -P ${CMAKE_DIR}/cmd.cmake CHDIR ${T_DIST_DIR})
	endif()
	return()
endif()

block()
	include(Files)
	include(Libraries)
endblock()

if(REVIEW)
	set(PRESET ${PRESET}-review)
endif()

step(${T_CFG} --preset ${PRESET})

if(APK)
	if(REVIEW OR DEFINED ENV{JENKINS_HOME})
		prepare_android_keystore(DEBUG _unused)
		step(${T_TARGET} apk)
	else()
		if(RELEASE)
			prepare_android_keystore(RELEASE STORE_APK)
		else()
			prepare_android_keystore(DEBUG STORE_APK)
		endif()

		step(${T_TARGET} apk ENV
			QT_ANDROID_KEYSTORE_PATH=${STORE_APK}
			QT_ANDROID_KEYSTORE_ALIAS=${STORE_APK_ALIAS}
			QT_ANDROID_KEYSTORE_STORE_PASS=${STORE_APK_PSW})
	endif()

	step(${T_TARGET} dump.apk)

	if(REVIEW OR (DAILY AND DEFINED ENV{JENKINS_HOME}))
		step(${T_TARGET} aab)
	else()
		if(RELEASE)
			prepare_android_keystore(UPLOAD STORE_AAB)
		else()
			prepare_android_keystore(DEBUG STORE_AAB)
		endif()

		if(DEFINED ENV{JENKINS_HOME})
			set(STORE_AAB $ENV{APK_SIGN_KEYSTORE_UPLOAD})
			set(STORE_AAB_ALIAS $ENV{APK_SIGN_KEYSTORE_ALIAS_UPLOAD})
			set(STORE_AAB_PSW $ENV{APK_SIGN_KEYSTORE_PSW_UPLOAD})
		endif()

		step(${T_TARGET} aab ENV
			QT_ANDROID_KEYSTORE_PATH=${STORE_AAB}
			QT_ANDROID_KEYSTORE_ALIAS=${STORE_AAB_ALIAS}
			QT_ANDROID_KEYSTORE_STORE_PASS=${STORE_AAB_PSW})
	endif()

	step(${T_TARGET} verify.signature)
	file(GLOB FILES "${T_DIST_DIR}/*.apk" "${T_DIST_DIR}/*.aab")
elseif(AAR)
	step(${T_TARGET} aar)
	file(GLOB FILES "${T_DIST_DIR}/*.aar")
endif()

hashsum(${FILES})

if(NOT RELEASE)
	step(${T_CTEST})
endif()
