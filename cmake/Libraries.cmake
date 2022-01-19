# Set CMAKE_PREFIX_PATH with toolchain directory

if(MINGW AND CMAKE_VERSION VERSION_LESS 3.17.0)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".a" ".lib")
endif()

if(DESKTOP)
	set(MIN_QT_VERSION 5.12)
else()
	set(MIN_QT_VERSION 5.15)
endif()

if(NOT DEFINED Qt)
	set(Qt Qt5)
endif()
find_package(${Qt} ${MIN_QT_VERSION} COMPONENTS Core Concurrent Network LinguistTools REQUIRED)
include(Compat)

if(QT6)
	list(APPEND QT_COMPONENTS StateMachine)
endif()

if(NOT INTEGRATED_SDK)
	list(APPEND QT_COMPONENTS Svg WebSockets Qml Quick QuickControls2 QuickTemplates2)

	if(QT_VERSION VERSION_GREATER_EQUAL "5.14")
		list(APPEND QT_COMPONENTS QmlWorkerScript)
		if(IOS)
			list(APPEND QT_COMPONENTS QmlImportScanner)
		endif()
	endif()
	if(NOT DESKTOP)
		list(APPEND QT_COMPONENTS QuickShapes)
	endif()
endif()

if(DESKTOP AND NOT INTEGRATED_SDK)
	list(APPEND QT_COMPONENTS Widgets)

	if(WIN32)
		list(APPEND QT_COMPONENTS WinExtras)
	endif()
endif()

if(ANDROID OR IOS OR WINDOWS_STORE OR CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	list(APPEND QT_COMPONENTS Nfc)
endif()

if(ANDROID)
	list(APPEND QT_COMPONENTS AndroidExtras)
endif()

foreach(dest "" "share/qt" "share/qt5" "share/qt6")
	if(EXISTS "${QT_HOST_PREFIX}/${dest}/translations")
		set(QT_TRANSLATIONS_DIR ${QT_HOST_PREFIX}/${dest}/translations)
	endif()
endforeach()

if(NOT QT_TRANSLATIONS_DIR)
	QUERY_QMAKE(QT_TRANSLATIONS_DIR QT_INSTALL_TRANSLATIONS)
endif()

message(STATUS "QT_HOST_PREFIX: ${QT_HOST_PREFIX}")
message(STATUS "QT_TRANSLATIONS_DIR: ${QT_TRANSLATIONS_DIR}")

set(QT_VENDOR_FILE "${QT_HOST_PREFIX}/mkspecs/qt_vendor_governikus")
if(EXISTS "${QT_VENDOR_FILE}")
	set(QT_VENDOR "Governikus")
	message(STATUS "QT_VENDOR: ${QT_VENDOR}")
endif()

if(NOT DEFINED QT_TRANSLATIONS_DIR)
	message(FATAL_ERROR "Cannot detect QT_TRANSLATIONS_DIR")
endif()


if(MINGW AND NOT CMAKE_CROSSCOMPILING)
	set(tmp_crosscompile_enabled TRUE)
	set(CMAKE_CROSSCOMPILING ON)
endif()

set(MIN_OPENSSL_VERSION 1.1)
find_package(OpenSSL ${MIN_OPENSSL_VERSION} REQUIRED)

if(tmp_crosscompile_enabled)
	set(CMAKE_CROSSCOMPILING OFF)
endif()


if(MINGW)
	set(PCSC_LIBRARIES -lwinscard)
	set(WIN_DEFAULT_LIBS "-ladvapi32" "-lkernel32" "-lole32" "-lsetupapi" "-lversion")
elseif(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
	set(PCSC_LIBRARIES winscard.lib)
	set(WIN_DEFAULT_LIBS setupapi.lib version.lib)
elseif(ANDROID)

elseif(IOS)
	find_library(IOS_ASSETSLIBRARY AssetsLibrary)
	find_library(IOS_UIKIT UIKit)
	find_library(IOS_STOREKIT StoreKit)
	find_library(IOS_MOBILECORESERVICES MobileCoreServices)
	find_library(IOS_COREFOUNDATION CoreFoundation)
	find_library(IOS_OPENGLES OpenGLES)
	find_library(IOS_FOUNDATION Foundation)
	find_library(IOS_QUARTZCORE QuartzCore)
	find_library(IOS_CORETEXT CoreText)
	find_library(IOS_COREGRAPHICS CoreGraphics)
	find_library(IOS_SECURITY Security)
	find_library(IOS_NETWORK Network)
	find_library(IOS_AUDIOTOOLBOX AudioToolbox)
	find_library(IOS_IMAGEIO ImageIO)
	find_library(IOS_CORENFC CoreNFC)
	find_library(IOS_MESSAGEUI MessageUI)
elseif(MAC)
	find_path(PCSC_INCLUDE_DIRS WinSCard.h)
	find_library(PCSC_LIBRARIES NAMES PCSC WinSCard)

	find_library(OSX_APPKIT AppKit)
	find_library(IOKIT NAMES IOKit)
	find_library(OSX_SECURITY Security)
	find_library(OSX_FOUNDATION Foundation)
	find_library(OSX_SERVICEMANAGEMENT ServiceManagement)
elseif(UNIX)
	find_package(PkgConfig REQUIRED)

	if(LINUX)
		pkg_check_modules(UDEV IMPORTED_TARGET libudev)
		if(NOT TARGET PkgConfig::UDEV)
			message(STATUS "Hardware detection disabled - Missing libudev")
		endif()
	endif()

	pkg_check_modules(PCSC REQUIRED libpcsclite)
	link_directories("${PCSC_LIBRARY_DIRS}")
endif()


if(BUILD_TESTING)
	list(APPEND QT_COMPONENTS Test)

	if(INTEGRATED_SDK)
		find_package(Threads REQUIRED)
	endif()

	if(NOT INTEGRATED_SDK)
		list(APPEND QT_COMPONENTS QuickTest)
	endif()
endif()

if(QT_COMPONENTS)
	find_package(${Qt} ${MIN_QT_VERSION} COMPONENTS ${QT_COMPONENTS} REQUIRED)
endif()
