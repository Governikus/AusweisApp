ADD_PLATFORM_LIBRARY(AusweisAppWidget)

TARGET_LINK_LIBRARIES(AusweisAppWidget Qt5::Widgets Qt5::Svg Qt5::PrintSupport Qt5::QuickWidgets AusweisAppCore AusweisAppGlobal AusweisAppQml AusweisAppExport)
TARGET_COMPILE_DEFINITIONS(AusweisAppWidget PRIVATE QT_STATICPLUGIN)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppWidget Qt5::WinExtras)
ENDIF()
