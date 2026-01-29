cmake_minimum_required(VERSION 3.25)

if(POLICY CMP0010)
	cmake_policy(SET CMP0010 NEW)
endif()

if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
endif()

project(AusweisApp_Libs)

list(APPEND LIBS_ID_SALT ${CMAKE_VERSION})
file(GLOB_RECURSE GENERATE_LIBS_ID_SEED "${CMAKE_SOURCE_DIR}/*")
list(FILTER GENERATE_LIBS_ID_SEED EXCLUDE REGEX "README|patches.cmake")

if(CMAKE_TOOLCHAIN_FILE)
	list(APPEND GENERATE_LIBS_ID_SEED "${CMAKE_TOOLCHAIN_FILE}")
endif()

function(GENERATE_LIBS_ID _out)
	foreach(file ${GENERATE_LIBS_ID_SEED})
		file(MD5 "${file}" _hash)
		string(MD5 hashes "${_hash}${hashes}${SALT}${LIBS_ID_SALT}")
	endforeach()
	set(${_out} ${hashes} PARENT_SCOPE)
endfunction()

set(PROJECT_CMAKE_DIR "${PROJECT_SOURCE_DIR}/../cmake")
macro(use_project_include _file)
	if(CONTAINER_SDK)
		set(CMAKE_MODULE_PATH "${PROJECT_CMAKE_DIR};${PROJECT_SOURCE_DIR}")
	else()
		set(CMAKE_MODULE_PATH "${PROJECT_CMAKE_DIR}")
	endif()

	include(${_file} RESULT_VARIABLE _file_path)
	list(APPEND GENERATE_LIBS_ID_SEED ${_file_path})
	unset(CMAKE_MODULE_PATH)
endmacro()

option(INTEGRATED_SDK "Build integrated specific SDK" OFF)
option(CONTAINER_SDK "Build container specific SDK" OFF)
option(DEVELOPER "Include modules/features for developer" OFF)

if(CONTAINER_SDK)
	set(INTEGRATED_SDK ON)
endif()

include(ExternalProject)
use_project_include(Helper)
use_project_include(DVCS)


if(MSVC)
	find_program(MAKE jom CMAKE_FIND_ROOT_PATH_BOTH)
	if(NOT MAKE)
		find_program(MAKE nmake CMAKE_FIND_ROOT_PATH_BOTH)
	endif()
else()
	find_program(MAKE make CMAKE_FIND_ROOT_PATH_BOTH)
endif()

if(MINGW AND NOT MAKE)
	find_program(MAKE mingw32-make CMAKE_FIND_ROOT_PATH_BOTH)
endif()

if(MAKE)
	message(STATUS "Using 'make' command... ${MAKE}")
else()
	message(FATAL_ERROR "Cannot find 'make' command")
endif()

if(DEFINED ENV{CMAKE_BUILD_PARALLEL_LEVEL})
	set(PROCESSOR_COUNT $ENV{CMAKE_BUILD_PARALLEL_LEVEL})
else()
	include(ProcessorCount)
	ProcessorCount(PROCESSOR_COUNT)
endif()

if(NOT PROCESSOR_COUNT EQUAL 0 AND NOT MAKE MATCHES "nmake")
	set(MAKE_JOBS -j${PROCESSOR_COUNT})
	message(STATUS "PROCESSOR_COUNT: ${PROCESSOR_COUNT}")
endif()

if(CMAKE_BUILD_TYPE)
	string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
else()
	set(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "build type configuration" FORCE)
endif()

if(NOT ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG" AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RELEASE" AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RELWITHDEBINFO")
	message(FATAL_ERROR "CMAKE_BUILD_TYPE is invalid! Available options: RELEASE, RELWITHDEBINFO, DEBUG")
endif()

if(MSVC)
	FIND_HOST_PACKAGE(Perl REQUIRED)
else()
	set(PERL_EXECUTABLE perl)
endif()

set(PATCH_CMAKE ${PROJECT_SOURCE_DIR}/patch.cmake)

if(NOT DESTINATION_DIR)
	set(DESTINATION_DIR ${PROJECT_BINARY_DIR}/dist)
endif()


if(NOT PACKAGES_DIR)
	set(PACKAGES_DIR $ENV{PACKAGES_DIR})
	if(NOT PACKAGES_DIR)
		message(STATUS "Define PACKAGES_DIR for local packages")
		set(PACKAGES_DIR ${PROJECT_BINARY_DIR}/download)
	endif()
endif()

string(REPLACE "\\" "/" PACKAGES_DIR ${PACKAGES_DIR})
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/b)
use_project_include(Messages)

if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL CMAKE_SYSTEM_NAME)
	get_filename_component(compiler "${CMAKE_CXX_COMPILER}" NAME)
	string(REGEX REPLACE "[a-z|+]+$" "" CROSS_PREFIX "${compiler}")
endif()

################################## Versions
include(Versions.cmake)
list(APPEND LIBS_ID_SALT "${OPENSSL} ${QT}")

################################## Files
set(QT_FILE qt-everywhere-src-${QT}.tar.xz)
set(OPENSSL_FILE openssl-${OPENSSL}.tar.gz)

################################## Downloads
if("${QT}" MATCHES "alpha|beta|rc")
	set(QT_DEST_DIR development_releases)
else()
	set(QT_DEST_DIR archive) # official_releases
endif()

string(REPLACE "." ";" QT_VERSION_LIST ${QT})
list(GET QT_VERSION_LIST 0 QT_MAJOR_VERSION)
list(GET QT_VERSION_LIST 1 QT_MINOR_VERSION)
set(QT_SUBVERSION ${QT_MAJOR_VERSION}.${QT_MINOR_VERSION})
if(NOT DEFINED ENV{MIRROR_QT})
	set(ENV{MIRROR_QT} https://download.qt.io)
endif()
list(APPEND QT_URLS $ENV{MIRROR_QT}/${QT_DEST_DIR}/qt/${QT_SUBVERSION}/${QT}/single/${QT_FILE})

set(OPENSSL_SOURCE https://www.openssl.org/source)
if("${OPENSSL}" MATCHES "SNAP")
	list(APPEND OPENSSL_URLS ${OPENSSL_SOURCE}/snapshot/${OPENSSL_FILE})
else()
	string(SUBSTRING ${OPENSSL} 0 5 OPENSSL_SUBVERSION)
	if(NOT DEFINED ENV{MIRROR_GITHUB})
		set(ENV{MIRROR_GITHUB} https://github.com)
	endif()
	list(APPEND OPENSSL_URLS $ENV{MIRROR_GITHUB}/openssl/openssl/releases/download/openssl-${OPENSSL}/${OPENSSL_FILE})
	list(APPEND OPENSSL_URLS ${OPENSSL_SOURCE}/old/${OPENSSL_SUBVERSION}/${OPENSSL_FILE})
endif()



set(ENABLED_TARGETS)

################################## OpenSSL
#########################################################################
list(APPEND ENABLED_TARGETS openssl)

set(OPENSSL_CONFIGURE_FLAGS no-camellia no-bf no-aria no-seed no-poly1305 no-srp no-gost no-idea no-mdc2 no-rc2 no-rc4 no-rc5 no-srtp no-sm2 no-sm3 no-sm4)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-ct no-dgram no-cast no-chacha no-blake2 no-rmd160 no-scrypt no-siphash no-whirlpool no-md4 no-des no-ec2m)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-tls1 no-tls1-method no-tls1_1 no-tls1_1-method no-tls1_3 no-ssl3 no-ssl3-method no-dtls no-dtls1-method no-dtls1_2-method)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-deprecated no-engine no-async no-dso no-comp no-ts no-makedepend no-tests)
if(OPENSSL VERSION_GREATER "1.1.1")
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-legacy)
endif()
if(OPENSSL VERSION_GREATER_EQUAL "3.5")
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-tls-deprecated-ec)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
	set(OPENSSL_CONFIGURE_FLAGS --debug ${OPENSSL_CONFIGURE_FLAGS})
	if(MSVC)
		ADD_FLAG(/Od NOQUOTES VAR OPENSSL_COMPILER_FLAGS)
	endif()
else()
	set(OPENSSL_CONFIGURE_FLAGS no-ui-console no-filenames ${OPENSSL_CONFIGURE_FLAGS})
	ADD_FLAG(-Os NOQUOTES VAR OPENSSL_COMPILER_FLAGS)
endif()

if(WIN32)
	ADD_FLAG(-Wl,--dynamicbase NOQUOTES VAR OPENSSL_COMPILER_FLAGS USE_SAME_FOR_LINKER)
	ADD_FLAG(-Wl,--nxcompat NOQUOTES VAR OPENSSL_COMPILER_FLAGS USE_SAME_FOR_LINKER)
	ADD_FLAG(-Wl,--high-entropy-va NOQUOTES VAR OPENSSL_COMPILER_FLAGS USE_SAME_FOR_LINKER)

	if(MSVC AND MAKE MATCHES "jom")
		ADD_FLAG(/FS NOQUOTES VAR OPENSSL_COMPILER_FLAGS)
	endif()
else()
	ADD_FLAG(-fstack-protector-strong -fstack-protector NOQUOTES VAR OPENSSL_COMPILER_FLAGS)
endif()

if(IOS)
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm no-shared)
	if(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
		set(OPENSSL_ARCH iossimulator-xcrun)
		if (CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
			set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} "-arch x86_64")
		elseif (CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
			set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} "-arch arm64")
		endif()
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mios-simulator-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET})
	else()
		set(OPENSSL_ARCH ios64-xcrun)
		string(REGEX REPLACE "/SDKs/.*" "" CROSS_TOP_DEV_ROOT "${CMAKE_OSX_SYSROOT}")
		set(OPENSSL_ENV CROSS_TOP=${CROSS_TOP_DEV_ROOT} CROSS_SDK=iPhoneOS.sdk)
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mios-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET})
	endif()
	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -fvisibility=hidden)
elseif(APPLE)
	if(NOT CMAKE_OSX_ARCHITECTURES)
		if(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64")
			set(CMAKE_OSX_ARCHITECTURES x86_64)
		elseif(CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
			set(CMAKE_OSX_ARCHITECTURES arm64)
		endif()
	endif()

	if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
		set(OPENSSL_ARCH darwin64-x86_64-cc)
	elseif(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
		set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm)
		set(OPENSSL_ARCH darwin64-arm64-cc)
	else()
		message(FATAL_ERROR "Unknown CMAKE_OSX_ARCHITECTURES: ${CMAKE_OSX_ARCHITECTURES}")
	endif()

	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mmacosx-version-min=13.0)
elseif(MINGW OR CYGWIN)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OPENSSL_ARCH mingw64)
	else()
		set(OPENSSL_ARCH mingw)
	endif()

	if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
		set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} --cross-compile-prefix=${CROSS_PREFIX})
	endif()
elseif(MSVC)
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm)
	if(CMAKE_SYSTEM_PROCESSOR STREQUAL "AMD64")
		set(OPENSSL_ARCH VC-WIN64A)
	elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
		set(OPENSSL_ARCH VC-WIN32)
	elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "ARM64")
		set(OPENSSL_ARCH VC-WIN64-ARM)
	else()
		message(FATAL_ERROR "CMAKE_SYSTEM_PROCESSOR not supported by openssl: ${CMAKE_SYSTEM_PROCESSOR}")
	endif()
elseif(ANDROID)
	if(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
		set(OPENSSL_ARCH android-arm)
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mfloat-abi=softfp)
		set(OPENSSL_NDK_PREFIX armv7a)
		set(OPENSSL_NDK_INFIX eabi)
	elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
		set(OPENSSL_ARCH android-x86_64)
		set(OPENSSL_NDK_PREFIX x86_64)
	elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86")
		set(OPENSSL_ARCH android-x86)
		set(OPENSSL_NDK_PREFIX i686)
	elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
		set(OPENSSL_ARCH android-arm64)
		set(OPENSSL_NDK_PREFIX aarch64)
	else()
		message(FATAL_ERROR "CMAKE_ANDROID_ARCH_ABI not supported by openssl: ${CMAKE_ANDROID_ARCH_ABI}")
	endif()
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-stdio)

	get_filename_component(toolchain_bin "${CMAKE_C_COMPILER}" DIRECTORY)
	set(OPENSSL_ENV PATH=${toolchain_bin}/:$ENV{PATH} CC=clang CXX=clang++)

	if(ANDROID_NDK_REVISION VERSION_LESS "23")
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -D__ANDROID_API__=${CMAKE_SYSTEM_VERSION})
	endif()
	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} --target=${OPENSSL_NDK_PREFIX}-linux-android${OPENSSL_NDK_INFIX}${CMAKE_SYSTEM_VERSION})
elseif(BSD)
	if(CMAKE_SYSTEM_PROCESSOR STREQUAL "amd64")
		set(OPENSSL_ARCH BSD-x86_64)
	else()
		message(FATAL_ERROR "CMAKE_SYSTEM_PROCESSOR not supported by openssl: ${CMAKE_SYSTEM_PROCESSOR}")
	endif()
elseif(LINUX)
	if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64")
		set(OPENSSL_ARCH linux-x86_64)
	elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86")
		set(OPENSSL_ARCH linux-x86)
	elseif(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
		set(OPENSSL_ARCH linux-aarch64)
	elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
		set(OPENSSL_ARCH linux-armv4)
	else()
		message(FATAL_ERROR "CMAKE_SYSTEM_PROCESSOR not supported by openssl: ${CMAKE_SYSTEM_PROCESSOR}")
	endif()
else()
	message(FATAL_ERROR "Unsupported system")
endif()

if(OPENSSL_ENV)
	set(OPENSSL_ENV ${CMAKE_COMMAND} -E env ${OPENSSL_ENV})
endif()

if(OPENSSL_PATCHES)
	set(OPENSSL_PATCHES_CMD ${CMAKE_COMMAND} -DCOMPONENT=openssl -P ${PATCH_CMAKE})
endif()

ExternalProject_Add(openssl
	URL ${OPENSSL_URLS}
	URL_HASH SHA256=${OPENSSL_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${OPENSSL_PATCHES_CMD}
	CONFIGURE_COMMAND ${OPENSSL_ENV} ${PERL_EXECUTABLE} <SOURCE_DIR>/Configure --prefix=${DESTINATION_DIR} --libdir=lib ${OPENSSL_CONFIGURE_FLAGS} ${OPENSSL_ARCH} "${OPENSSL_COMPILER_FLAGS}"
	BUILD_COMMAND ${OPENSSL_ENV} ${MAKE} ${MAKE_JOBS}
	INSTALL_COMMAND ${OPENSSL_ENV} ${MAKE} ${MAKE_JOBS} install_sw
)

ExternalProject_Add_Step(openssl configdata
	COMMAND ${PERL_EXECUTABLE} configdata.pm --dump
	DEPENDEES configure
	DEPENDERS build
	WORKING_DIRECTORY <BINARY_DIR>)


if(MAC)
	set(OPENSSL_FILE_VERSION 3)
	add_custom_command(TARGET openssl POST_BUILD
		COMMAND install_name_tool -id @rpath/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}
		COMMAND install_name_tool -id @rpath/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}
		COMMAND install_name_tool -change ${DESTINATION_DIR}/lib/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} @rpath/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

if(IOS)
	# Remove this work-around! Do not build any .dylib or be able to use .dylib
	# Globbing is not supported by cmake command mode! This will work if executed with unix shell only.
	add_custom_command(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E rm -f ${DESTINATION_DIR}/lib/*.dylib)
elseif(ANDROID)
	add_custom_command(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E rm -f ${DESTINATION_DIR}/lib/*.a)
endif()

################################## Qt
#########################################################################
list(APPEND ENABLED_TARGETS qt)

set(QT_CONFIGURE_FLAGS_HOST -release -optimize-size -shared -no-openssl -no-zstd -no-opengl)

if(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
	if(APPLE)
		set(QT_CONFIGURE_FLAGS -debug-and-release) # debug-only framework builds are not supported on macOS
	else()
		set(QT_CONFIGURE_FLAGS -debug)
	endif()
	if(NOT INTEGRATED_SDK)
		set(QT_CONFIGURE_FLAGS_SHARED -qml-debug)
	endif()
else()
	set(QT_CONFIGURE_FLAGS -release)
	if(NOT WIN32)
		list(APPEND QT_CONFIGURE_FLAGS -optimize-size)
	endif()
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} $<$<CONFIG:RelWithDebInfo>:-force-debug-info>)
	if(NOT INTEGRATED_SDK)
		set(QT_CONFIGURE_FLAGS_SHARED -no-qml-debug)
	endif()
endif()

set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -prefix ${DESTINATION_DIR})
set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -system-proxies -openssl-linked -I ${DESTINATION_DIR}/include -L ${DESTINATION_DIR}/lib)

version_to_hex(QT_HEX "${QT}")

set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -disable-deprecated-up-to ${QT_HEX})
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -opensource -confirm-license -sbom)
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -nomake examples -nomake tests -no-dbus)
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -qt-zlib -qt-pcre)
if(NOT INTEGRATED_SDK)
	set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -qt-libpng -qt-libjpeg)
endif()

if(CMAKE_CXX_COMPILER_LAUNCHER STREQUAL "ccache")
	set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -ccache)
endif()

######## Modules

set(QT_CONFIGURE_FLAGS_HOST ${QT_CONFIGURE_FLAGS_HOST} -submodules qtbase,qtdeclarative,qttools)

set(QT_MODULES qtbase,qtwebsockets,qtscxml)
if(NOT INTEGRATED_SDK)
	set(QT_MODULES ${QT_MODULES},qttranslations,qtdeclarative,qtimageformats,qttools,qtsvg,qtconnectivity)
endif()
set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -submodules ${QT_MODULES})

set(QT_MODULES_SKIP qtactiveqt,qtlanguageserver) # enabled by dependency but not necessary
if(INTEGRATED_SDK)
	set(QT_MODULES_SKIP ${QT_MODULES_SKIP},qtdeclarative) # otherwise qtwebsockets and qtscxml enables it
endif()
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -skip ${QT_MODULES_SKIP})

######## Submodules

if(CONTAINER_SDK)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-gui)
endif()

if(IOS OR ANDROID)
	set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -no-widgets)
else()
	set(QT_CONFIGURE_FLAGS_HOST ${QT_CONFIGURE_FLAGS_HOST} -no-widgets)
endif()

######## Features

macro(DISABLE_FEATURES FLAGS)
    foreach(feature ${ARGN})
        list(APPEND ${FLAGS} -no-feature-${feature})
    endforeach()
endmacro()

# qtbase
DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED printsupport private_tests sql xml)
if(CONTAINER_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED testlib)
endif()

# qtbase/src/corelib
DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED hijricalendar islamiccivilcalendar jalalicalendar)
DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED sharedmemory filesystemwatcher)

# qtbase/src/gui
if(NOT CONTAINER_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED directfb imageformat_bmp imageformat_ppm imageformat_xbm)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED linuxfb mtdev pdf textodfwriter undocommand undogroup undostack)
	list(APPEND QT_CONFIGURE_FLAGS_SHARED -qt-harfbuzz)
endif()

# qtbase/src/network
DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED brotli dnslookup dtls localserver libresolv)
DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED schannel sctp securetransport topleveldomain)

# qtbase/src/testlib
if(NOT CONTAINER_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED testlib_selfcover)
endif()

# qtbase/src/widgets
if(NOT (CONTAINER_SDK OR IOS OR ANDROID))
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS calendarwidget colordialog dial fontcombobox fontdialog)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS lcdnumber splashscreen syntaxhighlighter undoview)
	if(NOT MAC)
		DISABLE_FEATURES(QT_CONFIGURE_FLAGS mdiarea)
	endif()
endif()

# qtconnectivity
if(NOT INTEGRATED_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS bluetooth)
endif()

# qtdeclarative/src/quick
if(IOS OR ANDROID)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quick-dialogs)
endif()
if(NOT CONTAINER_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quick-particles quick-path quick-pathview)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quick-sprite quick-tableview quick-treeview)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quick-vectorimage)
endif()

# qtdeclarative/src/quickcontrols
if(NOT CONTAINER_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quickcontrols2-fluentwinui3 quickcontrols2-imagine)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quickcontrols2-material quickcontrols2-universal)
	if(NOT DEVELOPER)
		DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED quickcontrols2-fusion)
	endif()
endif()

# qtscxml
DISABLE_FEATURES(QT_CONFIGURE_FLAGS qeventtransition scxml statemachine-qml)

# qttools
if(CONTAINER_SDK)
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED androiddeployqt)
else()
	DISABLE_FEATURES(QT_CONFIGURE_FLAGS_SHARED designer qdoc)
endif()

# qtwebsockets
DISABLE_FEATURES(QT_CONFIGURE_FLAGS websockets-qml)

########

set(QT_CONFIGURE_FLAGS_OTHER -no-journald)
set(QT_CONFIGURE configure)

if(APPLE)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -appstore-compliant)
endif()
if(IOS)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -xplatform macx-ios-clang)

	if(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
		set(ADDITIONAL_QT_DEFINES_HOST ${ADDITIONAL_QT_DEFINES_HOST} -DCMAKE_OSX_SYSROOT=macosx)
		if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64" OR CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
			set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES})
		endif()
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -sdk iphonesimulator)
	else()
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -sdk iphoneos)
	endif()
elseif(APPLE)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -framework)
elseif(WIN32 OR CYGWIN)
	if(MSVC)
		set(QT_PLATFORM win32-msvc)
		set(QT_ENV OPENSSL_LIBS=-llibcrypto\ -llibssl)
	else()
		set(QT_PLATFORM win32-g++)
		set(QT_ENV OPENSSL_LIBS=-lcrypto\ -lssl)
	endif()

	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-icu)

	if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows" OR CYGWIN)
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -opengl dynamic -platform ${QT_PLATFORM})
		set(QT_CONFIGURE ${QT_CONFIGURE}.bat)
		if(CYGWIN)
			set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -make-tool ${MAKE})
		endif()
	elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -opengl desktop -xplatform ${QT_PLATFORM} -device-option CROSS_COMPILE=${CROSS_PREFIX})
	else()
		message(FATAL_ERROR "Cross-Compiling not supported: ${CMAKE_HOST_SYSTEM_NAME}")
	endif()

	set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} "-DCMAKE_CXX_FLAGS=\"-DQT_WIN_SERVER_2016_COMPAT=1\"")
elseif(ANDROID)
	list(APPEND LIBS_ID_SALT ${ANDROID_NDK_REVISION} ${ANDROID_BUILD_TOOLS_REVISION})
	find_package(Java COMPONENTS Development REQUIRED)

	set(QT_ENV OPENSSL_LIBS=-lcrypto_${CMAKE_ANDROID_ARCH_ABI}\ -lssl_${CMAKE_ANDROID_ARCH_ABI})
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER}
		-android-sdk ${ANDROID_SDK_ROOT} -android-ndk ${CMAKE_ANDROID_NDK} -android-ndk-platform android-${CMAKE_SYSTEM_VERSION}
		-android-abis ${CMAKE_ANDROID_ARCH_ABI} -xplatform android-clang)
	set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} -DQT_ANDROID_API_USED_FOR_JAVA=android-${ANDROID_COMPILE_SDK_VERSION})
elseif(BSD)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -no-libudev)
elseif(LINUX)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -no-libproxy)
	find_program(LINKER ld.mold)
	if(LINKER)
		set(LINKER mold)
	else()
		find_program(LINKER ld.lld)
		if(LINKER)
			set(LINKER lld)
		endif()
	endif()

	if(LINKER)
		message(STATUS "Use linker: ${LINKER}")
		list(APPEND QT_CONFIGURE_FLAGS --linker=${LINKER})
	endif()
else()
	message(FATAL_ERROR "Unsupported system")
endif()

if(QT_ENV)
	set(QT_ENV ${CMAKE_COMMAND} -E env ${QT_ENV})
endif()

if(IOS OR ANDROID)
	option(BUILD_HOST_QT "Build host Qt" ON)

	if(BUILD_HOST_QT AND QT_HOST_PATH)
		message(FATAL_ERROR "Cannot provide QT_HOST_PATH when building host Qt")
	elseif(NOT BUILD_HOST_QT AND NOT QT_HOST_PATH)
		message(FATAL_ERROR "Missing QT_HOST_PATH to provide host tools")
	endif()
endif()

if(QT_PATCHES)
	set(QT_PATCHES_CMD ${CMAKE_COMMAND} -DCOMPONENT=qt -P ${PATCH_CMAKE})
endif()

# Build minimal qt for host tools until a dedicated target exists, see https://bugreports.qt.io/browse/QTQAINFRA-4203
if (BUILD_HOST_QT AND (IOS OR ANDROID))
	list(APPEND ENABLED_TARGETS qt-host)
	set(QT_HOST_DEPEND qt-host)
	set(QT_HOST_PATH ${DESTINATION_DIR}/qt-host)

	set(QT_CONFIGURE_FLAGS_HOST ${QT_CONFIGURE_FLAGS_HOST} -prefix ${QT_HOST_PATH})
	set(QT_CONFIGURE_FLAGS_HOST ${QT_CONFIGURE_FLAGS_HOST} ${QT_CONFIGURE_FLAGS_SHARED})
	set(ADDITIONAL_QT_DEFINES_HOST ${ADDITIONAL_QT_DEFINES_HOST} -DCMAKE_PREFIX_PATH=${QT_HOST_PATH})

	ExternalProject_Add(qt-host
		URL ${QT_URLS}
		URL_HASH SHA256=${QT_HASH}
		DOWNLOAD_DIR ${PACKAGES_DIR}

		PATCH_COMMAND ${QT_PATCHES_CMD}
		CONFIGURE_COMMAND <SOURCE_DIR>/${QT_CONFIGURE} ${QT_CONFIGURE_FLAGS_HOST} -- ${ADDITIONAL_QT_DEFINES_HOST}
		BUILD_COMMAND ${CMAKE_COMMAND} --build . ${MAKE_JOBS}
		INSTALL_COMMAND ${CMAKE_COMMAND} --install .
	)
endif()

if(QT_HOST_PATH)
	set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} -DQT_HOST_PATH=${QT_HOST_PATH})
endif()

set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_SHARED})
set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} -DOPENSSL_ROOT_DIR=${DESTINATION_DIR})
set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} -DCMAKE_PREFIX_PATH=${DESTINATION_DIR})

ExternalProject_Add(qt
	DEPENDS openssl ${QT_HOST_DEPEND}
	URL ${QT_URLS}
	URL_HASH SHA256=${QT_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${QT_PATCHES_CMD}
	CONFIGURE_COMMAND ${QT_ENV} <SOURCE_DIR>/${QT_CONFIGURE} ${QT_CONFIGURE_FLAGS} -- ${ADDITIONAL_QT_DEFINES}
	BUILD_COMMAND ${CMAKE_COMMAND} --build . ${MAKE_JOBS}
	INSTALL_COMMAND ${CMAKE_COMMAND} --install .
)

set(vendor_file "GovernikusConfig.cmake")
add_custom_command(TARGET qt POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_BINARY_DIR}/${vendor_file} ${DESTINATION_DIR}/lib/cmake/Governikus/${vendor_file})

#########################################################################

foreach(var ${ENABLED_TARGETS})
	EXTERNALPROJECT_GET_PROPERTY(${var} INSTALL_DIR)
	list(APPEND CLEAN_TARGETS ${INSTALL_DIR})
endforeach()
set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${DESTINATION_DIR};${CLEAN_TARGETS}")

option(COMPRESS_DEPENDS "Disable DEPENDS for compress target" ON)
if(COMPRESS_DEPENDS)
	set(COMPRESS_TARGETS ${ENABLED_TARGETS})
endif()

string(TIMESTAMP stamp "%Y-%m-%d")
set(IDENTIFIER ${stamp})
FIND_DVCS(${PROJECT_SOURCE_DIR}/..)
if(DVCS_FOUND)
	GET_DVCS_INFO()
	list(APPEND LIBS_ID_SALT ${dvcs_branch})

	if(DEFINED dvcs_phase)
		set(IDENTIFIER ${IDENTIFIER}_${dvcs_phase})
	endif()

	if(DEFINED dvcs_revision)
		set(IDENTIFIER ${IDENTIFIER}_${dvcs_revision})
	endif()
endif()

list(APPEND TARBALL_NAME Toolchain ${CMAKE_SYSTEM_NAME} ${CMAKE_HOST_SYSTEM_PROCESSOR} ${CMAKE_CXX_COMPILER_ID} ${CMAKE_BUILD_TYPE})

if(WIN32)
	if(SIGNTOOL_CMD)
		configure_file(${PROJECT_CMAKE_DIR}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)
		list(APPEND GENERATE_LIBS_ID_SEED "${PROJECT_CMAKE_DIR}/SignFiles.cmake.in")
		set(SIGN_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/SignFiles.cmake")
	endif()
elseif(MAC)
	set(APPLE_SDK macosx)
elseif(ANDROID)
	list(APPEND TARBALL_NAME ${CMAKE_ANDROID_ARCH_ABI})
elseif(IOS)
	if(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
		set(APPLE_SDK iphonesimulator)
	else()
		set(APPLE_SDK iphoneos)
	endif()
	list(APPEND TARBALL_NAME "${APPLE_SDK}")

	if(CMAKE_OSX_ARCHITECTURES)
		list(APPEND TARBALL_NAME ${CMAKE_OSX_ARCHITECTURES})
	endif()
elseif(LINUX)
	set(OS_RELEASE "/etc/os-release")
	if(EXISTS ${OS_RELEASE})
		file(STRINGS ${OS_RELEASE} _content)
		function(EXTRACT _identifier _out)
			foreach(line IN LISTS _content)
				string(REGEX MATCH "^${_identifier}=\"?([a-zA-Z0-9.-_]+)\"?" _unused "${line}")
				if(CMAKE_MATCH_1)
					set(${_out} ${CMAKE_MATCH_1} PARENT_SCOPE)
					break()
				endif()
			endforeach()
		endfunction()

		EXTRACT(ID LINUX_DISTRO)
		EXTRACT(VERSION_ID LINUX_DISTRO_VERSION)
		list(APPEND TARBALL_NAME ${LINUX_DISTRO} ${LINUX_DISTRO_VERSION})
	endif()
endif()

if(APPLE_SDK)
	execute_process(
		COMMAND xcrun --sdk ${APPLE_SDK} --show-sdk-path
		OUTPUT_VARIABLE APPLE_SDK_PATH
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)
	message(STATUS "APPLE_SDK_PATH: ${APPLE_SDK_PATH}")
	list(APPEND GENERATE_LIBS_ID_SEED "${APPLE_SDK_PATH}")
endif()

add_custom_target(compress.pre
		COMMAND ${CMAKE_COMMAND} -E rm -rf "${DESTINATION_DIR}/doc" "${DESTINATION_DIR}/share"
		COMMAND ${SIGN_COMMAND}
		DEPENDS ${COMPRESS_TARGETS}
		WORKING_DIRECTORY "${DESTINATION_DIR}")

if(SALT)
	GENERATE_LIBS_ID(IDENTIFIER)
endif()
list(APPEND TARBALL_NAME ${IDENTIFIER})
list(JOIN TARBALL_NAME "_" TARBALL_NAME)
set(compressed_filename ${TARBALL_NAME}.tar.zstd)
message(STATUS "Use tarball: ${compressed_filename}")
add_custom_command(OUTPUT ${compressed_filename}
		COMMAND ${CMAKE_COMMAND} -E tar cf "${compressed_filename}" --zstd "${DESTINATION_DIR}"
		DEPENDS compress.pre)
add_custom_target(compress DEPENDS ${compressed_filename})

include(patch.cmake)
if(QT_PATCHES)
	FETCH_PATCHES_NAMES(QT_PATCHES qt)
endif()
if(OPENSSL_PATCHES)
	FETCH_PATCHES_NAMES(OPENSSL_PATCHES openssl)
endif()
include(CMakePackageConfigHelpers)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/${vendor_file}.in ${CMAKE_BINARY_DIR}/${vendor_file} INSTALL_DESTINATION ${DESTINATION_DIR})
