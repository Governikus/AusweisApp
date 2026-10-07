cmake_minimum_required(VERSION 3.25)

find_program(XCRUN xcrun)
if(NOT XCRUN)
	message(FATAL_ERROR "Cannot find 'xcrun'")
endif()

set(APP_PATH ${CPACK_TEMPORARY_DIRECTORY}/${CPACK_PACKAGE_NAME}.app)
set(PKG_PATH ${CPACK_PACKAGE_DIRECTORY}/${CPACK_PACKAGE_FILE_NAME}.pkg)

file(GLOB_RECURSE TEST_FILES "${APP_PATH}/*test*")
if(TEST_FILES)
	message(FATAL_ERROR "Bundle contains test files: ${TEST_FILES}")
endif()

file(GLOB_RECURSE ALL_DIRS LIST_DIRECTORIES TRUE "${APP_PATH}/*")
foreach(dir IN LISTS ALL_DIRS)
	if(IS_DIRECTORY "${dir}" AND dir MATCHES "\\.dSYM$")
		message(FATAL_ERROR "Bundle contains dSYM dir: ${dir}")
	endif()
endforeach()

execute_process(COMMAND ${XCRUN} productbuild --sign "3rd Party Mac Developer Installer: Governikus Service GmbH (G7EQCJU4BR)" --component ${APP_PATH} /Applications ${PKG_PATH})
