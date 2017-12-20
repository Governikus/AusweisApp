ADD_PLATFORM_LIBRARY(AusweisAppTestHelper)

TARGET_INCLUDE_DIRECTORIES(AusweisAppTestHelper SYSTEM PUBLIC ${PCSC_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(AusweisAppTestHelper Qt5::Network Qt5::Xml Qt5::Test AusweisAppActivation AusweisAppCard AusweisAppCardDrivers AusweisAppCardRemote AusweisAppNetwork AusweisAppCore AusweisAppWebSocket)
TARGET_COMPILE_DEFINITIONS(AusweisAppTestHelper PRIVATE QT_STATICPLUGIN)

IF(DESKTOP)
	TARGET_LINK_LIBRARIES(AusweisAppTestHelper Qt5::Widgets)
ENDIF()
