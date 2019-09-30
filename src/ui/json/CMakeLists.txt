#####################################################################
# The json plugin implements the ui interface for internal usage.
#
# This plugin will be used by other ui plugins like AIDL or WebSocket
# to provide the SDK functionality. It provides no possibility for
# direct user control.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiJson)

TARGET_LINK_LIBRARIES(AusweisAppUiJson Qt5::Core AusweisAppCore AusweisAppGlobal AusweisAppActivationInternal)
TARGET_COMPILE_DEFINITIONS(AusweisAppUiJson PRIVATE QT_STATICPLUGIN)
