cmake_minimum_required(VERSION 3.8.0)

if(POLICY CMP0010)
	cmake_policy(SET CMP0010 NEW)
endif()

if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

if(POLICY CMP0135)
	cmake_policy(SET CMP0135 NEW)
endif()

if(NOT ANDROID AND NOT IOS)
	set(COMPILER_SEARCH C CXX)
endif()

project(AusweisApp2_Libs ${COMPILER_SEARCH})

option(INTEGRATED_SDK "Build integrated specific SDK" OFF)
option(CONTAINER_SDK "Build container specific SDK" OFF)

if(CONTAINER_SDK)
	set(INTEGRATED_SDK ON)
	set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake;${PROJECT_SOURCE_DIR}")
else()
	set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake")
endif()

include(ExternalProject)
include(Helper)
include(DVCS)


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

if(NOT DEFINED PROCESSOR_COUNT)
	set(PROCESSOR_COUNT $ENV{PROCESSOR_COUNT})
endif()

if(NOT DEFINED PROCESSOR_COUNT)
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

set(PATCH_CMAKE ${CMAKE_CURRENT_BINARY_DIR}/patch.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/patch.cmake.in ${PATCH_CMAKE} @ONLY)

if(NOT DESTINATION_DIR)
	set(DESTINATION_DIR ${PROJECT_BINARY_DIR}/dist)
endif()

configure_file(${PROJECT_SOURCE_DIR}/test/valgrind.supp.${CMAKE_BUILD_TYPE} ${DESTINATION_DIR}/test/valgrind.supp COPYONLY)

if(NOT PACKAGES_DIR)
	set(PACKAGES_DIR $ENV{PACKAGES_DIR})
	if(NOT PACKAGES_DIR)
		message(STATUS "Define PACKAGES_DIR for local packages")
		set(PACKAGES_DIR ${PROJECT_BINARY_DIR}/download)
	endif()
endif()

string(REPLACE "\\" "/" PACKAGES_DIR ${PACKAGES_DIR})
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/b)
include(Messages)

if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL CMAKE_SYSTEM_NAME)
	get_filename_component(compiler "${CMAKE_CXX_COMPILER}" NAME)
	string(REGEX REPLACE "[a-z|+]+$" "" CROSS_PREFIX "${compiler}")
endif()

################################## Versions
include(Versions.cmake)

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
list(APPEND QT_URLS https://download.qt.io/${QT_DEST_DIR}/qt/${QT_SUBVERSION}/${QT}/single/${QT_FILE})
list(APPEND QT_URLS https://ftp.fau.de/qtproject/${QT_DEST_DIR}/qt/${QT_SUBVERSION}/${QT}/single/${QT_FILE})

set(OPENSSL_SOURCE https://www.openssl.org/source)
if("${OPENSSL}" MATCHES "SNAP")
	list(APPEND OPENSSL_URLS ${OPENSSL_SOURCE}/snapshot/${OPENSSL_FILE})
else()
	string(SUBSTRING ${OPENSSL} 0 5 OPENSSL_SUBVERSION)
	list(APPEND OPENSSL_URLS ${OPENSSL_SOURCE}/${OPENSSL_FILE})
	list(APPEND OPENSSL_URLS ${OPENSSL_SOURCE}/old/${OPENSSL_SUBVERSION}/${OPENSSL_FILE})
endif()



set(ENABLED_TARGETS)

################################## OpenSSL
#########################################################################
list(APPEND ENABLED_TARGETS openssl)

set(OPENSSL_CONFIGURE_FLAGS no-camellia no-bf no-aria no-seed no-poly1305 no-srp no-gost no-idea no-mdc2 no-rc2 no-rc4 no-rc5 no-srtp no-sm2 no-sm3 no-sm4)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-ct no-dgram no-cast no-chacha no-blake2 no-rmd160 no-scrypt no-siphash no-whirlpool no-md4 no-des no-ec2m)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-tls1 no-tls1-method no-tls1_1 no-tls1_1-method no-tls1_3 no-ssl3 no-ssl3-method no-dtls no-dtls1-method no-dtls1_2-method)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-deprecated no-engine no-async no-dso no-comp no-ts no-makedepend no-tests no-legacy shared)

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
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm)
	if(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
		set(OPENSSL_ARCH iossimulator-xcrun)
		if (CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
			set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} "-arch arm64")
		endif()
	else()
		set(OPENSSL_ARCH ios64-cross)
		string(REGEX REPLACE "/SDKs/.*" "" CROSS_TOP_DEV_ROOT "${CMAKE_OSX_SYSROOT}")
		set(OPENSSL_ENV CROSS_TOP=${CROSS_TOP_DEV_ROOT} CROSS_SDK=iPhoneOS.sdk)
	endif()
	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -fvisibility=hidden)
elseif(APPLE)
	if(NOT CMAKE_OSX_ARCHITECTURES AND CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "arm64")
		set(CMAKE_OSX_ARCHITECTURES arm64)
	endif()

	if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
		set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm)
		set(OPENSSL_ARCH darwin64-arm64-cc)
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mmacosx-version-min=11.0)
	else()
		set(OPENSSL_ARCH darwin64-x86_64-cc)
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mmacosx-version-min=10.15)
	endif()
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
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OPENSSL_ARCH VC-WIN64A)
	else()
		set(OPENSSL_ARCH VC-WIN32)
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
		message(FATAL_ERROR "CMAKE_ANDROID_ARCH_ABI not supported by openssl")
	endif()
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-stdio)

	get_filename_component(toolchain_bin "${CMAKE_C_COMPILER}" DIRECTORY)
	set(OPENSSL_ENV PATH=${toolchain_bin}/:$ENV{PATH} CC=clang CXX=clang++)

	if(ANDROID_NDK_REVISION VERSION_LESS "23")
		set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -D__ANDROID_API__=${CMAKE_SYSTEM_VERSION})
	endif()
	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} --target=${OPENSSL_NDK_PREFIX}-linux-android${OPENSSL_NDK_INFIX}${CMAKE_SYSTEM_VERSION})
elseif(BSD)
	set(OPENSSL_ARCH BSD-x86_64)
elseif(LINUX)
	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(OPENSSL_ARCH linux-x86_64)
	else()
		set(OPENSSL_ARCH linux-generic32)
	endif()
else()
	message(FATAL_ERROR "Unsupported system")
endif()

if(OPENSSL_ENV)
	set(OPENSSL_ENV ${CMAKE_COMMAND} -E env ${OPENSSL_ENV})
endif()

ExternalProject_Add(openssl
	URL ${OPENSSL_URLS}
	URL_HASH SHA256=${OPENSSL_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${CMAKE_COMMAND} -DCOMPONENT=openssl -P ${PATCH_CMAKE}
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
	if(CMAKE_VERSION VERSION_LESS "3.17")
		add_custom_command(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${DESTINATION_DIR}/lib/*.dylib)
	else()
		add_custom_command(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E rm -f ${DESTINATION_DIR}/lib/*.dylib)
	endif()
elseif(ANDROID)
	if(CMAKE_VERSION VERSION_LESS "3.17")
		add_custom_command(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${DESTINATION_DIR}/lib/*.a)
	else()
		add_custom_command(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E rm -f ${DESTINATION_DIR}/lib/*.a)
	endif()
endif()

################################## Qt
#########################################################################
list(APPEND ENABLED_TARGETS qt)

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

set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -opensource -confirm-license)
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -nomake examples -nomake tests -no-mtdev -no-dbus -no-sql-sqlite)
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -qt-zlib -qt-pcre -qt-harfbuzz)
if(NOT INTEGRATED_SDK)
	set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -qt-libpng -qt-libjpeg)
endif()

if(CMAKE_CXX_COMPILER_LAUNCHER STREQUAL "ccache")
	set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -ccache)
endif()


list(APPEND NO_FEATURES_SHARED hijricalendar islamiccivilcalendar jalalicalendar)
list(APPEND NO_FEATURES_SHARED designer testlib_selfcover)
list(APPEND NO_FEATURES_SHARED imageformat_bmp imageformat_ppm imageformat_xbm)
list(APPEND NO_FEATURES_SHARED sharedmemory textodfwriter)
list(APPEND NO_FEATURES_SHARED undocommand undogroup undostack)
list(APPEND NO_FEATURES_SHARED printsupport dtls dom sql xml pdf)
if(CONTAINER_SDK)
	list(APPEND NO_FEATURES_SHARED testlib androiddeployqt)
endif()
foreach(feature ${NO_FEATURES_SHARED})
	set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -no-feature-${feature})
endforeach()

list(APPEND NO_FEATURES calendarwidget colordialog dial fontcombobox fontdialog lcdnumber)
list(APPEND NO_FEATURES splashscreen syntaxhighlighter undoview)
if(NOT MAC)
	list(APPEND NO_FEATURES mdiarea)
endif()
foreach(feature ${NO_FEATURES})
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-feature-${feature})
endforeach()


set(QT_MODULES qtbase,qtwebsockets,qtscxml)
if(NOT INTEGRATED_SDK)
	set(QT_MODULES ${QT_MODULES},qttranslations,qtdeclarative,qtimageformats,qttools,qtsvg,qtconnectivity,qtshadertools)
endif()
set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -submodules ${QT_MODULES})

set(QT_MODULES_SKIP qtactiveqt,qtlanguageserver) # enabled by dependency but not necessary
if(INTEGRATED_SDK)
	set(QT_MODULES_SKIP ${QT_MODULES_SKIP},qtdeclarative) # otherwise qtwebsockets and qtscxml enables it
endif()
set(QT_CONFIGURE_FLAGS_SHARED ${QT_CONFIGURE_FLAGS_SHARED} -skip ${QT_MODULES_SKIP})


set(QT_CONFIGURE_FLAGS_OTHER -no-journald -no-directfb -no-linuxfb)
set(QT_CONFIGURE configure)

if(APPLE)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -appstore-compliant)
endif()
if(IOS)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -xplatform macx-ios-clang)

	if(CMAKE_OSX_SYSROOT MATCHES "iphonesimulator")
		set(QT_HOST_CMAKE_FLAGS ${QT_HOST_CMAKE_FLAGS} -DCMAKE_OSX_SYSROOT=macosx)
		if (CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
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

	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-icu -no-sql-odbc)

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
elseif(ANDROID)
	find_package(Java COMPONENTS Development REQUIRED)

	set(QT_ENV OPENSSL_LIBS=-lcrypto_${CMAKE_ANDROID_ARCH_ABI}\ -lssl_${CMAKE_ANDROID_ARCH_ABI})
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER}
		-android-sdk ${ANDROID_SDK} -android-ndk ${CMAKE_ANDROID_NDK} -android-ndk-platform android-${CMAKE_SYSTEM_VERSION}
		-android-abis ${CMAKE_ANDROID_ARCH_ABI} -xplatform android-clang)

	# ANDROID_TARGET_SDK_VERSION cannot be passed here as Qt has it's own variable and uses the latest in "${ANDROID_SDK_ROOT}/platforms".
	# https://code.qt.io/cgit/qt/qtbase.git/tree/cmake/QtPlatformAndroid.cmake?h=6.4#n36

	if(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-use-gold-linker)
	endif()
elseif(BSD)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -no-libudev)
elseif(LINUX)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -no-libproxy --linker=gold)
else()
	message(FATAL_ERROR "Unsupported system")
endif()

if(IOS OR ANDROID)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-widgets)
endif()

if(QT_ENV)
	set(QT_ENV ${CMAKE_COMMAND} -E env ${QT_ENV})
endif()
if(CONTAINER_SDK)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-gui -no-accessibility -no-freetype)
endif()

if(IOS OR ANDROID)
	option(BUILD_HOST_QT "Build host Qt" ON)

	if(BUILD_HOST_QT AND QT_HOST_PATH)
		message(FATAL_ERROR "Cannot provide QT_HOST_PATH when building host Qt")
	elseif(NOT BUILD_HOST_QT AND NOT QT_HOST_PATH)
		message(FATAL_ERROR "Missing QT_HOST_PATH to provide host tools")
	endif()
endif()

# Build minimal qt for host tools until a dedicated target exists, see https://bugreports.qt.io/browse/QTQAINFRA-4203
if (BUILD_HOST_QT AND (IOS OR ANDROID))
	list(APPEND ENABLED_TARGETS qt-host)
	set(QT_HOST_DEPEND qt-host)
	set(QT_HOST_PATH ${DESTINATION_DIR}/qt-host)

	set(QT_HOST_CONFIGURE_FLAGS -prefix ${QT_HOST_PATH} -release -optimize-size -shared -no-widgets -no-openssl -no-zstd -no-opengl)
	set(QT_HOST_CONFIGURE_FLAGS ${QT_HOST_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_SHARED})
	set(QT_HOST_CMAKE_FLAGS ${QT_HOST_CMAKE_FLAGS} -DCMAKE_PREFIX_PATH=${QT_HOST_PATH})
	set(QT_HOST_CONFIGURE_FLAGS ${QT_HOST_CONFIGURE_FLAGS} -submodules qtbase,qtdeclarative,qtshadertools,qttools)

	ExternalProject_Add(qt-host
		URL ${QT_URLS}
		URL_HASH SHA256=${QT_HASH}
		DOWNLOAD_DIR ${PACKAGES_DIR}

		PATCH_COMMAND ${CMAKE_COMMAND} -DCOMPONENT=qt -P ${PATCH_CMAKE} &&
			${CMAKE_COMMAND} -E touch qtbase/.gitignore
		CONFIGURE_COMMAND <SOURCE_DIR>/${QT_CONFIGURE} ${QT_HOST_CONFIGURE_FLAGS} -- ${QT_HOST_CMAKE_FLAGS}
		BUILD_COMMAND ${CMAKE_COMMAND} --build . ${MAKE_JOBS}
		INSTALL_COMMAND ${CMAKE_COMMAND} --install .
	)
endif()

if(QT_HOST_PATH)
	set(ADDITIONAL_QT_DEFINES ${ADDITIONAL_QT_DEFINES} -DQT_HOST_PATH=${QT_HOST_PATH})
endif()

ExternalProject_Add(qt
	DEPENDS openssl ${QT_HOST_DEPEND}
	URL ${QT_URLS}
	URL_HASH SHA256=${QT_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${CMAKE_COMMAND} -DCOMPONENT=qt -P ${PATCH_CMAKE} &&
			${CMAKE_COMMAND} -E touch qtbase/.gitignore
	CONFIGURE_COMMAND ${QT_ENV} <SOURCE_DIR>/${QT_CONFIGURE} ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_SHARED} -- -DOPENSSL_ROOT_DIR=${DESTINATION_DIR} -DCMAKE_PREFIX_PATH=${DESTINATION_DIR} ${ADDITIONAL_QT_DEFINES}
	BUILD_COMMAND ${CMAKE_COMMAND} --build . ${MAKE_JOBS}
	INSTALL_COMMAND ${CMAKE_COMMAND} --install .
)

add_custom_command(TARGET qt POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${DESTINATION_DIR}/mkspecs/qt_vendor_governikus)

#########################################################################

foreach(var ${ENABLED_TARGETS})
	EXTERNALPROJECT_GET_PROPERTY(${var} INSTALL_DIR)
	list(APPEND CLEAN_TARGETS ${INSTALL_DIR})
endforeach()
if(CMAKE_VERSION VERSION_LESS "3.15")
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DESTINATION_DIR};${CLEAN_TARGETS}")
else()
	set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${DESTINATION_DIR};${CLEAN_TARGETS}")
endif()

option(COMPRESS_DEPENDS "Disable DEPENDS for compress target" ON)
if(COMPRESS_DEPENDS)
	set(COMPRESS_TARGETS ${ENABLED_TARGETS})
endif()

string(TIMESTAMP stamp "%Y-%m-%d")
FIND_DVCS(${PROJECT_SOURCE_DIR}/..)
if(DVCS_FOUND)
	GET_DVCS_INFO()

	if(DEFINED dvcs_phase)
		set(stamp ${stamp}_${dvcs_phase})
	endif()

	if(DEFINED dvcs_revision)
		set(stamp ${stamp}_${dvcs_revision})
	endif()
endif()

if(ANDROID)
	set(SYSTEM_NAME ${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ID}_${CMAKE_ANDROID_ARCH_ABI})
else()
	set(SYSTEM_NAME ${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ID})
endif()

if(WIN32)
	if(SIGNTOOL_CMD)
		configure_file(${CMAKE_MODULE_PATH}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)
		set(SIGN_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/SignFiles.cmake")
	endif()
endif()

if(CMAKE_VERSION VERSION_LESS "3.15" OR BSD)
	set(COMPRESSION cfJ)
	set(COMPRESSION_FILENDING tar.xz)
else()
	set(COMPRESSION cf)
	set(COMPRESSION_OPTION --zstd)
	set(COMPRESSION_FILENDING tar.zstd)
endif()

if(CMAKE_VERSION VERSION_LESS "3.17")
	set(REMOVE_DIRECTORIES ${CMAKE_COMMAND} -E remove_directory "${DESTINATION_DIR}/doc" "${DESTINATION_DIR}/share")
else()
	set(REMOVE_DIRECTORIES ${CMAKE_COMMAND} -E rm -rf "${DESTINATION_DIR}/doc" "${DESTINATION_DIR}/share")
endif()

add_custom_target(compress.pre ${compressed_filename}
		COMMAND ${REMOVE_DIRECTORIES}
		COMMAND ${SIGN_COMMAND}
		DEPENDS ${COMPRESS_TARGETS}
		WORKING_DIRECTORY "${DESTINATION_DIR}")

set(compressed_filename Toolchain_${SYSTEM_NAME}_${stamp}.${COMPRESSION_FILENDING})
add_custom_command(OUTPUT ${compressed_filename}
		COMMAND ${CMAKE_COMMAND} -E tar "${COMPRESSION}" "${compressed_filename}" ${COMPRESSION_OPTION} "${DESTINATION_DIR}"
		DEPENDS compress.pre)
add_custom_target(compress DEPENDS ${compressed_filename})
