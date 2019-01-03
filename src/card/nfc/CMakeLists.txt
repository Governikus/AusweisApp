#####################################################################
# The ReaderManagerPlugin for QtNfc.
#
# This plugin is a generic wrapper around Qt's NFC module.
# Supported: Android
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardNfc)

TARGET_LINK_LIBRARIES(AusweisAppCardNfc Qt5::Core Qt5::Nfc AusweisAppGlobal AusweisAppCard)
TARGET_COMPILE_DEFINITIONS(AusweisAppCardNfc PRIVATE QT_STATICPLUGIN)

IF(ANDROID)
	TARGET_LINK_LIBRARIES(AusweisAppCardNfc Qt5::AndroidExtras)
ENDIF()
