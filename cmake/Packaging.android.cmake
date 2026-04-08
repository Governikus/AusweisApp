set(RESOURCES_IMG_ANDROID_DIR ${RESOURCES_DIR}/images/android)
if(IS_FINAL_VERSION)
	set(ANDROID_LAUNCHER_ICON "npa.png")
	set(ANDROID_SPLASH_SCREEN_ICON_NAME "splash_npa.png")
else()
	set(ANDROID_LAUNCHER_ICON "npa_beta.png")
	set(ANDROID_SPLASH_SCREEN_ICON_NAME "splash_npa_beta.png")
endif()

if(INTEGRATED_SDK)
	set(ANDROID_MANIFEST AndroidManifest.xml.aar.in)
	set(JAVA_FILES network/WifiInfo.java ui/aidl/AidlBinder.java android/LogHandler.java android/BootstrapHelper.java android/AusweisApp2Service.java android/AusweisApp2LocalIfdServiceConnection.java)

	configure_file(${PACKAGING_DIR}/android/res/values/strings.xml ${ANDROID_PACKAGE_SRC_DIR}/res/values/strings.xml COPYONLY)
else()
	set(ANDROID_MANIFEST AndroidManifest.xml.apk.in)
	set(JAVA_FILES *.java)

	foreach(entry ldpi mdpi hdpi xhdpi xxhdpi xxxhdpi)
		configure_file(${RESOURCES_IMG_ANDROID_DIR}/${entry}/background_npa.png ${ANDROID_PACKAGE_SRC_DIR}/res/mipmap-${entry}/npa_background.png COPYONLY)
		configure_file(${RESOURCES_IMG_ANDROID_DIR}/${entry}/foreground_${ANDROID_LAUNCHER_ICON} ${ANDROID_PACKAGE_SRC_DIR}/res/mipmap-${entry}/npa_foreground.png COPYONLY)
		configure_file(${RESOURCES_IMG_ANDROID_DIR}/${entry}/monochrome_${ANDROID_LAUNCHER_ICON} ${ANDROID_PACKAGE_SRC_DIR}/res/mipmap-${entry}/npa_monochrome.png COPYONLY)
		configure_file(${RESOURCES_IMG_ANDROID_DIR}/${entry}/${ANDROID_SPLASH_SCREEN_ICON_NAME} ${ANDROID_PACKAGE_SRC_DIR}/res/drawable-${entry}/splash_npa.png COPYONLY)
	endforeach()

	configure_files("${PACKAGING_DIR}/android" "res/*" "${ANDROID_PACKAGE_SRC_DIR}")
endif()

configure_files(${SRC_DIR} "${JAVA_FILES}" ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2 FLATTEN)
configure_file(${PACKAGING_DIR}/android/IAusweisApp2Sdk.aidl ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2/IAusweisApp2Sdk.aidl COPYONLY)
configure_file(${PACKAGING_DIR}/android/IAusweisApp2SdkCallback.aidl ${ANDROID_PACKAGE_SRC_DIR}/src/com/governikus/ausweisapp2/IAusweisApp2SdkCallback.aidl COPYONLY)

set(ANDROID_VERSION_NAME ${ARTIFACT_VERSION})
configure_file(${PACKAGING_DIR}/android/${ANDROID_MANIFEST} ${ANDROID_PACKAGE_SRC_DIR}/AndroidManifest.xml @ONLY)
if(INTEGRATED_SDK)
	set(ANDROID_ROOT_LOGGER "java")
else()
	set(ANDROID_ROOT_LOGGER "")
	configure_file(${PACKAGING_DIR}/android/fileprovider.xml ${ANDROID_PACKAGE_SRC_DIR}/res/xml/fileprovider.xml COPYONLY)
	configure_file(${PACKAGING_DIR}/android/full_backup_content.xml ${ANDROID_PACKAGE_SRC_DIR}/res/xml/full_backup_content.xml COPYONLY)
	configure_file(${PACKAGING_DIR}/android/data_extraction_rules.xml ${ANDROID_PACKAGE_SRC_DIR}/res/xml/data_extraction_rules.xml COPYONLY)
endif()


set(QT_BUILD_GRADLE "${QT_INSTALL_ARCHDATA}/src/android/templates/build.gradle")
set(BUILD_GRADLE_APPEND "${PACKAGING_DIR}/android/build.gradle.append")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${QT_BUILD_GRADLE}")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${BUILD_GRADLE_APPEND}")

file(READ "${QT_BUILD_GRADLE}" BUILD_GRADLE)
if(DEFINED ENV{MIRROR_MAVEN})
	string(REPLACE "mavenCentral()" "maven {url = uri(System.getenv(\"MIRROR_MAVEN\"))}" BUILD_GRADLE "${BUILD_GRADLE}")
endif()
if(DEFINED ENV{MIRROR_GOOGLE_MAVEN})
	string(REPLACE "google()" "maven {url = uri(System.getenv(\"MIRROR_GOOGLE_MAVEN\"))}" BUILD_GRADLE "${BUILD_GRADLE}")
endif()
file(WRITE "${ANDROID_BUILD_DIR}/build.gradle" "${BUILD_GRADLE}")
file(READ "${BUILD_GRADLE_APPEND}" BUILD_GRADLE)
file(APPEND "${ANDROID_BUILD_DIR}/build.gradle" "${BUILD_GRADLE}")

if(INTEGRATED_SDK)
	set(BUILD_GRADLE_APPEND "${PACKAGING_DIR}/android/build.gradle.append.aar")
	file(READ "${BUILD_GRADLE_APPEND}" BUILD_GRADLE)
	file(APPEND "${ANDROID_BUILD_DIR}/build.gradle" "${BUILD_GRADLE}")
endif()

if(NOT INTEGRATED_SDK)
	file(APPEND "${ANDROID_BUILD_DIR}/build.gradle" "android.defaultConfig.applicationId '${BUNDLE_IDENTIFIER}'\n")
endif()

option(ANDROID_LINT "Lint Android package" ON)
if(NOT ANDROID_LINT)
	file(APPEND "${ANDROID_BUILD_DIR}/build.gradle" "tasks.lint.enabled = false\n")
endif()

if(INTEGRATED_SDK)
	set(ANDROID_FILE_EXT aar)
	file(APPEND "${ANDROID_BUILD_DIR}/build.gradle" "android.defaultConfig.consumerProguardFiles 'consumer-rules.pro'\n")
	if(DEFINED dvcs_revision)
		set(POM_SNAPSHOT "-SNAPSHOT")
	endif()
	configure_file(${PACKAGING_DIR}/android/pom.xml.in ${ANDROID_BUILD_DIR}/${CPACK_PACKAGE_FILE_NAME}.pom @ONLY)
	configure_file("${PACKAGING_DIR}/android/lint.aar.xml" "${ANDROID_BUILD_DIR}/lint-baseline.xml" COPYONLY)
	configure_file("${PACKAGING_DIR}/android/consumer-rules.pro" "${ANDROID_BUILD_DIR}/consumer-rules.pro" COPYONLY)
else()
	set(ANDROID_FILE_EXT apk)
	configure_file("${PACKAGING_DIR}/android/lint.apk.xml" "${ANDROID_BUILD_DIR}/lint-baseline.xml" COPYONLY)
endif()
configure_file(${PACKAGING_DIR}/android/gradle.properties.in ${ANDROID_BUILD_DIR}/gradle.properties @ONLY)

message(STATUS "Prepare ${ANDROID_FILE_EXT} file generation")

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	if(QT_ANDROID_SIGN_APK)
		set(ANDROID_FILE ${ANDROID_BUILD_NAME}-debug-signed.${ANDROID_FILE_EXT})
	else()
		set(ANDROID_FILE ${ANDROID_BUILD_NAME}-debug.${ANDROID_FILE_EXT})
	endif()
	set(ANDROID_FILE_AAB ${ANDROID_BUILD_NAME}-debug.aab)
else()
	if(INTEGRATED_SDK)
		set(DEPLOY_CMD_SIGN --release)
		set(ANDROID_FILE ${ANDROID_BUILD_NAME}-release.aar)
	elseif(QT_ANDROID_SIGN_APK)
		message(STATUS "Release build will be signed using: $ENV{QT_ANDROID_KEYSTORE_PATH} | Alias: $ENV{QT_ANDROID_KEYSTORE_ALIAS}")
		set(ANDROID_FILE ${ANDROID_BUILD_NAME}-release-signed.apk)
	else()
		set(ANDROID_FILE ${ANDROID_BUILD_NAME}-release-unsigned.apk)
		message(WARNING "Cannot sign release build! Set environment like QT_ANDROID_KEYSTORE_PATH")
	endif()
	set(ANDROID_FILE_AAB ${ANDROID_BUILD_NAME}-release.aab)
endif()

set(SOURCE_ANDROID_FILE_OUTPUT ${ANDROID_BUILD_DIR}/build/outputs)
set(SOURCE_ANDROID_FILE ${SOURCE_ANDROID_FILE_OUTPUT}/${ANDROID_FILE_EXT})
if(NOT INTEGRATED_SDK)
	set(SOURCE_ANDROID_FILE_AAB ${SOURCE_ANDROID_FILE_OUTPUT}/bundle)
	if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
		set(SOURCE_ANDROID_FILE ${SOURCE_ANDROID_FILE}/debug)
		set(SOURCE_ANDROID_FILE_AAB ${SOURCE_ANDROID_FILE_AAB}/debug)
	else()
		set(SOURCE_ANDROID_FILE ${SOURCE_ANDROID_FILE}/release)
		set(SOURCE_ANDROID_FILE_AAB ${SOURCE_ANDROID_FILE_AAB}/release)
	endif()
endif()
set(SOURCE_ANDROID_FILE ${SOURCE_ANDROID_FILE}/${ANDROID_FILE})
set(SOURCE_ANDROID_FILE_AAB ${SOURCE_ANDROID_FILE_AAB}/${ANDROID_FILE_AAB})

set(DESTINATION_ANDROID_FILE_BASE ${PROJECT_BINARY_DIR}/dist/${CPACK_PACKAGE_FILE_NAME})
set(DESTINATION_ANDROID_FILE ${DESTINATION_ANDROID_FILE_BASE}.${ANDROID_FILE_EXT})
set(DESTINATION_ANDROID_FILE_AAB ${DESTINATION_ANDROID_FILE_BASE}.aab)
if(INTEGRATED_SDK)
	add_custom_command(TARGET aar POST_BUILD
				COMMAND ${ANDROID_BUILD_DIR}/gradlew sourcesJar
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "build/libs/${ANDROID_BUILD_NAME}-sources.jar" "${PROJECT_BINARY_DIR}/dist/${CPACK_PACKAGE_FILE_NAME}-sources.jar"
				WORKING_DIRECTORY ${ANDROID_BUILD_DIR})
else()
	add_custom_command(TARGET aab POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOURCE_ANDROID_FILE_AAB}" "${DESTINATION_ANDROID_FILE_BASE}.aab"
				WORKING_DIRECTORY ${ANDROID_BUILD_DIR})

	if(QT_ANDROID_SIGN_APK)
		add_custom_command(TARGET apk POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOURCE_ANDROID_FILE}.idsig" "${DESTINATION_ANDROID_FILE}.idsig"
					WORKING_DIRECTORY ${ANDROID_BUILD_DIR})
	endif()

	if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
		set(DEBUG_SYMBOLS "${ANDROID_BUILD_DIR}/build/outputs/native-debug-symbols/debug/native-debug-symbols.zip")
	else()
		set(DEBUG_SYMBOLS "${ANDROID_BUILD_DIR}/build/outputs/native-debug-symbols/release/native-debug-symbols.zip")
	endif()
	add_custom_command(TARGET apk POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "${DEBUG_SYMBOLS}" "${DESTINATION_ANDROID_FILE}.native-debug-symbols.zip"
		WORKING_DIRECTORY ${ANDROID_BUILD_DIR})
endif()

add_custom_command(TARGET ${ANDROID_FILE_EXT} POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOURCE_ANDROID_FILE}" "${DESTINATION_ANDROID_FILE}"
			COMMAND ${ANDROID_BUILD_DIR}/gradlew lint
			WORKING_DIRECTORY ${ANDROID_BUILD_DIR})

if(INTEGRATED_SDK)
	add_custom_command(TARGET aar POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ANDROID_BUILD_DIR}/${CPACK_PACKAGE_FILE_NAME}.pom" "${PROJECT_BINARY_DIR}/dist/${CPACK_PACKAGE_FILE_NAME}.pom"
				WORKING_DIRECTORY ${ANDROID_BUILD_DIR})
else()
	find_program(apksigner apksigner HINTS ${ANDROID_SDK_ROOT}/build-tools/${ANDROID_BUILD_TOOLS_REVISION})
	find_program(jarsigner jarsigner)

	if(apksigner OR jarsigner)
		add_custom_target(verify.signature)
	endif()

	if(apksigner)
		if(QT_ANDROID_SIGN_APK)
			set(APKSIGNER_PARAM -v4-signature-file ${DESTINATION_ANDROID_FILE}.idsig)
		endif()
		add_custom_target(verify.signature.apk COMMAND ${apksigner} verify --verbose --print-certs -Werr ${APKSIGNER_PARAM} ${DESTINATION_ANDROID_FILE})
		add_dependencies(verify.signature verify.signature.apk)
	endif()

	if(jarsigner)
		add_custom_target(verify.signature.aab COMMAND ${jarsigner} -verify -verbose -certs ${DESTINATION_ANDROID_FILE_AAB})
		add_dependencies(verify.signature verify.signature.aab)
	endif()

	find_program(aapt NAMES aapt2 aapt HINTS ${ANDROID_SDK_ROOT}/build-tools/${ANDROID_BUILD_TOOLS_REVISION})
	if(aapt)
		set(ANDROID_DUMP ${DESTINATION_ANDROID_FILE}.dump)
		add_custom_command(OUTPUT ${ANDROID_DUMP} COMMAND ${aapt} dump badging ${DESTINATION_ANDROID_FILE} > ${ANDROID_DUMP})
		add_custom_target(dump.apk DEPENDS ${ANDROID_DUMP})
	endif()
endif()
