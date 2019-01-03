IF(COVERAGE)

	# LCov (http://ltp.sourceforge.net/coverage/lcov.php)
	FIND_PROGRAM(LCOV_BIN lcov CMAKE_FIND_ROOT_PATH_BOTH)
	IF(LCOV_BIN)
		SET(LCOV_FILE "${PROJECT_BINARY_DIR}/coverage.info")
		SET(LCOV_GLOBAL_CMD ${LCOV_BIN} -q -o ${LCOV_FILE})
		SET(LCOV_CMD ${LCOV_GLOBAL_CMD} -c -d ${PROJECT_BINARY_DIR} -b ${PROJECT_SOURCE_DIR})
		SET(LCOV_RM_CMD ${LCOV_GLOBAL_CMD} -r ${LCOV_FILE} "*/test/*" "*/include/*" "*/src/external/*" "moc_*" "*.moc" "qrc_*" "ui_*")

		IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
			IF(WIN32)
				SET(CLANG_GCOV py)
			ELSE()
				SET(CLANG_GCOV sh)
			ENDIF()
			SET(LCOV_CMD ${LCOV_CMD} --gcov-tool ${RESOURCES_DIR}/jenkins/clang-gcov.${CLANG_GCOV})
		ENDIF()

		ADD_CUSTOM_COMMAND(OUTPUT ${LCOV_FILE} COMMAND ${LCOV_CMD} COMMAND ${LCOV_RM_CMD})
		ADD_CUSTOM_TARGET(lcov COMMAND ${LCOV_BIN} -l ${LCOV_FILE} DEPENDS ${LCOV_FILE})

		FIND_PROGRAM(GENHTML_BIN genhtml CMAKE_FIND_ROOT_PATH_BOTH)
		IF(GENHTML_BIN)
			SET(REPORT_DIR "${PROJECT_BINARY_DIR}/coverage.report")
			SET(GENHTML_CMD ${GENHTML_BIN} -q -p ${PROJECT_SOURCE_DIR} --num-spaces=4 -o ${REPORT_DIR} ${LCOV_FILE})
			FIND_PROGRAM(FILT_BIN c++filt CMAKE_FIND_ROOT_PATH_BOTH)
			IF(FILT_BIN)
				SET(GENHTML_CMD ${GENHTML_CMD} --demangle-cpp)
			ENDIF()

			ADD_CUSTOM_COMMAND(OUTPUT ${REPORT_DIR} COMMAND ${GENHTML_CMD} DEPENDS ${LCOV_FILE})
			ADD_CUSTOM_TARGET(lcov.report DEPENDS ${REPORT_DIR})
		ENDIF()

		SET(LCOV_XML "${PROJECT_BINARY_DIR}/coverage.xml")
		SET(LCOV_COBERTURA_CMD ${RESOURCES_DIR}/jenkins/lcov_cobertura.py ${LCOV_FILE} -b ${PROJECT_SOURCE_DIR} -o ${LCOV_XML})
		ADD_CUSTOM_COMMAND(OUTPUT ${LCOV_XML} COMMAND ${LCOV_COBERTURA_CMD} DEPENDS ${LCOV_FILE})
		ADD_CUSTOM_TARGET(lcov.xml DEPENDS ${LCOV_XML})
	ENDIF()

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

	ADD_CUSTOM_COMMAND(OUTPUT ${DOXYGEN_BIN_DIR} COMMAND ${DOXYGEN_CMD})
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
	FILE(GLOB_RECURSE FILES_JAVA ${PROJECT_SOURCE_DIR}/*.java)
	FILE(GLOB_RECURSE FILES_CPP ${PROJECT_SOURCE_DIR}/*.cpp)
	FILE(GLOB_RECURSE FILES_H ${PROJECT_SOURCE_DIR}/*.h)
	FILE(GLOB_RECURSE FILES_MM ${PROJECT_SOURCE_DIR}/*.mm)
	FILE(GLOB_RECURSE FILES_M ${PROJECT_SOURCE_DIR}/*.m)
	SET(FILES ${FILES_JAVA} ${FILES_CPP} ${FILES_H} ${FILES_MM} ${FILES_M})
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

	EXECUTE_PROCESS(COMMAND ${UNCRUSTIFY} --version OUTPUT_VARIABLE UNCRUSTIFY_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	STRING(REPLACE "uncrustify " "" UNCRUSTIFY_VERSION ${UNCRUSTIFY_VERSION})

	SET(UNCRUSTIFY_NEEDED_VERSION "0.67")
	IF("${UNCRUSTIFY_VERSION}" STRLESS "${UNCRUSTIFY_NEEDED_VERSION}")
		MESSAGE(WARNING "Uncrustify seems to be too old. Use at least ${UNCRUSTIFY_NEEDED_VERSION}... you are using: ${UNCRUSTIFY_VERSION}")
	ELSE()
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
	IF(IOS)
	    SET(CONVERT_CMD convert -alpha off)
	    SET(BACKGROUND_COLOR "#5489c2")
	ELSE()
	    SET(CONVERT_CMD convert)
	    SET(BACKGROUND_COLOR "transparent")
	ENDIF()

	ADD_CUSTOM_TARGET(npaicons.win
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -define icon:auto-resize=256,96,64,48,40,32,24,20,16 npa.svg npa.ico
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android.preview
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 36x36 npa_preview.svg android/ldpi/npa_preview.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 160 -resize 48x48 npa_preview.svg android/mdpi/npa_preview.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 240 -resize 72x72 npa_preview.svg android/hdpi/npa_preview.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 320 -resize 96x96 npa_preview.svg android/xhdpi/npa_preview.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 480 -resize 144x144 npa_preview.svg android/xxhdpi/npa_preview.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 640 -resize 192x192 npa_preview.svg android/xxxhdpi/npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android.beta
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 36x36 npa_beta.svg android/ldpi/npa_beta.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 160 -resize 48x48 npa_beta.svg android/mdpi/npa_beta.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 240 -resize 72x72 npa_beta.svg android/hdpi/npa_beta.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 320 -resize 96x96 npa_beta.svg android/xhdpi/npa_beta.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 480 -resize 144x144 npa_beta.svg android/xxhdpi/npa_beta.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 640 -resize 192x192 npa_beta.svg android/xxxhdpi/npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.android
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 36x36 npa.svg android/ldpi/npa.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 160 -resize 48x48 npa.svg android/mdpi/npa.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 240 -resize 72x72 npa.svg android/hdpi/npa.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 320 -resize 96x96 npa.svg android/xhdpi/npa.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 480 -resize 144x144 npa.svg android/xxhdpi/npa.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -units PixelsPerInch -resample 640 -resize 192x192 npa.svg android/xxxhdpi/npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.ios.beta
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 20x20 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 40x40 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 60x60 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 29x29 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 58x58 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 87x87 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 40x40 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 80x80 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 120x120 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 120x120 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 180x180 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 76x76 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 152x152 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 167x167 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 1024x1024 npa_beta.svg iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.ios
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 20x20 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 40x40 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 60x60 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 29x29 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 58x58 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 87x87 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 40x40 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 80x80 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 120x120 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 120x120 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 180x180 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 76x76 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 152x152 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 167x167 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 1024x1024 npa.svg iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	SET(BACKGROUND_COLOR "rgb\(220,235,246\)")

	ADD_CUSTOM_TARGET(npaicons.iphone
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 320x320 -gravity center -extent 640x1136 npa.svg iOS/launchImages/Default-568h@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 320x320 -gravity center -extent 640x1136 npa.svg iOS/launchImages/launchImage568@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 480x480 -gravity center -extent 960x1704 npa.svg iOS/launchImages/launchImage568@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 353x353 -gravity center -extent 705x1334 npa.svg iOS/launchImages/launchImage667@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 563x563 -gravity center -extent 1125x2001 npa.svg iOS/launchImages/launchImage667@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 414x414 -gravity center -extent 828x1472 npa.svg iOS/launchImages/launchImage736@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 621x621 -gravity center -extent 1242x2208 npa.svg iOS/launchImages/launchImage736@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons.ipad
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 768x768 -gravity center -extent 2048x1536 npa.svg iOS/launchImages/launchImage1024@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 1152x1152 -gravity center -extent 3072x2304 npa.svg iOS/launchImages/launchImage1024@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 834x834 -gravity center -extent 2224x1668 npa.svg iOS/launchImages/launchImage1112@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 1251x1251 -gravity center -extent 3336x2502 npa.svg iOS/launchImages/launchImage1112@3x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 1024x1024 -gravity center -extent 2732x2048 npa.svg iOS/launchImages/launchImage1366@2x.png
		COMMAND ${CONVERT_CMD} -background '${BACKGROUND_COLOR}' -resize 1536x1536 -gravity center -extent 4098x3072 npa.svg iOS/launchImages/launchImage1366@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(npaicons DEPENDS npaicons.win npaicons.ios npaicons.ios.beta npaicons.iphone npaicons.ipad npaicons.android npaicons.android.beta npaicons.android.preview)
ENDIF()

FIND_PROGRAM(PNGQUANT pngquant CMAKE_FIND_ROOT_PATH_BOTH)
IF(PNGQUANT)
SET(PNGQUANT_CMD pngquant -f -o)
	ADD_CUSTOM_TARGET(pngquant.ios.beta
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20@2x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20@3x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon60@2x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon60@3x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon76.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon76@2x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon1024.png -- iOS/appIcons/beta/Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.ios
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20@2x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20@3x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon60@2x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon60@3x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon76.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon76@2x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon1024.png -- iOS/appIcons/Images.xcassets/AppIcon.appiconset/icon1024.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/Default-568h@2x.png -- iOS/launchImages/Default-568h@2x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.iphone
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage568@2x.png -- iOS/launchImages/launchImage568@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage568@3x.png -- iOS/launchImages/launchImage568@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage667@2x.png -- iOS/launchImages/launchImage667@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage667@3x.png -- iOS/launchImages/launchImage667@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage736@2x.png -- iOS/launchImages/launchImage736@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage736@3x.png -- iOS/launchImages/launchImage736@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.ipad
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage1024@2x.png -- iOS/launchImages/launchImage1024@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage1024@3x.png -- iOS/launchImages/launchImage1024@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage1112@2x.png -- iOS/launchImages/launchImage1112@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage1112@3x.png -- iOS/launchImages/launchImage1112@3x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage1366@2x.png -- iOS/launchImages/launchImage1366@2x.png
		COMMAND ${PNGQUANT_CMD} iOS/launchImages/launchImage1366@3x.png -- iOS/launchImages/launchImage1366@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.android.preview
		COMMAND ${PNGQUANT_CMD} android/ldpi/npa_preview.png -- android/ldpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} android/mdpi/npa_preview.png -- android/mdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} android/hdpi/npa_preview.png -- android/hdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} android/xhdpi/npa_preview.png -- android/xhdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} android/xxhdpi/npa_preview.png -- android/xxhdpi/npa_preview.png
		COMMAND ${PNGQUANT_CMD} android/xxxhdpi/npa_preview.png -- android/xxxhdpi/npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.android.beta
		COMMAND ${PNGQUANT_CMD} android/ldpi/npa_beta.png -- android/ldpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} android/mdpi/npa_beta.png -- android/mdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} android/hdpi/npa_beta.png -- android/hdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} android/xhdpi/npa_beta.png -- android/xhdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} android/xxhdpi/npa_beta.png -- android/xxhdpi/npa_beta.png
		COMMAND ${PNGQUANT_CMD} android/xxxhdpi/npa_beta.png -- android/xxxhdpi/npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant.android
		COMMAND ${PNGQUANT_CMD} android/ldpi/npa.png -- android/ldpi/npa.png
		COMMAND ${PNGQUANT_CMD} android/mdpi/npa.png -- android/mdpi/npa.png
		COMMAND ${PNGQUANT_CMD} android/hdpi/npa.png -- android/hdpi/npa.png
		COMMAND ${PNGQUANT_CMD} android/xhdpi/npa.png -- android/xhdpi/npa.png
		COMMAND ${PNGQUANT_CMD} android/xxhdpi/npa.png -- android/xxhdpi/npa.png
		COMMAND ${PNGQUANT_CMD} android/xxxhdpi/npa.png -- android/xxxhdpi/npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	ADD_CUSTOM_TARGET(pngquant DEPENDS pngquant.ios pngquant.ios.beta pngquant.iphone pngquant.ipad pngquant.android pngquant.android.beta pngquant.android.preview)
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
