cmake_minimum_required(VERSION 3.13.0)

if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0023)
	cmake_policy(SET CMP0023 NEW)
endif()

if(POLICY CMP0046)
	cmake_policy(SET CMP0046 NEW)
endif()

if(POLICY CMP0048)
	cmake_policy(SET CMP0048 NEW)
endif()

if(POLICY CMP0054)
	cmake_policy(SET CMP0054 NEW)
endif()

if(POLICY CMP0063)
	cmake_policy(SET CMP0063 NEW)
endif()

if(POLICY CMP0071)
	cmake_policy(SET CMP0071 NEW)
endif()

if(POLICY CMP0072)
	cmake_policy(SET CMP0072 NEW)
endif()

if(POLICY CMP0074)
	cmake_policy(SET CMP0074 NEW)
endif()

if(POLICY CMP0076)
	cmake_policy(SET CMP0076 NEW)
endif()

if(POLICY CMP0092)
	cmake_policy(SET CMP0092 NEW)
endif()

# "tools.only" can be defined to disable the normal build and enable
# cmdline "tools" only. For example: "make format" or "make package_source"
if(tools.only)
	set(LANGUAGES NONE)
else()
	set(LANGUAGES CXX)
endif()

if(UNIX AND NOT IOS)
	set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15 CACHE STRING "Required macOS version")
endif()

project(AusweisApp2 VERSION 1.24.4 LANGUAGES ${LANGUAGES})

# Set TWEAK if not defined in PROJECT_VERSION above to
# have a valid tweak version without propagating it
if(NOT PROJECT_VERSION_TWEAK)
	set(PROJECT_VERSION_TWEAK 0)
endif()

if(APPLE AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.16" AND NOT tools.only)
	enable_language(OBJCXX)
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT AND (IOS OR ANDROID))
	set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/dist" CACHE PATH "default install path" FORCE)
endif()
set(CMAKE_DIR "${PROJECT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH "${CMAKE_DIR}")
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
	elseif(LINUX OR BSD)
		set(VENDOR "") # Qt uses Organization for paths
	else()
		set(VENDOR AusweisApp2_CE) # CommunityEdition
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

if(IOS)
	if(NOT USE_DISTRIBUTION_PROFILE)
		set(USE_DISTRIBUTION_PROFILE false)
	endif()
	message(STATUS "USE_DISTRIBUTION_PROFILE: ${USE_DISTRIBUTION_PROFILE}")
endif()

if("${PROJECT_BINARY_DIR}" STREQUAL "${PROJECT_SOURCE_DIR}" AND NOT FORCE_SOURCE_BUILD)
	message(FATAL_ERROR "in tree building is not supported!")
endif()

if(CMAKE_BUILD_TYPE)
	string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
else()
	set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "build type configuration" FORCE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(DOCS_DIR ${PROJECT_SOURCE_DIR}/docs)
set(RESOURCES_DIR ${PROJECT_SOURCE_DIR}/resources)
set(PACKAGING_DIR ${RESOURCES_DIR}/packaging)
set(COPYRIGHT_TEXT "2014-2022 ${VENDOR}")
if(APPLE)
	string(REPLACE " \& " " \&amp; " COPYRIGHT_TEXT ${COPYRIGHT_TEXT})
endif()
set(BUNDLE_IDENTIFIER com.governikus.ausweisapp2)

include(Tools)
include(DVCS)
add_subdirectory(docs)

include(Appcast)
include(Messages)
if(tools.only)
	include(Packaging)
	return()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" AND NOT CONTAINER_SDK)
	set(VALGRIND_SUPPRESSIONS "${CMAKE_PREFIX_PATH}/test/valgrind.supp")
	if(NOT EXISTS "${VALGRIND_SUPPRESSIONS}")
		set(VALGRIND_SUPPRESSIONS "${CMAKE_SOURCE_DIR}/libs/test/valgrind.supp")
	endif()
	message(STATUS "Using valgrind suppressions: ${VALGRIND_SUPPRESSIONS}")
	set(VALGRIND_COMMAND_OPTIONS "--tool=memcheck --leak-check=full --show-leak-kinds=definite --errors-for-leak-kinds=definite --error-exitcode=1 --gen-suppressions=all --suppressions=${VALGRIND_SUPPRESSIONS}")

	include(CTest)
	configure_file("${CMAKE_DIR}/CTestCustom.cmake.in" "${CMAKE_BINARY_DIR}/CTestCustom.cmake" @ONLY)

	set(SONAR_CACHE_DIR ${CMAKE_BINARY_DIR}/../cache)
	if(NOT EXISTS "${SONAR_CACHE_DIR}")
		set(SONAR_CACHE_DIR ${CMAKE_BINARY_DIR})
	endif()
	get_filename_component(SONAR_CACHE_DIR ${SONAR_CACHE_DIR} ABSOLUTE)
	configure_file("${RESOURCES_DIR}/sonar-project.properties.in" "${CMAKE_BINARY_DIR}/sonar-project.properties" @ONLY)
endif()

option(USE_SMARTEID "Enable Smart-eID" OFF)

include(Libraries)
include(CompilerFlags)

if(LINUX OR BSD)
	include(GNUInstallDirs)
endif()

add_subdirectory(resources)
add_subdirectory(src)

if(TARGET ${Qt}::Test AND NOT IOS)
	add_subdirectory(test)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/utils")
	add_subdirectory(utils)
endif()

include(Packaging)
