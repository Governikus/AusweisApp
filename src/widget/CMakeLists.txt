ADD_PLATFORM_LIBRARY(AusweisAppWidget)

TARGET_LINK_LIBRARIES(AusweisAppWidget Qt5::Core Qt5::Widgets Qt5::Svg AusweisAppCore AusweisAppGlobal AusweisAppExport AusweisAppRemoteDevice AusweisAppConfiguration)
TARGET_COMPILE_DEFINITIONS(AusweisAppWidget PRIVATE QT_STATICPLUGIN)

IF(WIN32)
	TARGET_LINK_LIBRARIES(AusweisAppWidget Qt5::WinExtras)
ENDIF()
