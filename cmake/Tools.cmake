IF(COVERAGE)
	# gcovr (http://gcovr.com/)
	FIND_PROGRAM(GCOVR_BIN gcovr CMAKE_FIND_ROOT_PATH_BOTH)
	IF(GCOVR_BIN)
		SET(GCOVR_FILE "${PROJECT_BINARY_DIR}/gcovr.xml")
		SET(GCOVR_CMD ${GCOVR_BIN} -x -o ${GCOVR_FILE} --exclude="utils" --exclude="src/external" --exclude="test" -r ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

		ADD_CUSTOM_COMMAND(OUTPUT ${GCOVR_FILE} COMMAND ${GCOVR_CMD} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
		ADD_CUSTOM_TARGET(gcovr DEPENDS ${GCOVR_FILE})
	ENDIF()
ENDIF()

# CppCheck (http://cppcheck.sourceforge.net)
FIND_PROGRAM(CPPCHECK_BIN cppcheck CMAKE_FIND_ROOT_PATH_BOTH)
IF(CPPCHECK_BIN)
	SET(XML_FILE "${PROJECT_BINARY_DIR}/cppcheck.xml")
	SET(XML_FILE_TESTS "${PROJECT_BINARY_DIR}/cppcheck.tests.xml")

	SET(CPPCHECK_SUPPRESS --suppress=missingInclude --suppress=unmatchedSuppression --suppress=unusedFunction --suppress=noExplicitConstructor)
	SET(CPPCHECK_SUPPRESS_SRC ${CPPCHECK_SUPPRESS})
	SET(CPPCHECK_SUPPRESS_TESTS ${CPPCHECK_SUPPRESS} --suppress=noConstructor)

	DIRLIST_OF_FILES(CPPCHECK_INCLUDE_DIRS ${SRC_DIR}/*.h)
	FOREACH(dir ${CPPCHECK_INCLUDE_DIRS})
		SET(CPPCHECK_OPTIONS "${CPPCHECK_OPTIONS} -I${dir}")
	ENDFOREACH()

	SET(CPPCHECK_CMD ${CPPCHECK_BIN} ${CPPCHECK_OPTIONS} --relative-paths=${PROJECT_SOURCE_DIR} --enable=all ${SRC_DIR} ${CPPCHECK_SUPPRESS_SRC} --force)
	SET(CPPCHECK_CMD_TESTS ${CPPCHECK_BIN} ${CPPCHECK_OPTIONS} --relative-paths=${PROJECT_SOURCE_DIR} --enable=all ${TEST_DIR} ${CPPCHECK_SUPPRESS_TESTS} --force)

	SET(CPPCHECK_OPTIONS_FILE -q --xml --xml-version=2)
	ADD_CUSTOM_COMMAND(OUTPUT ${XML_FILE} COMMAND ${CPPCHECK_CMD} ${CPPCHECK_OPTIONS_FILE} 2> ${XML_FILE} COMMAND ${CPPCHECK_CMD_TESTS} ${CPPCHECK_OPTIONS_FILE} 2> ${XML_FILE_TESTS})
	ADD_CUSTOM_COMMAND(OUTPUT ${XML_FILE_TESTS} COMMAND ${CPPCHECK_CMD_TESTS} ${CPPCHECK_OPTIONS_FILE} 2> ${XML_FILE_TESTS})
	ADD_CUSTOM_TARGET(cppcheck COMMAND ${CPPCHECK_CMD_TESTS} -v COMMAND ${CPPCHECK_CMD} -v)
	ADD_CUSTOM_TARGET(cppcheck.report DEPENDS ${XML_FILE} ${XML_FILE_TESTS})
ENDIF()

# CppNcss (http://cppncss.sourceforge.net)
FIND_PROGRAM(CPPNCSS_BIN cppncss CMAKE_FIND_ROOT_PATH_BOTH)
IF(CPPNCSS_BIN)
	SET(XML_FILE "${PROJECT_BINARY_DIR}/cppncss.xml")
	SET(CPPNCSS_CMD ${CPPNCSS_BIN} -k -r -p="${PROJECT_SOURCE_DIR}/" ${SRC_DIR} ${TEST_DIR})

	ADD_CUSTOM_COMMAND(OUTPUT ${XML_FILE} COMMAND ${CPPNCSS_CMD} -x -f="${XML_FILE}")
	ADD_CUSTOM_TARGET(cppncss COMMAND ${CPPNCSS_CMD} -m=CCN,NCSS,function)
	ADD_CUSTOM_TARGET(cppncss.report DEPENDS ${XML_FILE})
ENDIF()

# pmccabe (http://parisc-linux.org/~bame/pmccabe/)
FIND_PROGRAM(PMCCABE_BIN pmccabe CMAKE_FIND_ROOT_PATH_BOTH)
IF(PMCCABE_BIN)
	ADD_CUSTOM_TARGET(pmccabe COMMAND ${PMCCABE_BIN} -v ${SRC_DIR}/*.cpp ${TEST_DIR}/*.cpp)
ENDIF()

# Doxygen (http://www.doxygen.org)
# http://www.stack.nl/~dimitri/doxygen/manual/config.html
FIND_PACKAGE(Doxygen)
IF(DOXYGEN_FOUND)
	SET(DOXYGEN_BIN_DIR "${PROJECT_BINARY_DIR}/doxygen")
	SET(DOXYGEN_CMD ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
	SET(DOXYGEN_CFG ${PROJECT_SOURCE_DIR}/Doxyfile.in)
	CONFIGURE_FILE(${DOXYGEN_CFG} ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)

	ADD_CUSTOM_COMMAND(OUTPUT ${DOXYGEN_BIN_DIR} COMMAND ${DOXYGEN_CMD} USES_TERMINAL)
	ADD_CUSTOM_TARGET(doxy DEPENDS ${DOXYGEN_BIN_DIR} WORKING_DIRECTORY ${PROJECT_BINARY_DIR} SOURCES ${DOXYGEN_CFG})
ENDIF()

FIND_PROGRAM(CLOC_BIN cloc CMAKE_FIND_ROOT_PATH_BOTH)
IF(CLOC_BIN)
	SET(CLOC_FILE "${PROJECT_BINARY_DIR}/cloc.xml")
	SET(CLOC_CMD ${CLOC_BIN} ${CMAKE_SOURCE_DIR})

	ADD_CUSTOM_COMMAND(OUTPUT ${CLOC_FILE} COMMAND ${CLOC_CMD} --by-file-by-lang --xml --report-file=${CLOC_FILE})
	ADD_CUSTOM_TARGET(cloc COMMAND ${CLOC_CMD})
	ADD_CUSTOM_TARGET(cloc.report DEPENDS ${CLOC_FILE})
ENDIF()

FIND_PROGRAM(UNCRUSTIFY uncrustify CMAKE_FIND_ROOT_PATH_BOTH)
IF(UNCRUSTIFY)
	EXECUTE_PROCESS(COMMAND ${UNCRUSTIFY} --version OUTPUT_VARIABLE UNCRUSTIFY_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	STRING(REPLACE "uncrustify " "" UNCRUSTIFY_VERSION ${UNCRUSTIFY_VERSION})
	STRING(REPLACE "Uncrustify-" "" UNCRUSTIFY_VERSION ${UNCRUSTIFY_VERSION})

	SET(UNCRUSTIFY_NEEDED_VERSION "0.70.1")
	IF("${UNCRUSTIFY_VERSION}" STRLESS "${UNCRUSTIFY_NEEDED_VERSION}")
		MESSAGE(WARNING "Uncrustify seems to be too old. Use at least ${UNCRUSTIFY_NEEDED_VERSION}... you are using: ${UNCRUSTIFY_VERSION}")
	ELSE()
		MESSAGE(STATUS "Found uncrustify ${UNCRUSTIFY_VERSION}")
		FILE(GLOB_RECURSE FILES_JAVA ${PROJECT_SOURCE_DIR}/*.java)
		FILE(GLOB_RECURSE FILES_CPP ${PROJECT_SOURCE_DIR}/*.cpp)
		FILE(GLOB_RECURSE FILES_H ${PROJECT_SOURCE_DIR}/*.h)
		FILE(GLOB_RECURSE FILES_H_IN ${PROJECT_SOURCE_DIR}/*.h.in)
		FILE(GLOB_RECURSE FILES_MM ${PROJECT_SOURCE_DIR}/*.mm)
		FILE(GLOB_RECURSE FILES_M ${PROJECT_SOURCE_DIR}/*.m)
		SET(FILES ${FILES_JAVA} ${FILES_CPP} ${FILES_H_IN} ${FILES_H} ${FILES_MM} ${FILES_M})
		SET(FORMATTING_FILE ${PROJECT_BINARY_DIR}/formatting.files)

		FILE(WRITE ${FORMATTING_FILE} "")
		FOREACH(file ${FILES})
			IF(NOT "${file}" MATCHES "/external/")
				FILE(APPEND ${FORMATTING_FILE} ${file})
				FILE(APPEND ${FORMATTING_FILE} "\n")
			ENDIF()
		ENDFOREACH()

		SET(UNCRUSTIFY_CFG ${PROJECT_SOURCE_DIR}/uncrustify.cfg)
		SET(UNCRUSTIFY_CMD ${UNCRUSTIFY} -c ${UNCRUSTIFY_CFG} --replace --no-backup -q -F ${FORMATTING_FILE})
		ADD_CUSTOM_TARGET(format COMMAND ${UNCRUSTIFY_CMD} SOURCES ${UNCRUSTIFY_CFG} ${FILES})
	ENDIF()
ENDIF()

FIND_PROGRAM(QMLLINT_BIN qmllint CMAKE_FIND_ROOT_PATH_BOTH)
IF(QMLLINT_BIN)
	FILE(GLOB_RECURSE TEST_FILES_QML ${TEST_DIR}/qml/*.qml)
	FILE(GLOB_RECURSE FILES_QML ${RESOURCES_DIR}/qml/*.qml)
	FILE(GLOB_RECURSE FILES_JS ${RESOURCES_DIR}/qml/*.js)
	SET(QMLLINT_CMD ${QMLLINT_BIN} ${FILES_QML} ${FILES_JS})

	ADD_CUSTOM_TARGET(qmllint COMMAND ${QMLLINT_CMD} SOURCES ${TEST_FILES_QML} ${FILES_QML} ${FILES_JS})
ENDIF()

# doc8 (https://pypi.python.org/pypi/doc8)
FIND_PROGRAM(DOC8_BIN doc8 CMAKE_FIND_ROOT_PATH_BOTH)
FUNCTION(CREATE_DOC8_TARGET _dir _name)
	IF(DOC8_BIN)
		ADD_CUSTOM_TARGET(doc8.${_name} COMMAND ${DOC8_BIN} --config ${PROJECT_SOURCE_DIR}/docs/doc8.ini WORKING_DIRECTORY ${_dir})
		IF(NOT TARGET doc8)
			ADD_CUSTOM_TARGET(doc8)
		ENDIF()
		ADD_DEPENDENCIES(doc8 doc8.${_name})
	ENDIF()
ENDFUNCTION()

FIND_PROGRAM(CONVERT convert CMAKE_FIND_ROOT_PATH_BOTH)
IF(CONVERT)
	SET(BACKGROUND_COLOR "#dcebf6")

	ADD_CUSTOM_TARGET(npaicons.docs
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 96x96 npa.svg npa_docs.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.win
		COMMAND ${CONVERT} -background transparent -define icon:auto-resize=256,96,64,48,40,32,24,20,16 npa.svg npa.ico
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android.playstore
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resize 512x512 playstore_release.svg playstore_release.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resize 512x512 playstore_preview.svg playstore_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(npaicons.android.legacy
		COMMAND ${CONVERT} -background transparent -resize 36x36 npa.svg android/ldpi/npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 48x48 npa.svg android/mdpi/npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 72x72 npa.svg android/hdpi/npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 96x96 npa.svg android/xhdpi/npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 144x144 npa.svg android/xxhdpi/npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 192x192 npa.svg android/xxxhdpi/npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android.legacy.beta
		COMMAND ${CONVERT} -background transparent -resize 36x36 npa_beta.svg android/ldpi/npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 48x48 npa_beta.svg android/mdpi/npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 72x72 npa_beta.svg android/hdpi/npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 96x96 npa_beta.svg android/xhdpi/npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 144x144 npa_beta.svg android/xxhdpi/npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 192x192 npa_beta.svg android/xxxhdpi/npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android.legacy.preview
		COMMAND ${CONVERT} -background transparent -resize 36x36 npa_preview.svg android/ldpi/npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 48x48 npa_preview.svg android/mdpi/npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 72x72 npa_preview.svg android/hdpi/npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 96x96 npa_preview.svg android/xhdpi/npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 144x144 npa_preview.svg android/xxhdpi/npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 192x192 npa_preview.svg android/xxxhdpi/npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android.adaptive.background
		COMMAND ${CONVERT} -background transparent -resize 81x81 adaptive_background.svg ldpi/background_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 108x108 adaptive_background.svg mdpi/background_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 162x162 adaptive_background.svg hdpi/background_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 216x216 adaptive_background.svg xhdpi/background_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 324x324 adaptive_background.svg xxhdpi/background_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 432x432 adaptive_background.svg xxxhdpi/background_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(npaicons.android.adaptive.foreground
		COMMAND ${CONVERT} -background transparent -resize 81x81 adaptive_foreground_release.svg ldpi/foreground_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 108x108 adaptive_foreground_release.svg mdpi/foreground_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 162x162 adaptive_foreground_release.svg hdpi/foreground_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 216x216 adaptive_foreground_release.svg xhdpi/foreground_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 324x324 adaptive_foreground_release.svg xxhdpi/foreground_npa.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 432x432 adaptive_foreground_release.svg xxxhdpi/foreground_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(npaicons.android.adaptive.foreground.beta
		COMMAND ${CONVERT} -background transparent -resize 81x81 adaptive_foreground_beta.svg ldpi/foreground_npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 108x108 adaptive_foreground_beta.svg mdpi/foreground_npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 162x162 adaptive_foreground_beta.svg hdpi/foreground_npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 216x216 adaptive_foreground_beta.svg xhdpi/foreground_npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 324x324 adaptive_foreground_beta.svg xxhdpi/foreground_npa_beta.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 432x432 adaptive_foreground_beta.svg xxxhdpi/foreground_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(npaicons.android.adaptive.foreground.preview
		COMMAND ${CONVERT} -background transparent -resize 81x81 adaptive_foreground_preview.svg ldpi/foreground_npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 160 -resize 108x108 adaptive_foreground_preview.svg mdpi/foreground_npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 240 -resize 162x162 adaptive_foreground_preview.svg hdpi/foreground_npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 320 -resize 216x216 adaptive_foreground_preview.svg xhdpi/foreground_npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 480 -resize 324x324 adaptive_foreground_preview.svg xxhdpi/foreground_npa_preview.png
		COMMAND ${CONVERT} -background transparent -units PixelsPerInch -resample 640 -resize 432x432 adaptive_foreground_preview.svg xxxhdpi/foreground_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(npaicons.ios.launchimage
		COMMAND ${CONVERT} -background none -resize 256x256 npa.svg iOS/appIcons/Images.xcassets/LaunchImage.imageset/launchImage.png
		COMMAND ${CONVERT} -background none -resize 512x512 npa.svg iOS/appIcons/Images.xcassets/LaunchImage.imageset/launchImage@2x.png
		COMMAND ${CONVERT} -background none -resize 768x768 npa.svg iOS/appIcons/Images.xcassets/LaunchImage.imageset/launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.ios.launchimage.beta
		COMMAND ${CONVERT} -background none -resize 256x256 npa_beta.svg iOS/appIcons/beta/Images.xcassets/LaunchImage.imageset/launchImage.png
		COMMAND ${CONVERT} -background none -resize 512x512 npa_beta.svg iOS/appIcons/beta/Images.xcassets/LaunchImage.imageset/launchImage@2x.png
		COMMAND ${CONVERT} -background none -resize 768x768 npa_beta.svg iOS/appIcons/beta/Images.xcassets/LaunchImage.imageset/launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.ios
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 20x20 npa.svg Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 40x40 npa.svg Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 60x60 npa.svg Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 29x29 npa.svg Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 58x58 npa.svg Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 87x87 npa.svg Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 40x40 npa.svg Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 80x80 npa.svg Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 120x120 npa.svg Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 120x120 npa.svg Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 180x180 npa.svg Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 76x76 npa.svg Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 152x152 npa.svg Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 167x167 npa.svg Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 1024x1024 npa.svg Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/iOS/appIcons)

	ADD_CUSTOM_TARGET(npaicons.ios.beta
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 20x20 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 40x40 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 60x60 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 29x29 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 58x58 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 87x87 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 40x40 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 80x80 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 120x120 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 120x120 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 180x180 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 76x76 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 152x152 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 167x167 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${CONVERT} -alpha off -background '${BACKGROUND_COLOR}' -resize 1024x1024 npa_beta.svg beta/Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/iOS/appIcons)

	ADD_CUSTOM_TARGET(npaicons DEPENDS npaicons.docs npaicons.win npaicons.android.playstore npaicons.android.legacy npaicons.android.legacy.beta npaicons.android.legacy.preview npaicons.android.adaptive.background npaicons.android.adaptive.foreground npaicons.android.adaptive.foreground.beta npaicons.android.adaptive.foreground.preview npaicons.ios.launchimage npaicons.ios.launchimage.beta npaicons.ios npaicons.ios.beta)

	ADD_CUSTOM_TARGET(readerimages
		COMMAND ${CONVERT} -background none -resize 512x512 img_RemoteReader.svg ${RESOURCES_DIR}/updatable-files/reader/img_RemoteReader.png
		COMMAND ${CONVERT} -background none -resize 512x512 img_RemoteReader_mit_ausweis.svg ${RESOURCES_DIR}/updatable-files/reader/img_RemoteReader_mit_ausweis.png
		COMMAND ${CONVERT} -background none -resize 512x512 img_PersoSim.svg ${RESOURCES_DIR}/updatable-files/reader/img_PersoSim.png
		COMMAND ${CONVERT} -background none -resize 512x512 img_PersoSim_mit_ausweis.svg ${RESOURCES_DIR}/updatable-files/reader/img_PersoSim_mit_ausweis.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/reader/src)
ENDIF()

FIND_PROGRAM(PNGQUANT pngquant CMAKE_FIND_ROOT_PATH_BOTH)
IF(PNGQUANT)
	SET(PNGQUANT_CMD pngquant -f -o)

	ADD_CUSTOM_TARGET(pngquant.docs
		COMMAND ${PNGQUANT_CMD} npa_docs.png -- npa_docs.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.android.legacy
		COMMAND ${PNGQUANT_CMD} ldpi/npa.png -- ldpi/npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/npa.png -- mdpi/npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/npa.png -- hdpi/npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/npa.png -- xhdpi/npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/npa.png -- xxhdpi/npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/npa.png -- xxxhdpi/npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.android.legacy.beta
		COMMAND ${PNGQUANT_CMD} ldpi/npa_beta.png -- ldpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} mdpi/npa_beta.png -- mdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} hdpi/npa_beta.png -- hdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} xhdpi/npa_beta.png -- xhdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/npa_beta.png -- xxhdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/npa_beta.png -- xxxhdpi/npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.android.legacy.preview
		COMMAND ${PNGQUANT_CMD} ldpi/npa_preview.png -- ldpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} mdpi/npa_preview.png -- mdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} hdpi/npa_preview.png -- hdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} xhdpi/npa_preview.png -- xhdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/npa_preview.png -- xxhdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/npa_preview.png -- xxxhdpi/npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.android.adaptive.background
		COMMAND ${PNGQUANT_CMD} ldpi/background_npa.png -- ldpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/background_npa.png -- mdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/background_npa.png -- hdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/background_npa.png -- xhdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/background_npa.png -- xxhdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/background_npa.png -- xxxhdpi/background_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.android.adaptive.foreground
		COMMAND ${PNGQUANT_CMD} ldpi/foreground_npa.png -- ldpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/foreground_npa.png -- mdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/foreground_npa.png -- hdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/foreground_npa.png -- xhdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/foreground_npa.png -- xxhdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/foreground_npa.png -- xxxhdpi/foreground_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.android.adaptive.foreground.beta
		COMMAND ${PNGQUANT_CMD} ldpi/foreground_npa_beta.png -- ldpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} mdpi/foreground_npa_beta.png -- mdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} hdpi/foreground_npa_beta.png -- hdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xhdpi/foreground_npa_beta.png -- xhdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/foreground_npa_beta.png -- xxhdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/foreground_npa_beta.png -- xxxhdpi/foreground_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.android.adaptive.foreground.preview
		COMMAND ${PNGQUANT_CMD} ldpi/foreground_npa_preview.png -- ldpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} mdpi/foreground_npa_preview.png -- mdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} hdpi/foreground_npa_preview.png -- hdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xhdpi/foreground_npa_preview.png -- xhdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/foreground_npa_preview.png -- xxhdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/foreground_npa_preview.png -- xxxhdpi/foreground_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	ADD_CUSTOM_TARGET(pngquant.ios.launchimage
		COMMAND ${PNGQUANT_CMD} launchImage.png -- launchImage.png
		COMMAND ${PNGQUANT_CMD} launchImage@2x.png -- launchImage@2x.png
		COMMAND ${PNGQUANT_CMD} launchImage@3x.png -- launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/iOS/appIcons/Images.xcassets/LaunchImage.imageset)

	ADD_CUSTOM_TARGET(pngquant.ios.launchimage.beta
		COMMAND ${PNGQUANT_CMD} launchImage.png -- launchImage.png
		COMMAND ${PNGQUANT_CMD} launchImage@2x.png -- launchImage@2x.png
		COMMAND ${PNGQUANT_CMD} launchImage@3x.png -- launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/iOS/appIcons/beta/Images.xcassets/LaunchImage.imageset)

	ADD_CUSTOM_TARGET(pngquant.ios
		COMMAND ${PNGQUANT_CMD} icon20.png -- icon20.png
		COMMAND ${PNGQUANT_CMD} icon20@2x.png -- icon20@2x.png
		COMMAND ${PNGQUANT_CMD} icon20@3x.png -- icon20@3x.png
		COMMAND ${PNGQUANT_CMD} iconSmall.png -- iconSmall.png
		COMMAND ${PNGQUANT_CMD} iconSmall@2x.png -- iconSmall@2x.png
		COMMAND ${PNGQUANT_CMD} iconSmall@3x.png -- iconSmall@3x.png
		COMMAND ${PNGQUANT_CMD} iconSmall40.png -- iconSmall40.png
		COMMAND ${PNGQUANT_CMD} iconSmall40@2x.png -- iconSmall40@2x.png
		COMMAND ${PNGQUANT_CMD} iconSmall40@3x.png -- iconSmall40@3x.png
		COMMAND ${PNGQUANT_CMD} icon60@2x.png -- icon60@2x.png
		COMMAND ${PNGQUANT_CMD} icon60@3x.png -- icon60@3x.png
		COMMAND ${PNGQUANT_CMD} icon76.png -- icon76.png
		COMMAND ${PNGQUANT_CMD} icon76@2x.png -- icon76@2x.png
		COMMAND ${PNGQUANT_CMD} icon83.5@2x.png -- icon83.5@2x.png
		COMMAND ${PNGQUANT_CMD} icon1024.png -- icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/iOS/appIcons/Images.xcassets/AppIcon.appiconset)

	ADD_CUSTOM_TARGET(pngquant.ios.beta
		COMMAND ${PNGQUANT_CMD} icon20.png -- icon20.png
		COMMAND ${PNGQUANT_CMD} icon20@2x.png -- icon20@2x.png
		COMMAND ${PNGQUANT_CMD} icon20@3x.png -- icon20@3x.png
		COMMAND ${PNGQUANT_CMD} iconSmall.png -- iconSmall.png
		COMMAND ${PNGQUANT_CMD} iconSmall@2x.png -- iconSmall@2x.png
		COMMAND ${PNGQUANT_CMD} iconSmall@3x.png -- iconSmall@3x.png
		COMMAND ${PNGQUANT_CMD} iconSmall40.png -- iconSmall40.png
		COMMAND ${PNGQUANT_CMD} iconSmall40@2x.png -- iconSmall40@2x.png
		COMMAND ${PNGQUANT_CMD} iconSmall40@3x.png -- iconSmall40@3x.png
		COMMAND ${PNGQUANT_CMD} icon60@2x.png -- icon60@2x.png
		COMMAND ${PNGQUANT_CMD} icon60@3x.png -- icon60@3x.png
		COMMAND ${PNGQUANT_CMD} icon76.png -- icon76.png
		COMMAND ${PNGQUANT_CMD} icon76@2x.png -- icon76@2x.png
		COMMAND ${PNGQUANT_CMD} icon83.5@2x.png -- icon83.5@2x.png
		COMMAND ${PNGQUANT_CMD} icon1024.png -- icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset)

	ADD_CUSTOM_TARGET(pngquant DEPENDS pngquant.docs pngquant.android.legacy pngquant.android.legacy.beta pngquant.android.legacy.preview pngquant.android.adaptive.background pngquant.android.adaptive.foreground pngquant.android.adaptive.foreground.beta pngquant.android.adaptive.foreground.preview pngquant.ios.launchimage pngquant.ios.launchimage.beta pngquant.ios pngquant.ios.beta)

	ADD_CUSTOM_TARGET(pngquant.readerimages
		COMMAND ${PNGQUANT_CMD} img_RemoteReader.png -- img_RemoteReader.png
		COMMAND ${PNGQUANT_CMD} img_RemoteReader_mit_ausweis.png -- img_RemoteReader_mit_ausweis.png
		COMMAND ${PNGQUANT_CMD} img_PersoSim.png -- img_PersoSim.png
		COMMAND ${PNGQUANT_CMD} img_PersoSim_mit_ausweis.png -- img_PersoSim_mit_ausweis.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/updatable-files/reader)
ENDIF()

IF(NOT JAVA_EXECUTABLE)
	FIND_PACKAGE(Java COMPONENTS Runtime)
	IF(Java_JAVA_EXECUTABLE)
		SET(JAVA_EXECUTABLE "${Java_JAVA_EXECUTABLE}")
	ENDIF()
ENDIF()
IF(JAVA_EXECUTABLE)
	FIND_FILE(PLANTUML plantuml.jar PATHS ENV HOME NO_DEFAULT_PATH)
	IF(PLANTUML)
		MESSAGE(STATUS "Target uml.statemachines is available using: ${PLANTUML}")
		CONFIGURE_FILE(${RESOURCES_DIR}/statemachine.sh.in ${PROJECT_BINARY_DIR}/statemachine.sh @ONLY)
		ADD_CUSTOM_TARGET(uml.statemachines COMMAND ./statemachine.sh WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
	ENDIF()
ENDIF()


FIND_PROGRAM(DOT dot CMAKE_FIND_ROOT_PATH_BOTH)
IF(DOT)
	SET(architecture_file Architecture)
	SET(ARCHI_PDF_DEPENDS)

	ADD_CUSTOM_TARGET(architecture.graphviz ${CMAKE_COMMAND} --graphviz=${architecture_file} . WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
	LIST(APPEND ARCHI_PDF_DEPENDS architecture.graphviz)

	FIND_PROGRAM(SED sed CMAKE_FIND_ROOT_PATH_BOTH)
	IF(SED)
		# 1. Strip line of plugins as it is misleading
		# 2. Strip "AusweisApp" prefix
		ADD_CUSTOM_TARGET(architecture.sed
			COMMAND ${SED} -i -E '/AusweisApp -> AusweisApp\(Ui|Card|Activation\).+/d' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/"AusweisApp"/"AusweisApp2"/' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/"AusweisApp2"/"REPLACE"/' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/AusweisApp//' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/"REPLACE"/"AusweisApp2"/' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/diamond/box/' ${architecture_file}
			DEPENDS ${ARCHI_PDF_DEPENDS}
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

		LIST(APPEND ARCHI_PDF_DEPENDS architecture.sed)
	ENDIF()

	ADD_CUSTOM_TARGET(architecture
		${DOT} -O -Tpdf ${architecture_file}
		DEPENDS ${ARCHI_PDF_DEPENDS}
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
ENDIF()

FIND_PACKAGE(PythonInterp 2.7)
IF(PYTHONINTERP_FOUND)
	ADD_CUSTOM_TARGET(checkproviderurls
		COMMAND ${PYTHON_EXECUTABLE} "${PROJECT_SOURCE_DIR}/utils/providercheck/check-urls.py" "${PROJECT_SOURCE_DIR}/resources/updatable-files/supported-providers.json")
ENDIF()

FIND_PROGRAM(SED sed CMAKE_FIND_ROOT_PATH_BOTH)
IF(SED)
	FILE(GLOB FILES_TO_GENERATE ${RESOURCES_DIR}/images/tutorial/src/*.svg)
	SET(TARGET_DIR ${RESOURCES_DIR}/images/tutorial/generated)
	ADD_CUSTOM_TARGET(generate_composite_images.sed)
	FOREACH(SRC ${FILES_TO_GENERATE})
	    GET_FILENAME_COMPONENT(SRC_NAME ${SRC} NAME)
	    ADD_CUSTOM_COMMAND(TARGET generate_composite_images.sed PRE_BUILD
		COMMAND ${SED} -E 's/xlink:href=\\"[\\.\\/]+/xlink:href=\\":\\//' ${SRC} > ${TARGET_DIR}/${SRC_NAME})
	ENDFOREACH(SRC)

ENDIF()

INCLUDE(Sphinx)
