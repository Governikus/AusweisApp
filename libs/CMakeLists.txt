CMAKE_MINIMUM_REQUIRED(VERSION 3.5.0)

IF(POLICY CMP0010)
	CMAKE_POLICY(SET CMP0010 NEW)
ENDIF()

IF(POLICY CMP0020)
	CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()

IF(NOT ANDROID AND NOT IOS)
	SET(COMPILER_SEARCH C CXX)
ENDIF()

PROJECT(AusweisApp2_Libs ${COMPILER_SEARCH})

SET(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../cmake")
INCLUDE(ExternalProject)
INCLUDE(Helper)
INCLUDE(DVCS)

IF(ANDROID)
	GET_ANDROID_TOOLCHAIN_VARS(ANDROID_TOOLCHAIN_PREFIX ANDROID_TOOLCHAIN_MACHINE_NAME)
	IF(CMAKE_SYSROOT_COMPILE)
		SET(UNIFIED_INCLUDE ${CMAKE_SYSROOT_COMPILE}/usr/include)
	ENDIF()
ENDIF()


IF(MSVC)
	FIND_PROGRAM(MAKE nmake CMAKE_FIND_ROOT_PATH_BOTH)
ELSE()
	FIND_PROGRAM(MAKE make CMAKE_FIND_ROOT_PATH_BOTH)
ENDIF()

IF(MINGW AND NOT MAKE)
	FIND_PROGRAM(MAKE mingw32-make CMAKE_FIND_ROOT_PATH_BOTH)
ENDIF()

IF(MAKE)
	MESSAGE(STATUS "Using 'make' command... ${MAKE}")
ELSE()
	MESSAGE(FATAL_ERROR "Cannot find 'make' command")
ENDIF()

IF(NOT DEFINED PROCESSOR_COUNT)
	INCLUDE(ProcessorCount)
	ProcessorCount(PROCESSOR_COUNT)
ENDIF()

IF(NOT PROCESSOR_COUNT EQUAL 0 AND NOT "${MAKE}" MATCHES "nmake")
	SET(MAKE_JOBS -j${PROCESSOR_COUNT})
	MESSAGE(STATUS "PROCESSOR_COUNT: ${PROCESSOR_COUNT}")
ENDIF()

IF(CMAKE_BUILD_TYPE)
	STRING(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE)
ELSE()
	SET(CMAKE_BUILD_TYPE "DEBUG" CACHE STRING "build type configuration" FORCE)
ENDIF()

IF(NOT ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG" AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RELEASE" AND NOT ${CMAKE_BUILD_TYPE} STREQUAL "RELWITHDEBINFO")
	MESSAGE(FATAL_ERROR "CMAKE_BUILD_TYPE is invalid! Available options: RELEASE, RELWITHDEBINFO, DEBUG")
ENDIF()

IF(MSVC)
	FIND_HOST_PACKAGE(Perl REQUIRED)
ELSE()
	SET(PERL_EXECUTABLE perl)
ENDIF()

FIND_HOST_PACKAGE(PythonInterp 2.7 REQUIRED)
IF(NOT "${PYTHON_VERSION_MAJOR}" STREQUAL "2")
	MESSAGE(FATAL_ERROR "Cannot find 'python v2' command")
ENDIF()

FIND_PROGRAM(PATCH_CMD patch CMAKE_FIND_ROOT_PATH_BOTH)
IF(PATCH_CMD)
	SET(PATCH_OPTIONS -i)
	MESSAGE(STATUS "Using 'patch' command... ${PATCH_CMD}")
ELSE()
	SET(PATCH_CMD ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/patch.py --debug -v)
	MESSAGE(STATUS "Cannot find 'patch' command... using patch.py")
ENDIF()

IF(NOT DESTINATION_DIR)
	SET(DESTINATION_DIR ${PROJECT_BINARY_DIR}/dist)
ENDIF()

SET(PATCHES_DIR ${PROJECT_SOURCE_DIR}/../patches)

IF(NOT PACKAGES_DIR)
	SET(PACKAGES_DIR $ENV{PACKAGES_DIR})
	IF(NOT PACKAGES_DIR)
		MESSAGE(STATUS "Define PACKAGES_DIR for local packages")
		SET(PACKAGES_DIR ${PROJECT_BINARY_DIR}/download)
	ENDIF()
ENDIF()

STRING(REPLACE "\\" "/" PACKAGES_DIR ${PACKAGES_DIR})
SET_DIRECTORY_PROPERTIES(PROPERTIES EP_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/b)
INCLUDE(Messages)


################################## Versions
SET(QT 5.11.2)
SET(QT_HASH c6104b840b6caee596fa9a35bc5f57f67ed5a99d6a36497b6fe66f990a53ca81)

SET(OPENSSL 1.1.1)
SET(OPENSSL_HASH 2836875a0f89c03d0fdf483941512613a50cfb421d6fd94b9f41d7279d586a3d)

################################## Files
SET(QT_FILE qt-everywhere-src-${QT}.tar.xz)
SET(OPENSSL_FILE openssl-${OPENSSL}.tar.gz)

################################## Downloads
IF("${QT}" MATCHES "alpha|beta|rc")
	SET(QT_DEST_DIR development_releases)
ELSE()
	SET(QT_DEST_DIR official_releases)
ENDIF()
STRING(SUBSTRING ${QT} 0 4 QT_SUBVERSION)

SET(QT_URL https://download.qt.io/${QT_DEST_DIR}/qt/${QT_SUBVERSION}/${QT}/single)
SET(OPENSSL_URL https://www.openssl.org/source)

SET(ENABLED_TARGETS)


################################## OpenSSL
#########################################################################
LIST(APPEND ENABLED_TARGETS openssl)

SET(OPENSSL_CONFIGURE_FLAGS no-camellia no-bf no-aria no-seed no-poly1305 no-srp no-gost no-ocsp no-idea no-mdc2 no-rc2 no-rc4 no-rc5 no-srtp no-hw no-sm2 no-sm3 no-sm4)
SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-cast no-chacha no-blake2 no-rmd160 no-scrypt no-siphash no-whirlpool no-md4 no-des)
SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-tls1 no-tls1-method no-tls1_1 no-tls1_1-method no-tls1_3 no-ssl3 no-ssl3-method no-dtls no-dtls1-method no-dtls1_2-method)
SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-deprecated no-engine no-async no-dso no-comp no-ts no-makedepend no-tests shared)

IF(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
	SET(OPENSSL_CONFIGURE_FLAGS --debug ${OPENSSL_CONFIGURE_FLAGS})
ELSE()
	ADD_FLAG(-Os NOQUOTES VAR OPENSSL_COMPILER_FLAGS)
ENDIF()

ADD_FLAG(-fstack-protector-strong -fstack-protector NOQUOTES VAR OPENSSL_COMPILER_FLAGS)

IF(IOS)
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} ios64-cross)
	SET(OPENSSL_ENV export CROSS_TOP=${CMAKE_IOS_DEVELOPER_ROOT} && export CROSS_SDK=iPhoneOS.sdk &&)
ELSEIF(APPLE)
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} darwin64-x86_64-cc)
	SET(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mmacosx-version-min=10.11)
ELSEIF(MINGW)
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} mingw)
ELSEIF(MSVC)
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-asm VC-WIN32)
ELSEIF(ANDROID)
	IF(CMAKE_ANDROID_ARCH_ABI STREQUAL "armeabi-v7a")
		SET(OPENSSL_ARCH android-arm)
		SET(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -mfloat-abi=softfp)
	ELSEIF(CMAKE_ANDROID_ARCH_ABI STREQUAL "x86")
		SET(OPENSSL_ARCH android-x86)
	ELSEIF(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
		SET(OPENSSL_ARCH android-arm64)
	ELSE()
		MESSAGE(FATAL_ERROR "CMAKE_ANDROID_ARCH_ABI not supported by openssl")
	ENDIF()
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} no-stdio ${OPENSSL_ARCH})

	GET_FILENAME_COMPONENT(toolchain_bin "${CMAKE_C_COMPILER}" DIRECTORY)
	SET(OPENSSL_ENV export PATH=${toolchain_bin}/:$ENV{PATH} &&)
	IF(NOT CMAKE_COMPILER_IS_GNUCXX)
		SET(OPENSSL_ENV ${OPENSSL_ENV} export CC=clang && export CXX=clang++ &&)
	ENDIF()

	IF(UNIFIED_INCLUDE)
		SET(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -isystem${UNIFIED_INCLUDE} -isystem${UNIFIED_INCLUDE}/${ANDROID_TOOLCHAIN_MACHINE_NAME})
	ENDIF()
	SET(OPENSSL_COMPILER_FLAGS ${OPENSSL_COMPILER_FLAGS} -D__ANDROID_API__=${CMAKE_SYSTEM_VERSION})
ELSEIF(BSD)
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} BSD-x86_64)
ELSEIF(LINUX)
	IF(CMAKE_SYSTEM_PROCESSOR STREQUAL "i686")
		SET(OPENSSL_ARCH linux-generic32)
	ELSE()
		SET(OPENSSL_ARCH linux-x86_64)
	ENDIF()
	SET(OPENSSL_CONFIGURE_FLAGS ${OPENSSL_CONFIGURE_FLAGS} ${OPENSSL_ARCH})
ELSE()
	MESSAGE(FATAL_ERROR "Unsupported system")
ENDIF()

ExternalProject_Add(openssl
	URL ${OPENSSL_URL}/${OPENSSL_FILE}
	URL_HASH SHA256=${OPENSSL_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/openssl-android-shlib_variant.patch &&
				${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/openssl-Ignore-disabled-ciphers.patch &&
				${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/openssl-DSA-mod-inverse-fix.patch &&
				${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/openssl-Timing-vulnerability-in-DSA-signature-generation-CVE.patch &&
				${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/openssl-Timing-vulnerability-in-ECDSA-signature-generation-C.patch
	CONFIGURE_COMMAND ${OPENSSL_ENV} ${PERL_EXECUTABLE} Configure --prefix=${DESTINATION_DIR} ${OPENSSL_CONFIGURE_FLAGS} "${OPENSSL_COMPILER_FLAGS}"
	BUILD_COMMAND ${OPENSSL_ENV} ${MAKE} ${MAKE_JOBS}
	BUILD_IN_SOURCE 1
	INSTALL_COMMAND ${OPENSSL_ENV} ${MAKE} ${MAKE_JOBS} install_sw
)

ExternalProject_Add_Step(openssl configdata
	COMMAND ${PERL_EXECUTABLE} configdata.pm --dump
	DEPENDEES configure
	DEPENDERS build
	WORKING_DIRECTORY <BINARY_DIR>)


IF(MAC)
	SET(OPENSSL_FILE_VERSION 1.1)
	ADD_CUSTOM_COMMAND(TARGET openssl POST_BUILD
		COMMAND install_name_tool -id libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}
		COMMAND install_name_tool -id libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX}
		COMMAND install_name_tool -change ${DESTINATION_DIR}/lib/libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} libcrypto.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX} ${DESTINATION_DIR}/lib/libssl.${OPENSSL_FILE_VERSION}${CMAKE_SHARED_LIBRARY_SUFFIX})
ENDIF()

IF(IOS)
	# Remove this work-around! Do not build any .dylib or be able to use .dylib
	# Globbing is not supported by cmake command mode! This will work if executed with unix shell only.
	ADD_CUSTOM_COMMAND(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${DESTINATION_DIR}/lib/*.dylib)
ELSEIF(ANDROID)
	ADD_CUSTOM_COMMAND(TARGET openssl POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove ${DESTINATION_DIR}/lib/*.a)
ENDIF()

################################## Qt
#########################################################################
LIST(APPEND ENABLED_TARGETS qt)

IF(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
	SET(QT_CONFIGURE_FLAGS -debug -qml-debug)
	SET(QT_PATCH_COMMAND ${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Enable-debug-output-for-OpenSSL.patch &&)
ELSE()
	SET(QT_CONFIGURE_FLAGS -release -optimize-size -no-qml-debug)
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} $<$<CONFIG:RelWithDebInfo>:-force-debug-info>)
ENDIF()

SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -prefix ${DESTINATION_DIR} -opensource -confirm-license -c++std c++11 -qt-zlib -no-mtdev -qt-libpng -qt-libjpeg -no-dbus -no-harfbuzz -qt-pcre -system-proxies -no-compile-examples -nomake examples -nomake tests -no-sql-sqlite -openssl-linked -I ${DESTINATION_DIR}/include -L ${DESTINATION_DIR}/lib)

IF(NOT ANDROID)
	LIST(APPEND NO_FEATURES bearermanagement)
ENDIF()

LIST(APPEND NO_FEATURES ftp paint_debug lcdnumber mdiarea)
LIST(APPEND NO_FEATURES calendarwidget colordialog cups dial fontcombobox fontdialog)
LIST(APPEND NO_FEATURES imageformat_bmp imageformat_ppm imageformat_xbm)
LIST(APPEND NO_FEATURES sharedmemory textodfwriter filesystemwatcher)
LIST(APPEND NO_FEATURES undocommand undogroup undostack undoview)
LIST(APPEND NO_FEATURES printer printdialog printpreviewdialog printpreviewwidget)
LIST(APPEND NO_FEATURES splashscreen syntaxhighlighter dom sql)
FOREACH(feature ${NO_FEATURES})
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-feature-${feature})
ENDFOREACH()

SET(QT_CONFIGURE_FLAGS_OTHER -no-journald -no-directfb -no-linuxfb)
SET(QT_CONFIGURE_FLAGS_SKIP_MODULES -skip qtwebglplugin -skip qtscxml -skip qtxmlpatterns -skip qtwebchannel -skip qtwebengine -skip qtscript -skip qtactiveqt -skip qtlocation -skip qtserialbus -skip qtserialport -skip qtgamepad -skip qtvirtualkeyboard -skip qtcanvas3d -skip qtcharts -skip qtdatavis3d -skip qt3d -skip qtpurchasing -skip qtwayland -skip qtremoteobjects -skip qtspeech -skip qtwebview -skip multimedia -skip qtquickcontrols)

SET(QT_CONFIGURE ./configure)
IF(IOS)
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -sdk iphoneos -xplatform macx-ios-clang)
ELSEIF(APPLE)
	FIND_PROGRAM(XCODE_SELECT xcode-select)
	IF(NOT XCODE_SELECT)
		MESSAGE(FATAL_ERROR "Cannot find xcode-select")
	ENDIF()

	EXECUTE_PROCESS(COMMAND ${XCODE_SELECT} -p OUTPUT_VARIABLE osx_dev_dir OUTPUT_STRIP_TRAILING_WHITESPACE)
	FILE(GLOB osx_sdk "${osx_dev_dir}/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.*")
	GET_FILENAME_COMPONENT(osx_sdk ${osx_sdk} NAME)
	STRING(REPLACE ".sdk" "" osx_sdk "${osx_sdk}")
	STRING(TOLOWER "${osx_sdk}" osx_sdk)
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -framework -sdk ${osx_sdk})
ELSEIF(WIN32)
	IF(MSVC)
		SET(QT_PLATFORM win32-msvc2015)
		SET(QT_OPENSSL OPENSSL_LIBS=-llibcrypto\ -llibssl)
	ELSE()
		SET(QT_PLATFORM win32-g++)
		SET(QT_OPENSSL OPENSSL_LIBS=-lcrypto\ -lssl)
	ENDIF()

	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_OPENSSL} -opengl dynamic -no-icu -no-sql-odbc -platform ${QT_PLATFORM})
	SET(QT_CONFIGURE configure.bat)
ELSEIF(ANDROID)
	IF(CMAKE_COMPILER_IS_GNUCXX)
		SET(ANDROID_XPLATFORM android-g++)
	ELSE()
		SET(ANDROID_XPLATFORM android-clang)
	ENDIF()
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER}
		-android-sdk ${ANDROID_SDK} -android-ndk ${CMAKE_ANDROID_NDK} -android-ndk-platform android-${CMAKE_SYSTEM_VERSION} -android-ndk-host ${CMAKE_ANDROID_NDK_TOOLCHAIN_HOST_TAG}
		-android-arch ${CMAKE_ANDROID_ARCH_ABI} -android-toolchain-version ${ANDROID_NDK_TOOLCHAIN_VERSION}
		-xplatform ${ANDROID_XPLATFORM})

	IF(CMAKE_ANDROID_ARCH_ABI STREQUAL "arm64-v8a")
		SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-use-gold-linker)
	ENDIF()

	SET(QT_ENV export OPENSSL_LIBS=-lcrypto-gov\ -lssl-gov &&)
ELSE()
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_OTHER} -no-libproxy)
ENDIF()

IF(IOS OR ANDROID)
	SET(QT_CONFIGURE_FLAGS ${QT_CONFIGURE_FLAGS} -no-widgets)
ENDIF()

ExternalProject_Add(qt
	DEPENDS openssl
	URL ${QT_URL}/${QT_FILE}
	URL_HASH SHA256=${QT_HASH}
	DOWNLOAD_DIR ${PACKAGES_DIR}

	PATCH_COMMAND ${QT_PATCH_COMMAND}
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Disable-unused-imageformats.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Add-IsoDep-to-the-techList-on-Android.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Introduce-reportError-to-fix-QMetaObject-invokeMethod.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Add-work-around-for-freebsd-build.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Remove-Qt-Labs-specific-plugins-from-the-build.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-configure-refactor-directx-checks.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-configure-detect-fxc.exe-more-thoroughly.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-disable-designer.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-fix-macOS-no-printer.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Use-QUrl-toString-when-forming-the-Host-header.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-QUrl-Support-IPv6-addresses-with-zone-id.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Android-fix-compile-with-NDK-r18.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Android-Fix-crash.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-QObject-Fix-isSignalConnected-when-signals-have-been.patch &&
			${PATCH_CMD} -p1 ${PATCH_OPTIONS} ${PATCHES_DIR}/qt-Use-user-provided-session-data-if-available.patch &&
			${CMAKE_COMMAND} -E touch qtbase/.gitignore
	CONFIGURE_COMMAND ${QT_ENV} ${QT_CONFIGURE} ${QT_CONFIGURE_FLAGS} ${QT_CONFIGURE_FLAGS_SKIP_MODULES}
	BUILD_COMMAND ${MAKE} ${MAKE_JOBS}
	BUILD_IN_SOURCE 1
)

ADD_CUSTOM_COMMAND(TARGET qt POST_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${DESTINATION_DIR}/mkspecs/qt_vendor_governikus)

#########################################################################

FOREACH(var ${ENABLED_TARGETS})
	EXTERNALPROJECT_GET_PROPERTY(${var} INSTALL_DIR)
	LIST(APPEND CLEAN_TARGETS ${INSTALL_DIR})
ENDFOREACH()
SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${DESTINATION_DIR};${CLEAN_TARGETS}")

OPTION(COMPRESS_DEPENDS "Disable DEPENDS for compress target" ON)
IF(COMPRESS_DEPENDS)
	SET(COMPRESS_TARGETS ${ENABLED_TARGETS})
ENDIF()

STRING(TIMESTAMP stamp "%Y-%m-%d")
FIND_DVCS(${PROJECT_SOURCE_DIR}/..)
IF(DVCS_FOUND)
	GET_DVCS_INFO()

	IF(DEFINED dvcs_phase)
		SET(stamp ${stamp}_${dvcs_phase})
	ENDIF()

	IF(DEFINED dvcs_revision)
		SET(stamp ${stamp}_${dvcs_revision})
	ENDIF()
ENDIF()

IF(IOS)
	SET(SYSTEM_NAME iOS)
ELSEIF(ANDROID)
	SET(SYSTEM_NAME ${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ID}_${CMAKE_ANDROID_ARCH_ABI})
ELSE()
	SET(SYSTEM_NAME ${CMAKE_SYSTEM_NAME}_${CMAKE_CXX_COMPILER_ID})
ENDIF()

IF(WIN32)
	IF(SIGNTOOL_CMD)
		CONFIGURE_FILE(${CMAKE_MODULE_PATH}/SignFiles.cmake.in ${CMAKE_BINARY_DIR}/SignFiles.cmake @ONLY)
		SET(SIGN_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_BINARY_DIR}/SignFiles.cmake")
	ENDIF()
ENDIF()

SET(COMPRESSION cfJ)
SET(COMPRESSION_FILENDING tar.xz)
ADD_CUSTOM_TARGET(compress.pre ${compressed_filename}
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${DESTINATION_DIR}/doc"
		COMMAND ${CMAKE_COMMAND} -E remove_directory "${DESTINATION_DIR}/share"
		COMMAND ${SIGN_COMMAND}
		DEPENDS ${COMPRESS_TARGETS}
		WORKING_DIRECTORY "${DESTINATION_DIR}")

SET(compressed_filename Toolchain_${SYSTEM_NAME}_${stamp}.${COMPRESSION_FILENDING})
ADD_CUSTOM_COMMAND(OUTPUT ${compressed_filename}
		COMMAND ${CMAKE_COMMAND} -E tar "${COMPRESSION}" "${compressed_filename}" "${DESTINATION_DIR}"
		DEPENDS compress.pre)
ADD_CUSTOM_TARGET(compress DEPENDS ${compressed_filename})
