#####################################################################
# The widget plugin implements the ui interface for desktop systems.
#
# It is a wrapper to QWidgets that allows to use the platform
# specific styles for the underlying system.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiWidget)

TARGET_LINK_LIBRARIES(AusweisAppUiWidget Qt5::Core Qt5::Widgets Qt5::Svg AusweisAppGlobal AusweisAppCore AusweisAppUi AusweisAppExport AusweisAppRemoteDevice AusweisAppConfiguration AusweisAppUiCommon)
TARGET_COMPILE_DEFINITIONS(AusweisAppUiWidget PRIVATE QT_STATICPLUGIN)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppUiWidget Qt5::WinExtras)
ENDIF()
