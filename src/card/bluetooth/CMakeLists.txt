ADD_PLATFORM_LIBRARY(AusweisAppCardBluetooth)

TARGET_LINK_LIBRARIES(AusweisAppCardBluetooth Qt5::Core Qt5::Bluetooth AusweisAppGlobal AusweisAppCard)
TARGET_COMPILE_DEFINITIONS(AusweisAppCardBluetooth PRIVATE QT_STATICPLUGIN)

IF(ANDROID)
	TARGET_LINK_LIBRARIES(AusweisAppCardBluetooth Qt5::AndroidExtras)
ENDIF()
