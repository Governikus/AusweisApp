cmake_minimum_required(VERSION 3.8.0)

if(POLICY CMP0010)
	cmake_policy(SET CMP0010 NEW)
endif()

if(POLICY CMP0020)
	cmake_policy(SET CMP0020 NEW)
endif()

if(NOT ANDROID AND NOT IOS)
	set(COMPILER_SEARCH C CXX)
endif()

project(AusweisApp2_Libs ${COMPILER_SEARCH})

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake")
include(ExternalProject)
include(Helper)
include(DVCS)


if(MSVC)
	find_program(MAKE nmake CMAKE_FIND_ROOT_PATH_BOTH)
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
	include(ProcessorCount)
	ProcessorCount(PROCESSOR_COUNT)
endif()

if(NOT PROCESSOR_COUNT EQUAL 0 AND NOT "${MAKE}" MATCHES "nmake")
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
set(QT 5.15.2)
set(QT_HASH 3a530d1b243b5dec00bc54937455471aaa3e56849d2593edb8ded07228202240)

set(OPENSSL 1.1.1m)
set(OPENSSL_HASH f89199be8b23ca45fc7cb9f1d8d3ee67312318286ad030f5316aca6462db6c96)

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

set(OPENSSL_CONFIGURE_FLAGS no-camellia no-bf no-aria no-seed no-poly1305 no-srp no-gost no-idea no-mdc2 no-rc2 no-rc4 no-rc5 no-srtp no-hw no-sm2 no-sm3 no-sm4)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-ct no-dgram no-cast no-chacha no-blake2 no-rmd160 no-scrypt no-siphash no-whirlpool no-md4 no-des)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-tls1 no-tls1-method no-tls1_1 no-tls1_1-method no-tls1_3 no-ssl3 no-ssl3-method no-dtls no-dtls1-method no-dtls1_2-method)
set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-deprecated no-engine no-async no-dso no-comp no-ts no-makedepend no-tests shared)

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
else()
	ADD_FLAG(-fstack-protector-strong -fstack-protector NOQUOTES VAR OPENSSL_COMPILER_FLAGS)
endif()

if(IOS)
	if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
		set(OPENSSL_ARCH iossimulator-xcrun)
		set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm)
	else()
		set(OPENSSL_ARCH ios64-cross)
		string(REGEX REPLACE "/SDKs/.*" "" CROSS_TOP_DEV_ROOT "${CMAKE_OSX_SYSROOT}")
		set(OPENSSL_ENV export CROSS_TOP=${CROSS_TOP_DEV_ROOT} && export CROSS_SDK=iPhoneOS.sdk &&)
	endif()
elseif(APPLE)
	set(OPENSSL_ARCH darwin64-x86_64-cc)
	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mmacosx-version-min=10.13)
elseif(MINGW)
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
	elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86_64")
		set(OPENSSL_ARCH android-x86_64)
	elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86")
		set(OPENSSL_ARCH android-x86)
	elseif(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
		set(OPENSSL_ARCH android-arm64)
	else()
		message(FATAL_ERROR "CMAKE_ANDROID_ARCH_ABI not supported by openssl")
	endif()
	set(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-stdio)

	get_filename_component(toolchain_bin "${CMAKE_C_COMPILER}" DIRECTORY)
	set(OPENSSL_ENV export PATH=${toolchain_bin}/:$ENV{PATH} &&)
	set(OPENSSL_ENV ${OPENSSL_ENV} export CC=clang && export CXX=clang++ &&)

	set(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -D__ANDROID_API__=${CMAKE_SYSTEM_VERSION})
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

ExternalProject_Add(openssl
	URL ${OPENSSL_URLS}
	URL_HASH SHA256=${OPENSSL_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${CMAKE_COMMAND} -DCOMPONENT=openssl -P ${PATCH_CMAKE}
	CONFIGURE_COMMAND ${OPENSSL_ENV} ${PERL_EXECUTABLE} <SOURCE_DIR>/Configure --prefix=${DESTINATION_DIR} ${OPENSSL_CONFIGURE_FLAGS} ${OPENSSL_ARCH} "${OPENSSL_COMPILER_FLAGS}"
	BUILD_COMMAND ${OPENSSL_ENV} ${MAKE} ${MAKE_JOBS}
	INSTALL_COMMAND ${OPENSSL_ENV} ${MAKE} ${MAKE_JOBS} install_sw
)

ExternalProject_Add_Step(openssl configdata
	COMMAND ${PERL_EXECUTABLE} configdata.pm --dump
	DEPENDEES configure
	DEPENDERS build
	WORKING_DIRECTORY <BINARY_DIR>)


if(MAC)
	set(OPENSSL_FILE_VERSION 1.1)
	add_custom_command(TARGET openssl POST_BUILD
		COMMAND install_name_tool -id libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}
		COMMAND install_name_tool -id libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}
		COMMAND install_name_tool -change ${DESTINATION_DIR}/lib/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX})
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
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -qml-debug)
else()
	set(QT_CONFIGURE_FLAGS -release -optimize-size -no-qml-debug)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} $<$<CONFIG:RelWithDebInfo>:-force-debug-info>)
endif()

set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -prefix ${DESTINATION_DIR} -opensource -confirm-license -recheck-all)
set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -nomake examples -nomake tests -no-mtdev -no-dbus -no-harfbuzz -no-compile-examples -no-sql-sqlite)
set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -qt-zlib -qt-libpng -qt-libjpeg -qt-pcre)
set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -system-proxies -openssl-linked -I ${DESTINATION_DIR}/include -L ${DESTINATION_DIR}/lib)

if(CMAKE_CXX_COMPILER_LAUNCHER STREQUAL "ccache")
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -ccache)
endif()


list(APPEND NO_FEATURES dtls winrt_bt ftp lcdnumber mdiarea)
list(APPEND NO_FEATURES hijricalendar islamiccivilcalendar jalalicalendar)
list(APPEND NO_FEATURES native-win32-bluetooth testlib_selfcover)
list(APPEND NO_FEATURES calendarwidget colordialog cups dial fontcombobox fontdialog)
list(APPEND NO_FEATURES imageformat_bmp imageformat_ppm imageformat_xbm)
list(APPEND NO_FEATURES sharedmemory textodfwriter bearermanagement)
list(APPEND NO_FEATURES undocommand undogroup undostack undoview)
list(APPEND NO_FEATURES printer printdialog printpreviewdialog printpreviewwidget)
list(APPEND NO_FEATURES splashscreen syntaxhighlighter dom sql qdoc)
foreach(feature ${NO_FEATURES})
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-feature-${feature})
endforeach()


list(APPEND SKIP_MODULES qtwebglplugin qtscxml qtxmlpatterns qtwebchannel)
list(APPEND SKIP_MODULES qtwebengine qtscript qtactiveqt qtlocation qtserialbus)
list(APPEND SKIP_MODULES qtserialport qtgamepad qtvirtualkeyboard qtcanvas3d qtcharts)
list(APPEND SKIP_MODULES qtdatavis3d qt3d qtpurchasing qtwayland qtremoteobjects)
list(APPEND SKIP_MODULES qtspeech qtwebview multimedia qtquickcontrols qtlottie qtquick3d)
foreach(module ${SKIP_MODULES})
	set(QT_CONFIGURE_FLAGS_SKIP_MODULES ${QT_CONFIGURE_FLAGS_SKIP_MODULES} -skip ${module})
endforeach()


set(QT_CONFIGURE_FLAGS_OTHER -no-journald -no-directfb -no-linuxfb)
set(QT_CONFIGURE configure)
if(IOS)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -xplatform macx-ios-clang)

	if(CMAKE_OSX_ARCHITECTURES STREQUAL "x86_64")
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -sdk iphonesimulator)
	else()
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -sdk iphoneos)
	endif()
elseif(APPLE)
	find_program(XCODE_SELECT xcode-select)
	if(NOT XCODE_SELECT)
		message(FATAL_ERROR "Cannot find xcode-select")
	endif()

	execute_process(COMMAND ${XCODE_SELECT} -p OUTPUT_VARIABLE osx_dev_dir OUTPUT_STRIP_TRAILING_WHITESPACE)
	file(GLOB osx_sdk "${osx_dev_dir}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX1*")
	get_filename_component(osx_sdk ${osx_sdk} NAME)
	string(REPLACE ".sdk" "" osx_sdk "${osx_sdk}")
	string(TOLOWER "${osx_sdk}" osx_sdk)
	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -framework -sdk ${osx_sdk})
elseif(WIN32)
	if(MSVC)
		if(MSVC_TOOLSET_VERSION STREQUAL "140")
			set(QT_PLATFORM win32-msvc2015)
		elseif(MSVC_TOOLSET_VERSION STREQUAL "141")
			set(QT_PLATFORM win32-msvc2017)
		elseif(MSVC_TOOLSET_VERSION STREQUAL "142")
			set(QT_PLATFORM win32-msvc2019)
		elseif(MSVC_TOOLSET_VERSION STREQUAL "143")
			set(QT_PLATFORM win32-msvc)
		else()
			message(FATAL_ERROR "Version of MSVC not supported: ${MSVC_TOOLSET_VERSION}")
		endif()

		set(QT_OPENSSL OPENSSL_LIBS=-llibcrypto\ -llibssl)
	else()
		set(QT_PLATFORM win32-g++)
		set(QT_OPENSSL OPENSSL_LIBS=-lcrypto\ -lssl)
	endif()

	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_OPENSSL} -no-icu -no-sql-odbc)

	if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -opengl dynamic -platform ${QT_PLATFORM})
		set(QT_CONFIGURE ${QT_CONFIGURE}.bat)
	elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
		set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -opengl desktop -xplatform ${QT_PLATFORM} -device-option CROSS_COMPILE=${CROSS_PREFIX})
	else()
		message(FATAL_ERROR "Cross-Compiling not supported: ${CMAKE_HOST_SYSTEM_NAME}")
	endif()
elseif(ANDROID)
	find_package(Java COMPONENTS Development REQUIRED)

	set(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER}
		-android-sdk ${ANDROID_SDK} -android-ndk ${CMAKE_ANDROID_NDK} -android-ndk-platform android-${CMAKE_SYSTEM_VERSION} -android-ndk-host ${CMAKE_ANDROID_NDK_TOOLCHAIN_HOST_TAG}
		-android-abis ${CMAKE_ANDROID_ARCH_ABI} -xplatform android-clang)

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

ExternalProject_Add(qt
	DEPENDS openssl
	URL ${QT_URLS}
	URL_HASH SHA256=${QT_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${CMAKE_COMMAND} -DCOMPONENT=qt -P ${PATCH_CMAKE} &&
			${CMAKE_COMMAND} -E touch qtbase/.gitignore
	CONFIGURE_COMMAND ${QT_ENV} <SOURCE_DIR>/${QT_CONFIGURE} ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_SKIP_MODULES}
	BUILD_COMMAND ${MAKE} ${MAKE_JOBS}
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
