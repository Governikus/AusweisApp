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
		hashsum(${file} NO_FILENAME ALGORITHM MD5 SHA1 SHA256)
		step(${GPG_CMD} -a --detach-sig -u $ENV{GPG_ID} ${file})
	endforeach()

	cmake_minimum_required(VERSION 3.29) # CMP0159

	file(GLOB POM "${T_DIST_DIR}/*.pom")
	list(LENGTH POM POM_COUNT)
	if(NOT POM_COUNT EQUAL 1)
		message(FATAL_ERROR "Cannot find unique POM file: ${POM}")
	endif()

	function(parse_pom _tag)
		file(STRINGS "${POM}" _unused REGEX "<${_tag}>(.+)</${_tag}>")
		set(${_tag} ${CMAKE_MATCH_1} PARENT_SCOPE)
		message(STATUS "Parse ${_tag}: ${CMAKE_MATCH_1}")
	endfunction()

	parse_pom(groupId)
	parse_pom(artifactId)
	parse_pom(version)

	string(REPLACE "." ";" groupId_List "${groupId}")
	list(JOIN groupId_List "/" groupId_Dir)
	list(GET groupId_List 0 groupId_RootDir)

	set(BUNDLE_DIR ${T_BUILD_DIR}/${groupId_Dir}/${artifactId}/${version})
	step(${CMAKE_COMMAND} -E make_directory ${BUNDLE_DIR})

	file(GLOB FILES "${T_DIST_DIR}/${artifactId}-*")
	foreach(file ${FILES})
		step(${CMAKE_COMMAND} -E copy_if_different ${file} ${BUNDLE_DIR})
	endforeach()

	step(${CMAKE_COMMAND} -E tar cf dist/${artifactId}-${version}.zip --format=zip ${groupId_RootDir} CHDIR ${T_BUILD_DIR})

	if(DEFINED ENV{JENKINS_HOME} AND RELEASE)
		list(APPEND CMAKE_MODULE_PATH ${CMAKE_CI_DIR}/Deploy.cmake)
		include(AAR)
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

if(APK AND RELEASE AND DEFINED ENV{GITLAB_CI} AND DEFINED ENV{ANDROID_API_KEY})
	set(API_JSON ${WORKSPACE}/api.json)
	download_file(${API_JSON} $ENV{ANDROID_API_KEY})
	find_package(Python REQUIRED)
	step(${Python_EXECUTABLE} ${CMAKE_SOURCE_DIR}/ci/playstore.py --json ${API_JSON} OUTPUT VERSION_CODE)
	message(STATUS "Next VERSION_CODE: ${VERSION_CODE}")
	set(VERSION_CODE -DANDROID_VERSION_CODE_BASE=${VERSION_CODE})
endif()
step(${T_CFG} ${VERSION_CODE} --preset ${PRESET})

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

find_program(XSLTPROC xsltproc REQUIRED)
file(GLOB LINT_RESULT "${T_BUILD_DIR}/src/android-build/build/reports/lint-results*.xml")
step(${XSLTPROC} ${CMAKE_SOURCE_DIR}/ci/lint-to-junit.xsl ${LINT_RESULT} OUTPUT_FILE ${T_BUILD_DIR}/lint-results-junit.xml)

hashsum(${FILES})

if(NOT RELEASE)
	step(${T_CTEST})

	step(grep -Fq failures=\"0\" ${T_BUILD_DIR}/lint-results-junit.xml)
endif()
