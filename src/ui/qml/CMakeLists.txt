#####################################################################
# The qml plugin implements the ui interface for mobile systems.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiQml)

TARGET_LINK_LIBRARIES(AusweisAppUiQml Qt5::Core Qt5::Svg Qt5::Qml Qt5::Quick Qt5::QuickControls2)
TARGET_LINK_LIBRARIES(AusweisAppUiQml AusweisAppGlobal AusweisAppCore AusweisAppUi AusweisAppRemoteDevice AusweisAppUiCommon AusweisAppExport)

IF(ANDROID)
	TARGET_LINK_LIBRARIES(AusweisAppUiQml AusweisAppWhitelistClient)
ELSE()
	TARGET_LINK_LIBRARIES(AusweisAppUiQml debug AusweisAppWhitelistClient)
ENDIF()

IF(TARGET Qt5::Bluetooth)
	TARGET_LINK_LIBRARIES(AusweisAppUiQml Qt5::Bluetooth)
ENDIF()

TARGET_COMPILE_DEFINITIONS(AusweisAppUiQml PRIVATE QT_STATICPLUGIN)
