#####################################################################
# The ReaderManagerPlugin for QtNfc.
#
# This plugin is a generic wrapper around Qt's NFC module.
# Supported: Android
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardNfc)

target_link_libraries(AusweisAppCardNfc ${Qt}::Core ${Qt}::Nfc AusweisAppGlobal AusweisAppCard)
target_compile_definitions(AusweisAppCardNfc PRIVATE QT_STATICPLUGIN)

if(ANDROID)
	target_link_libraries(AusweisAppCardNfc ${Qt}::AndroidExtras)
endif()
