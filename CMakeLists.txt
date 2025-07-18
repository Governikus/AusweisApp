cmake_minimum_required(VERSION 3.19.0)

set(CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${CMAKE_DIR}")

include(Policies NO_POLICY_SCOPE)

# "tools.only" can be defined to disable the normal build and enable
# cmdline "tools" only. For example: "make format" or "make package_source"
if(tools.only)
	set(LANGUAGES NONE)
else()
	set(LANGUAGES CXX)
endif()

if(UNIX AND NOT IOS)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0 CACHE STRING "Required macOS version")
endif()

if(CMAKE_BUILD_TYPE)
	string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
else()
	set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "build type configuration" FORCE)
endif()

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(isMultiConfig)
	if(NOT CMAKE_CONFIGURATION_TYPES)
		set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE})
	endif()

	list(LENGTH CMAKE_CONFIGURATION_TYPES multiConfigCount)
	if(NOT multiConfigCount EQUAL 1)
		message(FATAL_ERROR "Multi-Config builds are not supported: QTBUG-123340")
	endif()
endif()

include(Version)
project(AusweisApp VERSION ${VERSION} LANGUAGES ${LANGUAGES})

if(ANDROID AND NOT GOVERNIKUS_TOOLCHAIN_FILE)
	message(FATAL_ERROR "-DCMAKE_TOOLCHAIN_FILE=${CMAKE_DIR}/android.toolchain.cmake is required")
endif()

# Set TWEAK if not defined in PROJECT_VERSION above to
# have a valid tweak version without propagating it
if(NOT PROJECT_VERSION_TWEAK)
	set(PROJECT_VERSION_TWEAK 0)
endif()

if(APPLE AND NOT tools.only)
	enable_language(OBJCXX)
endif()

option(BUILD_SHARED_LIBS "Enable build of shared libraries")
option(INTEGRATED_SDK "Build platform specific SDK" OFF)
option(CONTAINER_SDK "Build container specific SDK" OFF)

if(CONTAINER_SDK)
	set(INTEGRATED_SDK ON)
endif()

include(Helper)

if(NOT VENDOR)
	if(EXISTS "${CMAKE_DIR}/Vendor.internal.cmake")
		include(Vendor.internal)
	else()
		set(VENDOR "") # Qt uses Organization for paths
	endif()
endif()
if(VENDOR MATCHES "Governikus")
	set(VENDOR_GOVERNIKUS TRUE)
	set(LICENSE_FILE "${PROJECT_SOURCE_DIR}/LICENSE.officially.txt")
else()
	set(LICENSE_FILE "${PROJECT_SOURCE_DIR}/LICENSE.txt")
endif()

message(STATUS "VENDOR: ${VENDOR}")
message(STATUS "VERSION: ${PROJECT_VERSION}")

if(ANDROID)
	if(NOT BUILD_PREVIEW)
		set(BUILD_PREVIEW false)
	endif()
	message(STATUS "BUILD_PREVIEW: ${BUILD_PREVIEW}")

	if(NOT ANDROID_VERSION_CODE)
		set(ANDROID_VERSION_CODE 0)
	endif()
	message(STATUS "ANDROID_VERSION_CODE: ${ANDROID_VERSION_CODE}")
endif()

if(IOS OR MAC)
	if(NOT USE_DISTRIBUTION_PROFILE)
		set(USE_DISTRIBUTION_PROFILE false)
	endif()
	message(STATUS "USE_DISTRIBUTION_PROFILE: ${USE_DISTRIBUTION_PROFILE}")
endif()

if("${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}" AND NOT FORCE_SOURCE_BUILD)
	message(FATAL_ERROR "in tree building is not supported!")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(DOCS_DIR ${PROJECT_SOURCE_DIR}/docs)
set(RESOURCES_DIR ${PROJECT_SOURCE_DIR}/resources)
set(PACKAGING_DIR ${RESOURCES_DIR}/packaging)
set(COPYRIGHT_TEXT "2014-2025 ${VENDOR}")
if(APPLE)
	string(REPLACE " \& " " \&amp; " COPYRIGHT_TEXT ${COPYRIGHT_TEXT})
endif()
set(BUNDLE_IDENTIFIER com.governikus.ausweisapp2)

if(NOT CONTAINER_SDK)
	include(CTest)
endif()

include(Tools)
include(DVCS)
add_subdirectory(docs)

include(Appcast)
include(Messages)
if(tools.only)
	include(Packaging)
	return()
endif()

option(USE_SMARTEID "Enable Smart-eID" OFF)

include(Libraries)
include(PoliciesQt NO_POLICY_SCOPE)
include(Tools.Libraries)
include(CompilerFlags)

if(LINUX OR BSD)
	include(GNUInstallDirs)
endif()

add_subdirectory(resources)
add_subdirectory(src)
if(BUILD_TESTING)
	add_subdirectory(test)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils")
	add_subdirectory(utils)
endif()

include(Packaging)

if(BUILD_TESTING)
	set(VALGRIND_SUPPRESSIONS "${CMAKE_PREFIX_PATH}/test/valgrind.supp")
	if(NOT EXISTS "${VALGRIND_SUPPRESSIONS}")
		set(VALGRIND_SUPPRESSIONS "${CMAKE_SOURCE_DIR}/libs/test/valgrind.supp")
	endif()
	message(STATUS "Using valgrind suppressions: ${VALGRIND_SUPPRESSIONS}")
	set(VALGRIND_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --show-leak-kinds=definite --errors-for-leak-kinds=definite --error-exitcode=1 --gen-suppressions=all --suppressions=${VALGRIND_SUPPRESSIONS}")

	configure_file("${CMAKE_DIR}/CTestCustom.cmake.in" "${CMAKE_BINARY_DIR}/CTestCustom.cmake" @ONLY)

	set(SONAR_CACHE_DIR ${CMAKE_BINARY_DIR}/../cache)
	if(NOT EXISTS "${SONAR_CACHE_DIR}")
		set(SONAR_CACHE_DIR ${CMAKE_BINARY_DIR})
	endif()
	get_filename_component(SONAR_CACHE_DIR ${SONAR_CACHE_DIR} ABSOLUTE)
	configure_file("${RESOURCES_DIR}/sonar-project.properties.in" "${CMAKE_BINARY_DIR}/sonar-project.properties" @ONLY)
endif()

include(FeatureSummary)
FEATURE_SUMMARY(WHAT ALL)
