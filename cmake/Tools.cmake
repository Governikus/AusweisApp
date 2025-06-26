if(COVERAGE)
	# gcovr (http://gcovr.com/)
	find_program(GCOVR_BIN gcovr CMAKE_FIND_ROOT_PATH_BOTH)
	if(GCOVR_BIN)
		set(GCOVR_FILE "${PROJECT_BINARY_DIR}/gcovr.xml")
		set(GCOVR_SONAR_FILE "${PROJECT_BINARY_DIR}/gcovr_sonarqube.xml")
		set(GCOVR_OPTIONS --gcov-ignore-parse-errors --exclude="${PROJECT_SOURCE_DIR}/utils" --exclude="${PROJECT_SOURCE_DIR}/src/external" --exclude="${PROJECT_SOURCE_DIR}/test" -r ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})

		add_custom_command(OUTPUT ${GCOVR_FILE} COMMAND ${GCOVR_BIN} -x -o ${GCOVR_FILE} ${GCOVR_OPTIONS})
		add_custom_command(OUTPUT ${GCOVR_SONAR_FILE} COMMAND ${GCOVR_BIN} --sonarqube ${GCOVR_SONAR_FILE} ${GCOVR_OPTIONS})
		add_custom_target(gcovr DEPENDS ${GCOVR_FILE})
		add_custom_target(gcovr.sonar DEPENDS ${GCOVR_SONAR_FILE})
	endif()
endif()

# CppCheck (http://cppcheck.sourceforge.net)
find_program(CPPCHECK_BIN cppcheck CMAKE_FIND_ROOT_PATH_BOTH)
if(CPPCHECK_BIN)
	set(XML_FILE "${PROJECT_BINARY_DIR}/cppcheck.xml")
	set(XML_FILE_TESTS "${PROJECT_BINARY_DIR}/cppcheck.tests.xml")

	set(CPPCHECK_SUPPRESS --suppress=missingInclude --suppress=unmatchedSuppression --suppress=unusedFunction --suppress=noExplicitConstructor)
	set(CPPCHECK_SUPPRESS_SRC ${CPPCHECK_SUPPRESS})
	set(CPPCHECK_SUPPRESS_TESTS ${CPPCHECK_SUPPRESS} --suppress=noConstructor)

	DIRLIST_OF_FILES(CPPCHECK_INCLUDE_DIRS ${SRC_DIR}/*.h)
	foreach(dir ${CPPCHECK_INCLUDE_DIRS})
		set(CPPCHECK_OPTIONS "${CPPCHECK_OPTIONS} -I${dir}")
	endforeach()

	set(CPPCHECK_CMD ${CPPCHECK_BIN} ${CPPCHECK_OPTIONS} --relative-paths=${PROJECT_SOURCE_DIR} --enable=all ${SRC_DIR} ${CPPCHECK_SUPPRESS_SRC} --force)
	set(CPPCHECK_CMD_TESTS ${CPPCHECK_BIN} ${CPPCHECK_OPTIONS} --relative-paths=${PROJECT_SOURCE_DIR} --enable=all ${TEST_DIR} ${CPPCHECK_SUPPRESS_TESTS} --force)

	set(CPPCHECK_OPTIONS_FILE -q --xml --xml-version=2)
	add_custom_command(OUTPUT ${XML_FILE} COMMAND ${CPPCHECK_CMD} ${CPPCHECK_OPTIONS_FILE} 2> ${XML_FILE} COMMAND ${CPPCHECK_CMD_TESTS} ${CPPCHECK_OPTIONS_FILE} 2> ${XML_FILE_TESTS})
	add_custom_command(OUTPUT ${XML_FILE_TESTS} COMMAND ${CPPCHECK_CMD_TESTS} ${CPPCHECK_OPTIONS_FILE} 2> ${XML_FILE_TESTS})
	add_custom_target(cppcheck COMMAND ${CPPCHECK_CMD_TESTS} -v COMMAND ${CPPCHECK_CMD} -v)
	add_custom_target(cppcheck.report DEPENDS ${XML_FILE} ${XML_FILE_TESTS})
endif()

# CppNcss (http://cppncss.sourceforge.net)
find_program(CPPNCSS_BIN cppncss CMAKE_FIND_ROOT_PATH_BOTH)
if(CPPNCSS_BIN)
	set(XML_FILE "${PROJECT_BINARY_DIR}/cppncss.xml")
	set(CPPNCSS_CMD ${CPPNCSS_BIN} -k -r -p="${PROJECT_SOURCE_DIR}/" ${SRC_DIR} ${TEST_DIR})

	add_custom_command(OUTPUT ${XML_FILE} COMMAND ${CPPNCSS_CMD} -x -f="${XML_FILE}")
	add_custom_target(cppncss COMMAND ${CPPNCSS_CMD} -m=CCN,NCSS,function)
	add_custom_target(cppncss.report DEPENDS ${XML_FILE})
endif()

# pmccabe (http://parisc-linux.org/~bame/pmccabe/)
find_program(PMCCABE_BIN pmccabe CMAKE_FIND_ROOT_PATH_BOTH)
if(PMCCABE_BIN)
	add_custom_target(pmccabe COMMAND ${PMCCABE_BIN} -v ${SRC_DIR}/*.cpp ${TEST_DIR}/*.cpp)
endif()

# Doxygen (http://www.doxygen.org)
# http://www.stack.nl/~dimitri/doxygen/manual/config.html
find_package(Doxygen)
if(DOXYGEN_FOUND)
	set(DOXYGEN_BIN_DIR "${PROJECT_BINARY_DIR}/doxygen")
	set(DOXYGEN_CMD ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile)
	set(DOXYGEN_CFG ${PROJECT_SOURCE_DIR}/Doxyfile.in)
	configure_file(${DOXYGEN_CFG} ${PROJECT_BINARY_DIR}/Doxyfile @ONLY)

	add_custom_command(OUTPUT ${DOXYGEN_BIN_DIR} COMMAND ${DOXYGEN_CMD} USES_TERMINAL)
	add_custom_target(doxy DEPENDS ${DOXYGEN_BIN_DIR} WORKING_DIRECTORY ${PROJECT_BINARY_DIR} SOURCES ${DOXYGEN_CFG})
endif()

find_program(CLOC_BIN cloc CMAKE_FIND_ROOT_PATH_BOTH)
if(CLOC_BIN)
	set(CLOC_FILE "${PROJECT_BINARY_DIR}/cloc.xml")
	set(CLOC_CMD ${CLOC_BIN} ${CMAKE_SOURCE_DIR})

	add_custom_command(OUTPUT ${CLOC_FILE} COMMAND ${CLOC_CMD} --by-file-by-lang --xml --report-file=${CLOC_FILE})
	add_custom_target(cloc COMMAND ${CLOC_CMD})
	add_custom_target(cloc.report DEPENDS ${CLOC_FILE})
endif()

add_custom_target(format)

find_program(UNCRUSTIFY uncrustify CMAKE_FIND_ROOT_PATH_BOTH)
if(UNCRUSTIFY)
	execute_process(COMMAND ${UNCRUSTIFY} --version OUTPUT_VARIABLE UNCRUSTIFY_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REPLACE "uncrustify " "" UNCRUSTIFY_VERSION ${UNCRUSTIFY_VERSION})
	string(REPLACE "Uncrustify-" "" UNCRUSTIFY_VERSION ${UNCRUSTIFY_VERSION})

	set(UNCRUSTIFY_NEEDED_VERSION "0.80.0")
	if("${UNCRUSTIFY_VERSION}" STRLESS "${UNCRUSTIFY_NEEDED_VERSION}")
		message(WARNING "Uncrustify seems to be too old. Use at least ${UNCRUSTIFY_NEEDED_VERSION}... you are using: ${UNCRUSTIFY_VERSION}")
	else()
		message(STATUS "Found uncrustify ${UNCRUSTIFY_VERSION}")
		list(APPEND GLOB_UNCRUSTIFY ${PROJECT_SOURCE_DIR}/*.java)
		list(APPEND GLOB_UNCRUSTIFY ${PROJECT_SOURCE_DIR}/*.cpp)
		list(APPEND GLOB_UNCRUSTIFY ${PROJECT_SOURCE_DIR}/*.h)
		list(APPEND GLOB_UNCRUSTIFY ${PROJECT_SOURCE_DIR}/*.h.in)
		list(APPEND GLOB_UNCRUSTIFY ${PROJECT_SOURCE_DIR}/*.mm)
		list(APPEND GLOB_UNCRUSTIFY ${PROJECT_SOURCE_DIR}/*.m)
		file(GLOB_RECURSE FILES ${GLOB_UNCRUSTIFY})
		set(FORMATTING_FILE ${PROJECT_BINARY_DIR}/formatting.files)

		file(WRITE ${FORMATTING_FILE} "")
		foreach(file ${FILES})
			if(NOT "${file}" MATCHES "/external/")
				file(APPEND ${FORMATTING_FILE} ${file})
				file(APPEND ${FORMATTING_FILE} "\n")
			endif()
		endforeach()

		set(UNCRUSTIFY_CFG ${PROJECT_SOURCE_DIR}/uncrustify.cfg)
		set(UNCRUSTIFY_CMD ${UNCRUSTIFY} -c ${UNCRUSTIFY_CFG} --replace --no-backup -q -F ${FORMATTING_FILE})
		add_custom_target(format.uncrustify COMMAND ${UNCRUSTIFY_CMD} SOURCES ${UNCRUSTIFY_CFG} ${FILES})
		add_dependencies(format format.uncrustify)
	endif()
endif()

find_program(DOS2UNIX dos2unix CMAKE_FIND_ROOT_PATH_BOTH)
if(DOS2UNIX)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/*.cpp)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/*.h)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/*.rst)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/*.svg)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/CMakeLists.txt)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/*.cmake)
	list(APPEND GLOB_DOS2UNIX ${PROJECT_SOURCE_DIR}/*.cmake.in)
	file(GLOB_RECURSE FILES ${GLOB_DOS2UNIX})

	foreach(FILE ${FILES})
		list(APPEND commands_dos2unix COMMAND ${DOS2UNIX} -q -k --add-eol ${FILE})
	endforeach()

	add_custom_target(format.dos2unix ${commands_dos2unix})
	add_dependencies(format format.dos2unix)
endif()

find_package(Python)
if(Python_FOUND)
	list(APPEND GLOB_JSON ${RESOURCES_DIR}/updatable-files/*.json)
	list(APPEND GLOB_JSON ${TEST_DIR}/json/*.json)
	file(GLOB_RECURSE JSON_FILES ${GLOB_JSON})

	foreach(JSON_FILE ${JSON_FILES})
		list(APPEND commands_json
			COMMAND ${Python_EXECUTABLE} -m json.tool --no-ensure-ascii --tab ${JSON_FILE} ${JSON_FILE})
	endforeach()

	add_custom_target(format.json ${commands_json})
	add_dependencies(format format.json)

	if(EXISTS "${CMAKE_SOURCE_DIR}/utils")
		set(QRCODE_GENERATOR ${CMAKE_SOURCE_DIR}/utils/qrcodeGenerator/main.py)
		set(LANGUAGES de en uk ru)
		foreach(LANGUAGE ${LANGUAGES})
			list(APPEND qrcodes COMMAND ${Python_EXECUTABLE} ${QRCODE_GENERATOR} "https://www.ausweisapp.bund.de/${LANGUAGE}/aa2/download" "${RESOURCES_DIR}/images/desktop/qrcode_download_${LANGUAGE}.png")
		endforeach()
		add_custom_target(qrcodes ${qrcodes})
	endif()
endif()

find_program(RUFF ruff CMAKE_FIND_ROOT_PATH_BOTH)
if(NOT RUFF)
	find_program(UV uv CMAKE_FIND_ROOT_PATH_BOTH)
	if(UV)
		set(RUFF ${UV} tool run ruff)
	endif()
endif()
if(RUFF)
	list(APPEND GLOB_PY ${PROJECT_SOURCE_DIR}/*.py)
	file(GLOB_RECURSE PY_FILES ${GLOB_PY})
	set(RUFF_CMD ${RUFF} format ${CMAKE_SOURCE_DIR})
	add_custom_target(format.ruff COMMAND ${RUFF_CMD} SOURCES ${PROJECT_SOURCE_DIR}/ruff.toml ${PY_FILES})
	add_dependencies(format format.ruff)
endif()

find_program(YAMLFMT yamlfmt CMAKE_FIND_ROOT_PATH_BOTH)
if(YAMLFMT)
	list(APPEND GLOB_YML ${PROJECT_SOURCE_DIR}/*.yml)
	list(APPEND GLOB_YML ${PROJECT_SOURCE_DIR}/*.yaml)
	file(GLOB_RECURSE YML_FILES ${GLOB_YML})

	add_custom_target(format.yamlfmt COMMAND ${YAMLFMT} ${YML_FILES} SOURCES ${YML_FILES} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
	add_dependencies(format format.yamlfmt)
endif()

# doc8 (https://pypi.python.org/pypi/doc8)
find_program(DOC8_BIN doc8 CMAKE_FIND_ROOT_PATH_BOTH)
function(CREATE_DOC8_TARGET _dir _name)
	if(DOC8_BIN)
		add_custom_target(doc8.${_name} COMMAND ${DOC8_BIN} --config ${PROJECT_SOURCE_DIR}/docs/doc8.ini WORKING_DIRECTORY ${_dir})
		if(NOT TARGET doc8)
			add_custom_target(doc8)
		endif()
		add_dependencies(doc8 doc8.${_name})
	endif()
endfunction()

find_program(INKSCAPE inkscape CMAKE_FIND_ROOT_PATH_BOTH)
if(INKSCAPE)
	set(BACKGROUND_COLOR "#ffffff")

	add_custom_target(npaicons.general
		COMMAND ${INKSCAPE} npa_release.svg -d 320 -y 0 -o npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	add_custom_target(npaicons.android.playstore
		COMMAND ${INKSCAPE} playstore_release.svg -w 512 -h 512 -y 0 -o playstore_release.png
		COMMAND ${INKSCAPE} playstore_preview.svg -w 512 -h 512 -y 0 -o playstore_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.background
		COMMAND ${INKSCAPE} adaptive_background.svg -d 120 -y 0 -o ldpi/background_npa.png
		COMMAND ${INKSCAPE} adaptive_background.svg -d 160 -y 0 -o mdpi/background_npa.png
		COMMAND ${INKSCAPE} adaptive_background.svg -d 240 -y 0 -o hdpi/background_npa.png
		COMMAND ${INKSCAPE} adaptive_background.svg -d 320 -y 0 -o xhdpi/background_npa.png
		COMMAND ${INKSCAPE} adaptive_background.svg -d 480 -y 0 -o xxhdpi/background_npa.png
		COMMAND ${INKSCAPE} adaptive_background.svg -d 640 -y 0 -o xxxhdpi/background_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.foreground
		COMMAND ${INKSCAPE} adaptive_foreground_release.svg -d 120 -y 0 -o ldpi/foreground_npa.png
		COMMAND ${INKSCAPE} adaptive_foreground_release.svg -d 160 -y 0 -o mdpi/foreground_npa.png
		COMMAND ${INKSCAPE} adaptive_foreground_release.svg -d 240 -y 0 -o hdpi/foreground_npa.png
		COMMAND ${INKSCAPE} adaptive_foreground_release.svg -d 320 -y 0 -o xhdpi/foreground_npa.png
		COMMAND ${INKSCAPE} adaptive_foreground_release.svg -d 480 -y 0 -o xxhdpi/foreground_npa.png
		COMMAND ${INKSCAPE} adaptive_foreground_release.svg -d 640 -y 0 -o xxxhdpi/foreground_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.foreground.beta
		COMMAND ${INKSCAPE} adaptive_foreground_beta.svg -d 120 -y 0 -o ldpi/foreground_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_foreground_beta.svg -d 160 -y 0 -o mdpi/foreground_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_foreground_beta.svg -d 240 -y 0 -o hdpi/foreground_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_foreground_beta.svg -d 320 -y 0 -o xhdpi/foreground_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_foreground_beta.svg -d 480 -y 0 -o xxhdpi/foreground_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_foreground_beta.svg -d 640 -y 0 -o xxxhdpi/foreground_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.foreground.preview
		COMMAND ${INKSCAPE} adaptive_foreground_preview.svg -d 120 -y 0 -o ldpi/foreground_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_foreground_preview.svg -d 160 -y 0 -o mdpi/foreground_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_foreground_preview.svg -d 240 -y 0 -o hdpi/foreground_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_foreground_preview.svg -d 320 -y 0 -o xhdpi/foreground_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_foreground_preview.svg -d 480 -y 0 -o xxhdpi/foreground_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_foreground_preview.svg -d 640 -y 0 -o xxxhdpi/foreground_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.monochrome
		COMMAND ${INKSCAPE} adaptive_monochrome_release.svg -d 120 -y 0 -o ldpi/monochrome_npa.png
		COMMAND ${INKSCAPE} adaptive_monochrome_release.svg -d 160 -y 0 -o mdpi/monochrome_npa.png
		COMMAND ${INKSCAPE} adaptive_monochrome_release.svg -d 240 -y 0 -o hdpi/monochrome_npa.png
		COMMAND ${INKSCAPE} adaptive_monochrome_release.svg -d 320 -y 0 -o xhdpi/monochrome_npa.png
		COMMAND ${INKSCAPE} adaptive_monochrome_release.svg -d 480 -y 0 -o xxhdpi/monochrome_npa.png
		COMMAND ${INKSCAPE} adaptive_monochrome_release.svg -d 640 -y 0 -o xxxhdpi/monochrome_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.monochrome.beta
		COMMAND ${INKSCAPE} adaptive_monochrome_beta.svg -d 120 -y 0 -o ldpi/monochrome_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_monochrome_beta.svg -d 160 -y 0 -o mdpi/monochrome_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_monochrome_beta.svg -d 240 -y 0 -o hdpi/monochrome_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_monochrome_beta.svg -d 320 -y 0 -o xhdpi/monochrome_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_monochrome_beta.svg -d 480 -y 0 -o xxhdpi/monochrome_npa_beta.png
		COMMAND ${INKSCAPE} adaptive_monochrome_beta.svg -d 640 -y 0 -o xxxhdpi/monochrome_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.adaptive.monochrome.preview
		COMMAND ${INKSCAPE} adaptive_monochrome_preview.svg -d 120 -y 0 -o ldpi/monochrome_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_monochrome_preview.svg -d 160 -y 0 -o mdpi/monochrome_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_monochrome_preview.svg -d 240 -y 0 -o hdpi/monochrome_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_monochrome_preview.svg -d 320 -y 0 -o xhdpi/monochrome_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_monochrome_preview.svg -d 480 -y 0 -o xxhdpi/monochrome_npa_preview.png
		COMMAND ${INKSCAPE} adaptive_monochrome_preview.svg -d 640 -y 0 -o xxxhdpi/monochrome_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(npaicons.android.launchimage
		COMMAND ${INKSCAPE} npa_release.svg -w 120 -h 120 -y 0 -o android/ldpi/splash_npa.png
		COMMAND ${INKSCAPE} npa_release.svg -w 180 -h 180 -y 0 -o android/mdpi/splash_npa.png
		COMMAND ${INKSCAPE} npa_release.svg -w 270 -h 270 -y 0 -o android/hdpi/splash_npa.png
		COMMAND ${INKSCAPE} npa_release.svg -w 360 -h 360 -y 0 -o android/xhdpi/splash_npa.png
		COMMAND ${INKSCAPE} npa_release.svg -w 540 -h 540 -y 0 -o android/xxhdpi/splash_npa.png
		COMMAND ${INKSCAPE} npa_release.svg -w 720 -h 720 -y 0 -o android/xxxhdpi/splash_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	add_custom_target(npaicons.android.launchimage.beta
		COMMAND ${INKSCAPE} npa_beta.svg -w 120 -h 120 -y 0 -o android/ldpi/splash_npa_beta.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 180 -h 180 -y 0 -o android/mdpi/splash_npa_beta.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 270 -h 270 -y 0 -o android/hdpi/splash_npa_beta.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 360 -h 360 -y 0 -o android/xhdpi/splash_npa_beta.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 540 -h 540 -y 0 -o android/xxhdpi/splash_npa_beta.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 720 -h 720 -y 0 -o android/xxxhdpi/splash_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	add_custom_target(npaicons.android.launchimage.preview
		COMMAND ${INKSCAPE} npa_preview.svg -w 120 -h 120 -y 0 -o android/ldpi/splash_npa_preview.png
		COMMAND ${INKSCAPE} npa_preview.svg -w 180 -h 180 -y 0 -o android/mdpi/splash_npa_preview.png
		COMMAND ${INKSCAPE} npa_preview.svg -w 270 -h 270 -y 0 -o android/hdpi/splash_npa_preview.png
		COMMAND ${INKSCAPE} npa_preview.svg -w 360 -h 360 -y 0 -o android/xhdpi/splash_npa_preview.png
		COMMAND ${INKSCAPE} npa_preview.svg -w 540 -h 540 -y 0 -o android/xxhdpi/splash_npa_preview.png
		COMMAND ${INKSCAPE} npa_preview.svg -w 720 -h 720 -y 0 -o android/xxxhdpi/splash_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	add_custom_target(npaicons.ios.launchimage
		COMMAND ${INKSCAPE} npa_release.svg -w 256 -h 256 -y 0 -o Images.xcassets/LaunchImage.imageset/launchImage.png
		COMMAND ${INKSCAPE} npa_release.svg -w 512 -h 512 -y 0 -o Images.xcassets/LaunchImage.imageset/launchImage@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 768 -h 768 -y 0 -o Images.xcassets/LaunchImage.imageset/launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons)

	add_custom_target(npaicons.ios.launchimage.beta
		COMMAND ${INKSCAPE} npa_beta.svg -w 256 -h 256 -y 0 -o beta/Images.xcassets/LaunchImage.imageset/launchImage.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 512 -h 512 -y 0 -o beta/Images.xcassets/LaunchImage.imageset/launchImage@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 768 -h 768 -y 0 -o beta/Images.xcassets/LaunchImage.imageset/launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons)

	add_custom_target(npaicons.ios
		COMMAND ${INKSCAPE} npa_release.svg -w 20 -h 20 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${INKSCAPE} npa_release.svg -w 40 -h 40 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 60 -h 60 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 29 -h 29 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${INKSCAPE} npa_release.svg -w 58 -h 58 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 87 -h 87 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 40 -h 40 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${INKSCAPE} npa_release.svg -w 80 -h 80 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 120 -h 120 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 120 -h 120 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 180 -h 180 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 76 -h 76 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${INKSCAPE} npa_release.svg -w 152 -h 152 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 167 -h 167 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${INKSCAPE} npa_release.svg -w 1024 -h 1024 -y 1 -b '${BACKGROUND_COLOR}' -o Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons)

	add_custom_target(npaicons.ios.beta
		COMMAND ${INKSCAPE} npa_beta.svg -w 20 -h 20 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon20.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 40 -h 40 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon20@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 60 -h 60 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon20@3x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 29 -h 29 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/iconSmall.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 58 -h 58 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/iconSmall@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 87 -h 87 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/iconSmall@3x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 40 -h 40 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/iconSmall40.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 80 -h 80 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/iconSmall40@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 120 -h 120 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/iconSmall40@3x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 120 -h 120 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon60@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 180 -h 180 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon60@3x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 76 -h 76 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon76.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 152 -h 152 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon76@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 167 -h 167 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon83.5@2x.png
		COMMAND ${INKSCAPE} npa_beta.svg -w 1024 -h 1024 -y 1 -b '${BACKGROUND_COLOR}' -o beta/Images.xcassets/AppIcon.appiconset/icon1024.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons)

	add_custom_target(npaicons DEPENDS
			npaicons.general
			npaicons.android.playstore
			npaicons.android.adaptive.background
			npaicons.android.adaptive.foreground
			npaicons.android.adaptive.foreground.beta
			npaicons.android.adaptive.foreground.preview
			npaicons.android.adaptive.monochrome
			npaicons.android.adaptive.monochrome.beta
			npaicons.android.adaptive.monochrome.preview
			npaicons.android.launchimage
			npaicons.android.launchimage.beta
			npaicons.android.launchimage.preview
			npaicons.ios.launchimage
			npaicons.ios.launchimage.beta
			npaicons.ios
			npaicons.ios.beta)

	add_custom_target(readerimages
		COMMAND ${INKSCAPE} img_RemoteReader.svg -w 512 -h 512 -y 0 -o ${RESOURCES_DIR}/updatable-files/reader/img_RemoteReader.png
		COMMAND ${INKSCAPE} img_PersoSim.svg -w 512 -h 512 -y 0 -o ${RESOURCES_DIR}/updatable-files/reader/img_PersoSim.png
		COMMAND ${INKSCAPE} img_Simulator.svg -w 512 -h 512 -y 0 -o ${RESOURCES_DIR}/updatable-files/reader/img_Simulator.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/src)
endif()

find_program(PNGQUANT pngquant CMAKE_FIND_ROOT_PATH_BOTH)
if(PNGQUANT)
	set(PNGQUANT_CMD pngquant -f -o)

	add_custom_target(pngquant.general
		COMMAND ${PNGQUANT_CMD} npa.png -- npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)

	add_custom_target(pngquant.android.adaptive.background
		COMMAND ${PNGQUANT_CMD} ldpi/background_npa.png -- ldpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/background_npa.png -- mdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/background_npa.png -- hdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/background_npa.png -- xhdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/background_npa.png -- xxhdpi/background_npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/background_npa.png -- xxxhdpi/background_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.adaptive.foreground
		COMMAND ${PNGQUANT_CMD} ldpi/foreground_npa.png -- ldpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/foreground_npa.png -- mdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/foreground_npa.png -- hdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/foreground_npa.png -- xhdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/foreground_npa.png -- xxhdpi/foreground_npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/foreground_npa.png -- xxxhdpi/foreground_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.adaptive.foreground.beta
		COMMAND ${PNGQUANT_CMD} ldpi/foreground_npa_beta.png -- ldpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} mdpi/foreground_npa_beta.png -- mdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} hdpi/foreground_npa_beta.png -- hdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xhdpi/foreground_npa_beta.png -- xhdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/foreground_npa_beta.png -- xxhdpi/foreground_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/foreground_npa_beta.png -- xxxhdpi/foreground_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.adaptive.foreground.preview
		COMMAND ${PNGQUANT_CMD} ldpi/foreground_npa_preview.png -- ldpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} mdpi/foreground_npa_preview.png -- mdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} hdpi/foreground_npa_preview.png -- hdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xhdpi/foreground_npa_preview.png -- xhdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/foreground_npa_preview.png -- xxhdpi/foreground_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/foreground_npa_preview.png -- xxxhdpi/foreground_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.adaptive.monochrome
		COMMAND ${PNGQUANT_CMD} ldpi/monochrome_npa.png -- ldpi/monochrome_npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/monochrome_npa.png -- mdpi/monochrome_npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/monochrome_npa.png -- hdpi/monochrome_npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/monochrome_npa.png -- xhdpi/monochrome_npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/monochrome_npa.png -- xxhdpi/monochrome_npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/monochrome_npa.png -- xxxhdpi/monochrome_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.adaptive.monochrome.beta
		COMMAND ${PNGQUANT_CMD} ldpi/monochrome_npa_beta.png -- ldpi/monochrome_npa_beta.png
		COMMAND ${PNGQUANT_CMD} mdpi/monochrome_npa_beta.png -- mdpi/monochrome_npa_beta.png
		COMMAND ${PNGQUANT_CMD} hdpi/monochrome_npa_beta.png -- hdpi/monochrome_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xhdpi/monochrome_npa_beta.png -- xhdpi/monochrome_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/monochrome_npa_beta.png -- xxhdpi/monochrome_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/monochrome_npa_beta.png -- xxxhdpi/monochrome_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.adaptive.monochrome.preview
		COMMAND ${PNGQUANT_CMD} ldpi/monochrome_npa_preview.png -- ldpi/monochrome_npa_preview.png
		COMMAND ${PNGQUANT_CMD} mdpi/monochrome_npa_preview.png -- mdpi/monochrome_npa_preview.png
		COMMAND ${PNGQUANT_CMD} hdpi/monochrome_npa_preview.png -- hdpi/monochrome_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xhdpi/monochrome_npa_preview.png -- xhdpi/monochrome_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/monochrome_npa_preview.png -- xxhdpi/monochrome_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/monochrome_npa_preview.png -- xxxhdpi/monochrome_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.launchimage
		COMMAND ${PNGQUANT_CMD} ldpi/splash_npa.png -- ldpi/splash_npa.png
		COMMAND ${PNGQUANT_CMD} mdpi/splash_npa.png -- mdpi/splash_npa.png
		COMMAND ${PNGQUANT_CMD} hdpi/splash_npa.png -- hdpi/splash_npa.png
		COMMAND ${PNGQUANT_CMD} xhdpi/splash_npa.png -- xhdpi/splash_npa.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/splash_npa.png -- xxhdpi/splash_npa.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/splash_npa.png -- xxxhdpi/splash_npa.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.launchimage.beta
		COMMAND ${PNGQUANT_CMD} ldpi/splash_npa_beta.png -- ldpi/splash_npa_beta.png
		COMMAND ${PNGQUANT_CMD} mdpi/splash_npa_beta.png -- mdpi/splash_npa_beta.png
		COMMAND ${PNGQUANT_CMD} hdpi/splash_npa_beta.png -- hdpi/splash_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xhdpi/splash_npa_beta.png -- xhdpi/splash_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/splash_npa_beta.png -- xxhdpi/splash_npa_beta.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/splash_npa_beta.png -- xxxhdpi/splash_npa_beta.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.android.launchimage.preview
		COMMAND ${PNGQUANT_CMD} ldpi/splash_npa_preview.png -- ldpi/splash_npa_preview.png
		COMMAND ${PNGQUANT_CMD} mdpi/splash_npa_preview.png -- mdpi/splash_npa_preview.png
		COMMAND ${PNGQUANT_CMD} hdpi/splash_npa_preview.png -- hdpi/splash_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xhdpi/splash_npa_preview.png -- xhdpi/splash_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxhdpi/splash_npa_preview.png -- xxhdpi/splash_npa_preview.png
		COMMAND ${PNGQUANT_CMD} xxxhdpi/splash_npa_preview.png -- xxxhdpi/splash_npa_preview.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/android)

	add_custom_target(pngquant.ios.launchimage
		COMMAND ${PNGQUANT_CMD} launchImage.png -- launchImage.png
		COMMAND ${PNGQUANT_CMD} launchImage@2x.png -- launchImage@2x.png
		COMMAND ${PNGQUANT_CMD} launchImage@3x.png -- launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons/Images.xcassets/LaunchImage.imageset)

	add_custom_target(pngquant.ios.launchimage.beta
		COMMAND ${PNGQUANT_CMD} launchImage.png -- launchImage.png
		COMMAND ${PNGQUANT_CMD} launchImage@2x.png -- launchImage@2x.png
		COMMAND ${PNGQUANT_CMD} launchImage@3x.png -- launchImage@3x.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons/beta/Images.xcassets/LaunchImage.imageset)

	add_custom_target(pngquant.ios
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
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons/Images.xcassets/AppIcon.appiconset)

	add_custom_target(pngquant.ios.beta
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
		WORKING_DIRECTORY ${RESOURCES_DIR}/images/ios/appIcons/beta/Images.xcassets/AppIcon.appiconset)

	add_custom_target(pngquant DEPENDS
			pngquant.general
			pngquant.android.adaptive.background
			pngquant.android.adaptive.foreground
			pngquant.android.adaptive.foreground.beta
			pngquant.android.adaptive.foreground.preview
			pngquant.android.adaptive.monochrome
			pngquant.android.adaptive.monochrome.beta
			pngquant.android.adaptive.monochrome.preview
			pngquant.android.launchimage
			pngquant.android.launchimage.beta
			pngquant.android.launchimage.preview
			pngquant.ios.launchimage
			pngquant.ios.launchimage.beta
			pngquant.ios
			pngquant.ios.beta)

	add_custom_target(pngquant.readerimages
		COMMAND ${PNGQUANT_CMD} img_RemoteReader.png -- img_RemoteReader.png
		COMMAND ${PNGQUANT_CMD} img_PersoSim.png -- img_PersoSim.png
		COMMAND ${PNGQUANT_CMD} img_Simulator.png -- img_Simulator.png
		WORKING_DIRECTORY ${RESOURCES_DIR}/updatable-files/reader)
endif()

find_program(CONVERT convert CMAKE_FIND_ROOT_PATH_BOTH)
if(CONVERT)
	add_custom_target(npaicons.win
		COMMAND ${CONVERT} -background transparent -define icon:auto-resize=256,96,64,48,40,32,24,20,16 npa.svg desktop/npa.ico
		WORKING_DIRECTORY ${RESOURCES_DIR}/images)
endif()

if(INKSCAPE AND APPLE AND NOT IOS)
	find_program(ICONUTIL iconutil)
	if(ICONUTIL)
		set(BUNDLE_ICON_SET_DIR ${CMAKE_CURRENT_BINARY_DIR}/bundle_icons.iconset)
		add_custom_target(npaicons.mac
			COMMAND ${CMAKE_COMMAND} -E make_directory ${BUNDLE_ICON_SET_DIR}
			COMMAND ${INKSCAPE} appIcon.svg -w 16 -h 16 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_16x16.png
			COMMAND ${INKSCAPE} appIcon.svg -w 32 -h 32 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_16x16@2x.png
			COMMAND ${INKSCAPE} appIcon.svg -w 32 -h 32 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_32x32.png
			COMMAND ${INKSCAPE} appIcon.svg -w 64 -h 64 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_32x32@2x.png
			COMMAND ${INKSCAPE} appIcon.svg -w 128 -h 128 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_128x128.png
			COMMAND ${INKSCAPE} appIcon.svg -w 256 -h 256 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_128x128@2x.png
			COMMAND ${INKSCAPE} appIcon.svg -w 256 -h 256 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_256x256.png
			COMMAND ${INKSCAPE} appIcon.svg -w 512 -h 512 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_256x256@2x.png
			COMMAND ${INKSCAPE} appIcon.svg -w 512 -h 512 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_512x512.png
			COMMAND ${INKSCAPE} appIcon.svg -w 1024 -h 1024 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_512x512@2x.png
			COMMAND ${ICONUTIL} -c icns --output "${PROJECT_NAME}.icns" ${BUNDLE_ICON_SET_DIR}
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUNDLE_ICON_SET_DIR}
			WORKING_DIRECTORY ${RESOURCES_DIR}/images/macos)

		add_custom_target(npaicons.mac.beta
			COMMAND ${CMAKE_COMMAND} -E make_directory ${BUNDLE_ICON_SET_DIR}
			COMMAND ${INKSCAPE} appIconBeta.svg -w 16 -h 16 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_16x16.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 32 -h 32 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_16x16@2x.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 32 -h 32 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_32x32.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 64 -h 64 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_32x32@2x.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 128 -h 128 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_128x128.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 256 -h 256 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_128x128@2x.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 256 -h 256 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_256x256.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 512 -h 512 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_256x256@2x.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 512 -h 512 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_512x512.png
			COMMAND ${INKSCAPE} appIconBeta.svg -w 1024 -h 1024 -y 0 -o ${BUNDLE_ICON_SET_DIR}/icon_512x512@2x.png
			COMMAND ${ICONUTIL} -c icns --output "beta/${PROJECT_NAME}.icns" ${BUNDLE_ICON_SET_DIR}
			COMMAND ${CMAKE_COMMAND} -E remove_directory ${BUNDLE_ICON_SET_DIR}
			WORKING_DIRECTORY ${RESOURCES_DIR}/images/macos)
	endif()
endif()

find_program(PLANTUML plantuml)
if(PLANTUML)
	message(STATUS "Target uml.statemachines is available using: ${PLANTUML}")
	configure_file(${RESOURCES_DIR}/statemachine.sh.in ${PROJECT_BINARY_DIR}/statemachine.sh @ONLY)
	add_custom_target(uml.statemachines COMMAND ./statemachine.sh WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()

find_program(DOT dot CMAKE_FIND_ROOT_PATH_BOTH)
if(DOT)
	set(architecture_file Architecture)
	set(ARCHI_PDF_DEPENDS)

	add_custom_target(architecture.graphviz ${CMAKE_COMMAND} --graphviz=${architecture_file} . WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
	list(APPEND ARCHI_PDF_DEPENDS architecture.graphviz)

	find_program(SED sed CMAKE_FIND_ROOT_PATH_BOTH)
	if(SED)
		# 1. Strip line of plugins as it is misleading
		# 2. Strip "AusweisApp" prefix
		add_custom_target(architecture.sed
			COMMAND ${SED} -i -E '/AusweisAppBinary -> AusweisApp\(Ui|Card|Activation\).+/d' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/"AusweisApp"/"REPLACE"/' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/AusweisApp//' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/"REPLACE"/"AusweisApp"/' ${architecture_file}
			COMMAND ${SED} -i'' -e 's/diamond/box/' ${architecture_file}
			DEPENDS ${ARCHI_PDF_DEPENDS}
			WORKING_DIRECTORY ${PROJECT_BINARY_DIR})

		list(APPEND ARCHI_PDF_DEPENDS architecture.sed)
	endif()

	add_custom_target(architecture
		${DOT} -O -Tpdf ${architecture_file}
		DEPENDS ${ARCHI_PDF_DEPENDS}
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endif()

include(Sphinx)
include(Tools.Libraries)
