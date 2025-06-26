cmake_minimum_required(VERSION 3.14)

if(NOT CMAKE_SCRIPT_MODE_FILE)
	message(STATUS "Usage: cmake -P cmake/SwiftPackage.cmake")
endif()

find_program(XCODEBUILD xcodebuild)
if(NOT XCODEBUILD)
	message(FATAL_ERROR "Cannot find xcodebuild")
endif()

find_program(LIPO lipo)
if(NOT LIPO)
	message(FATAL_ERROR "Cannot find lipo")
endif()


set(prefix AusweisApp2)
file(GLOB_RECURSE FRAMEWORK_ZIP "${CMAKE_BINARY_DIR}/*.framework.zip")
if(NOT FRAMEWORK_ZIP)
	message(FATAL_ERROR "Missing framework(s) in: ${CMAKE_BINARY_DIR}")
endif()


foreach(framework ${FRAMEWORK_ZIP})
	message(STATUS "Check framework: ${framework}")

	# Check that any framework has same version
	get_filename_component(framework_version ${framework} NAME)
	get_filename_component(framework_version ${framework_version} NAME_WLE) # strip .zip
	get_filename_component(framework_version ${framework_version} NAME_WLE) # strip .framework
	get_filename_component(framework_arch ${framework_version} LAST_EXT)
	get_filename_component(framework_version ${framework_version} NAME_WLE) # strip framework_arch
	string(REPLACE "${prefix}-" "" framework_version "${framework_version}") # strip AusweisApp

	if(APP_NAME_VERSION AND NOT APP_NAME_VERSION STREQUAL framework_version)
		message(STATUS "Different package version: ${APP_NAME_VERSION} / ${framework_version}")
		set(APP_MISMATCH -version-mismatch)
	else()
		set(APP_NAME_VERSION ${framework_version})
	endif()

	# Extract the .zip file to package the framework itself
	set(framework_arch_dir ${CMAKE_BINARY_DIR}/arch${framework_arch})
	set(framework_dir ${framework_arch_dir}/${prefix}.framework)
	file(REMOVE_RECURSE ${framework_arch_dir})
	file(MAKE_DIRECTORY ${framework_arch_dir})
	execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf "${framework}" WORKING_DIRECTORY "${framework_arch_dir}")
	if (NOT ${framework_arch} MATCHES simulator)
		list(APPEND FRAMEWORK_PARAM -framework ${framework_dir})
	else()
		if (NOT FAT_LIB_TARGETS)
			list(APPEND FRAMEWORK_PARAM -framework ${framework_dir})
			set(FAT_LIB_OUTPUT ${framework_dir}/${prefix})
		endif()
		list(APPEND FAT_LIB_TARGETS -create ${framework_dir}/${prefix})
	endif()
endforeach()

if(FAT_LIB_TARGETS)
	execute_process(COMMAND ${LIPO} ${FAT_LIB_TARGETS} -output ${FAT_LIB_OUTPUT})
endif()

file(REMOVE_RECURSE ${prefix}.xcframework)
execute_process(COMMAND ${XCODEBUILD} -create-xcframework ${FRAMEWORK_PARAM} -output ${prefix}.xcframework)

get_filename_component(SCRIPT_DIR "${CMAKE_SCRIPT_MODE_FILE}" DIRECTORY)
set(PACKAGING_DIR "${SCRIPT_DIR}/../resources/packaging")
configure_file("${PACKAGING_DIR}/ios/Package.swift" "${CMAKE_BINARY_DIR}/Package.swift" COPYONLY)

if(DIST_DIR)
	get_filename_component(DIST_DIR "${DIST_DIR}" ABSOLUTE)
else()
	set(DIST_DIR ${CMAKE_BINARY_DIR}/dist)
endif()

if(NOT EXISTS "${DIST_DIR}")
	file(MAKE_DIRECTORY ${DIST_DIR})
endif()

set(dist_filename ${DIST_DIR}/${prefix}-${APP_NAME_VERSION}${APP_MISMATCH}.swiftpackage.zip)
message(STATUS "Create package: ${dist_filename}")
execute_process(COMMAND ${CMAKE_COMMAND} -E tar cf "${dist_filename}" --format=zip ${prefix}.xcframework Package.swift)
