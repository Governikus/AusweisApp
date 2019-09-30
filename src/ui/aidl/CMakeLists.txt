#####################################################################
# The aidl plugin implements the ui interface for Android's AIDL.
#
# It provides a background service for the SDK.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiAidl)

TARGET_LINK_LIBRARIES(AusweisAppUiAidl Qt5::Core AusweisAppUiJson AusweisAppGlobal)
TARGET_COMPILE_DEFINITIONS(AusweisAppUiAidl PRIVATE QT_STATICPLUGIN)
