function(READ_REVISION _var _regex _files)
	foreach(file ${_files})
		if(EXISTS "${file}")
			file(READ "${file}" content)
			string(REGEX MATCH "${_regex}" _unused "${content}")
			set(${_var} ${CMAKE_MATCH_1} PARENT_SCOPE)
			return()
		endif()
	endforeach()
endfunction()

IF(NOT CMAKE_ANDROID_NDK)
	set(_ndk_dest $ENV{ANDROID_NDK_ROOT} $ENV{ANDROID_NDK_HOME} $ENV{ANDROID_NDK})
	foreach(_ndk ${_ndk_dest})
		if(EXISTS "${_ndk}")
			set(CMAKE_ANDROID_NDK ${_ndk})
		endif()
	endforeach()

	if(NOT CMAKE_ANDROID_NDK)
		message(FATAL_ERROR "Cannot find ANDROID_NDK_ROOT")
	endif()
endif()

set(ANDROID_SDK_ROOT $ENV{ANDROID_SDK_ROOT})
set(ANDROID_BUILD_TOOLS_REVISION $ENV{ANDROID_BUILD_TOOLS_REVISION})

if(NOT EXISTS "${ANDROID_SDK_ROOT}")
	message(FATAL_ERROR "Environment variable ANDROID_SDK_ROOT is undefined")
endif()

if(NOT ANDROID_BUILD_TOOLS_REVISION)
	set(_android_build_tools_dir "${ANDROID_SDK_ROOT}/build-tools")

	file(GLOB build_tools_list ${_android_build_tools_dir}/*)
	list(LENGTH build_tools_list build_tools_len)
	if(build_tools_len GREATER 0)
		list(SORT build_tools_list)
		list(GET build_tools_list -1 build_tools)
	endif()

	if(NOT build_tools)
		message(FATAL_ERROR "Cannot determine ANDROID_BUILD_TOOLS_REVISION: ${_android_build_tools_dir}")
	endif()

	get_filename_component(ANDROID_BUILD_TOOLS_REVISION "${build_tools}" NAME)
endif()

READ_REVISION(ANDROID_NDK_REVISION ".*Revision = ([0-9|\\.]+)" "${CMAKE_ANDROID_NDK}/source.properties")

if(NOT QT_TOOLCHAIN_FILE)
	foreach(path ${CMAKE_PREFIX_PATH})
		set(file ${path}/lib/cmake/Qt6/qt.toolchain.cmake)
		if(EXISTS "${file}")
			set(QT_TOOLCHAIN_FILE ${file})
			break()
		endif()
	endforeach()
endif()

if(QT_TOOLCHAIN_FILE)
	message(STATUS "QT_TOOLCHAIN_FILE: ${QT_TOOLCHAIN_FILE}")
	include(${QT_TOOLCHAIN_FILE})
endif()

set(ANDROID_BUILD_NAME android-build)
set(ANDROID_BUILD_DIR "${PROJECT_BINARY_DIR}/src/${ANDROID_BUILD_NAME}")
set(ANDROID_PACKAGE_SRC_DIR ${PROJECT_BINARY_DIR}/package-src-dir)
set(QT_ENABLE_VERBOSE_DEPLOYMENT ON)

if(DEFINED ENV{QT_ANDROID_KEYSTORE_PATH} AND NOT INTEGRATED_SDK)
	set(QT_ANDROID_SIGN_APK ON)
	set(QT_ANDROID_SIGN_AAB ON)
endif()

set(CMAKE_ANDROID_NDK_TOOLCHAIN_VERSION clang)
set(CMAKE_SYSTEM_NAME Android)
set(CMAKE_SYSTEM_VERSION 28)
set(ANDROID_TARGET_SDK_VERSION 35)
set(ANDROID_COMPILE_SDK_VERSION 35)
set(CMAKE_ANDROID_STL_TYPE c++_shared)

if(NOT CMAKE_ANDROID_ARCH_ABI)
	set(CMAKE_ANDROID_ARCH_ABI armeabi-v7a)
endif()

# Only required by CMake < 3.30.3 with Android NDK 27 when
# CMAKE_ANDROID_ARCH_ABI is not set. Will be fixed with Android NDK 27b.
# https://bugreports.qt.io/browse/QTBUG-127468
if(ANDROID_NDK_REVISION VERSION_EQUAL "27.0.12077973" AND CMAKE_VERSION VERSION_LESS "3.30.3")
	set(NDK_PROC_aarch64_ABI "DUMMY")
	set(NDK_PROC_armv7-a_ABI "DUMMY")
	set(NDK_PROC_i686_ABI    "DUMMY")
	set(NDK_PROC_x86_64_ABI  "DUMMY")
endif()

if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
	set(CMAKE_ANDROID_ARM_MODE ON)
	set(CMAKE_ANDROID_ARM_NEON ON)
endif()

set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(GOVERNIKUS_TOOLCHAIN_FILE ON)
