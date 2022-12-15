# Set CMAKE_PREFIX_PATH with toolchain directory

if(MINGW AND CMAKE_VERSION VERSION_LESS 3.17.0)
	set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll.a" ".a" ".lib")
endif()

if(DESKTOP)
	set(MIN_QT_VERSION 5.15)
else()
	set(MIN_QT_VERSION 6.2)
endif()

if(IOS OR ANDROID)
	if(NOT QT_HOST_PATH)
		# If no QT_HOST_PATH is set, see if it was build as part of the toolchain
		foreach(path ${CMAKE_PREFIX_PATH})
			set(QT_HOST_PATH ${path}/qt-host/)
			if(EXISTS QT_HOST_PATH)
				break()
			endif()
		endforeach()
	endif()

	if(NOT EXISTS ${QT_HOST_PATH})
		message(FATAL_ERROR "QT_HOST_PATH does not exist")
	endif()

	message(STATUS "Using QT_HOST_PATH: ${QT_HOST_PATH}")
	set(CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} ${QT_HOST_PATH} )
endif()

if(NOT DEFINED Qt)
	find_package(Qt6 ${MIN_QT_VERSION} COMPONENTS Core CMAKE_FIND_ROOT_PATH_BOTH)
	if(TARGET Qt6::Core)
		set(Qt Qt6)
	else()
		set(Qt Qt5)
	endif()
endif()
find_package(${Qt} ${MIN_QT_VERSION} COMPONENTS Core Concurrent Network REQUIRED CMAKE_FIND_ROOT_PATH_BOTH)
include(Compat)

if(QT6)
	list(APPEND QT_COMPONENTS StateMachine)
endif()

if(NOT CONTAINER_SDK)
	list(APPEND QT_COMPONENTS LinguistTools)
endif()

if(NOT INTEGRATED_SDK)
	list(APPEND QT_COMPONENTS Svg WebSockets Qml Quick QuickControls2 QuickTemplates2)

	if(QT_VERSION VERSION_GREATER_EQUAL "5.14")
		list(APPEND QT_COMPONENTS QmlWorkerScript)
	endif()
	if(NOT DESKTOP AND NOT QT6)
		list(APPEND QT_COMPONENTS QuickShapes)
	endif()
	if(QT6)
		list(APPEND QT_COMPONENTS ShaderTools)
	endif()
endif()

if(DESKTOP AND NOT INTEGRATED_SDK)
	list(APPEND QT_COMPONENTS Widgets)
endif()

if(ANDROID OR IOS OR WINDOWS_STORE OR (QT6 AND CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
	list(APPEND QT_COMPONENTS Nfc)
endif()

if(ANDROID)
	if(INTEGRATED_SDK)
		list(APPEND QT_COMPONENTS WebSockets)
	endif()
elseif(CONTAINER_SDK)
	list(APPEND QT_COMPONENTS WebSockets)
endif()


QUERY_QMAKE(QT_INSTALL_ARCHDATA QT_INSTALL_ARCHDATA)
QUERY_QMAKE(QT_INSTALL_TRANSLATIONS QT_INSTALL_TRANSLATIONS)

message(STATUS "QT_INSTALL_ARCHDATA: ${QT_INSTALL_ARCHDATA}")
message(STATUS "QT_INSTALL_TRANSLATIONS: ${QT_INSTALL_TRANSLATIONS}")

set(QT_VENDOR_FILE "${QT_INSTALL_ARCHDATA}/mkspecs/qt_vendor_governikus")
if(EXISTS "${QT_VENDOR_FILE}")
	set(QT_VENDOR "Governikus")
	message(STATUS "QT_VENDOR: ${QT_VENDOR}")
endif()

if(NOT DEFINED QT_INSTALL_TRANSLATIONS)
	message(FATAL_ERROR "Cannot detect QT_INSTALL_TRANSLATIONS")
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
	find_package(PCSC REQUIRED)
	set(WIN_DEFAULT_LIBS "-ladvapi32" "-lkernel32" "-lole32" "-lsetupapi" "-lversion" "-liphlpapi")
elseif(MSVC OR CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
	find_package(PCSC REQUIRED)
	set(WIN_DEFAULT_LIBS setupapi.lib version.lib iphlpapi.lib)
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
	find_package(PCSC REQUIRED)

	find_library(OSX_USERNOTIFICATIONS UserNotifications)
	find_library(OSX_APPKIT AppKit)
	find_library(IOKIT NAMES IOKit)
	find_library(OSX_SECURITY Security)
	find_library(OSX_FOUNDATION Foundation)
	find_library(OSX_SERVICEMANAGEMENT ServiceManagement)
elseif(UNIX)
	if(LINUX OR BSD)
		find_package(PkgConfig REQUIRED)
		pkg_check_modules(UDEV IMPORTED_TARGET libudev)
		if(NOT TARGET PkgConfig::UDEV)
			message(STATUS "Hardware detection disabled - Missing libudev")
		endif()
	endif()

	find_package(PCSC REQUIRED)
endif()


if(BUILD_TESTING AND NOT CONTAINER_SDK)
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

if(LINUX OR BSD)
	try_run(testResult compileResult "${CMAKE_BINARY_DIR}" "${CMAKE_DIR}/tests/openssl.cpp"
		CMAKE_FLAGS INSTALL_RPATH_USE_LINK_PATH:BOOL=ON
		LINK_LIBRARIES ${Qt}::Network OpenSSL::Crypto OpenSSL::SSL
		OUTPUT_VARIABLE runResult)
	if(NOT testResult EQUAL 0)
		message(FATAL_ERROR "Your OpenSSL library looks incompatible: ${testResult}\n${runResult}")
	endif()
endif()
