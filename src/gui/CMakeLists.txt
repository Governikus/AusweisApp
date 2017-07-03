ADD_PLATFORM_LIBRARY(AusweisAppWidget)

TARGET_LINK_LIBRARIES(AusweisAppWidget Qt5::Widgets Qt5::Svg Qt5::PrintSupport AusweisAppCore AusweisAppGlobal)
TARGET_COMPILE_DEFINITIONS(AusweisAppWidget PRIVATE QT_STATICPLUGIN)
