#####################################################################
# The aidl plugin implements the ui interface for Android's AIDL.
#
# It provides a background service for the SDK.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiAidl)

target_link_libraries(AusweisAppUiAidl ${Qt}::Core AusweisAppUiJson AusweisAppGlobal)
target_compile_definitions(AusweisAppUiAidl PRIVATE QT_STATICPLUGIN)
