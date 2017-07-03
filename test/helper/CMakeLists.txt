ADD_PLATFORM_LIBRARY(AusweisAppTestHelper)

TARGET_INCLUDE_DIRECTORIES(AusweisAppTestHelper SYSTEM PUBLIC ${PCSC_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
# TODO: Remove the QmlTestRunner from the lib AusweisAppTestHelper, currently it's included and therefore Qt5::QuickTest is needed as linked lib
TARGET_LINK_LIBRARIES(AusweisAppTestHelper Qt5::Network Qt5::Xml Qt5::Test AusweisAppActivation AusweisAppCard AusweisAppCardRemote AusweisAppNetwork AusweisAppCore AusweisAppWebSocket Qt5::QuickTest)
TARGET_COMPILE_DEFINITIONS(AusweisAppTestHelper PRIVATE QT_STATICPLUGIN)

IF(DESKTOP)
	TARGET_LINK_LIBRARIES(AusweisAppTestHelper Qt5::Widgets)
ENDIF()

ADD_EXECUTABLE(QmlTestRunner QmlTestRunner.cpp)
TARGET_LINK_LIBRARIES(QmlTestRunner Qt5::QuickTest Qt5::Gui)
