cmake_minimum_required(VERSION 3.25)

set(CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${CMAKE_DIR}")

option(PROVIDER "Enable dependency provider" OFF)
if(PROVIDER)
	set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_DIR}/Provider.cmake")
endif()

include(Policies NO_POLICY_SCOPE)

# "tools.only" can be defined to disable the normal build and enable
# cmdline "tools" only. For example: "make format" or "make package_source"
if(tools.only)
	set(LANGUAGES NONE)
else()
	set(LANGUAGES CXX)
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
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES ${LANGUAGES})

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
	if(ANDROID_VERSION_CODE_BASE)
		if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
			set(COUNTER 0)
		elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
			set(COUNTER 1)
		elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
			set(COUNTER 2)
		else()
			message(FATAL_ERROR "Unknown architecture: ${CMAKE_ANDROID_ARCH_ABI}")
		endif()
		math(EXPR ANDROID_VERSION_CODE "${ANDROID_VERSION_CODE_BASE} + ${COUNTER}")
	endif()
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
set(COPYRIGHT_TEXT "2014-2026 ${VENDOR}")
if(APPLE)
	string(REPLACE " \& " " \&amp; " COPYRIGHT_TEXT ${COPYRIGHT_TEXT})
endif()
set(BUNDLE_IDENTIFIER com.governikus.ausweisapp2)

if(NOT CONTAINER_SDK)
	set(VALGRIND_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --show-leak-kinds=definite --errors-for-leak-kinds=definite --track-origins=yes --error-exitcode=1")
	set(VALGRIND_SUPPRESSIONS "${CMAKE_PREFIX_PATH}/test/valgrind.supp")
	if(EXISTS "${VALGRIND_SUPPRESSIONS}")
		message(STATUS "Using valgrind suppressions: ${VALGRIND_SUPPRESSIONS}")
		set(VALGRIND_COMMAND_OPTIONS "${VALGRIND_COMMAND_OPTIONS} --gen-suppressions=all --suppressions=${VALGRIND_SUPPRESSIONS}")
	endif()

	include(CTest)
endif()

include(Tools)
include(DVCS)
add_subdirectory(docs)

include(Messages)
if(tools.only)
	include(Packaging)
	return()
endif()

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
if(VENDOR_GOVERNIKUS AND LIBS_GOVERNIKUS)
	include(SBOM)
endif()

if(BUILD_TESTING)
	configure_file("${CMAKE_DIR}/CTestCustom.cmake.in" "${CMAKE_BINARY_DIR}/CTestCustom.cmake" @ONLY)

	if(EXISTS "${PROJECT_SOURCE_DIR}/ci")
		configure_file("${PROJECT_SOURCE_DIR}/ci/sonar-project.properties.in" "${CMAKE_BINARY_DIR}/sonar-project.properties" @ONLY)
	endif()
endif()

include(FeatureSummary)
FEATURE_SUMMARY(WHAT ALL)
