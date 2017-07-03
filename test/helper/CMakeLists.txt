ADD_PLATFORM_LIBRARY(AusweisAppTestHelper)

TARGET_INCLUDE_DIRECTORIES(AusweisAppTestHelper SYSTEM PUBLIC ${PCSC_INCLUDE_DIRS} ${OPENSSL_INCLUDE_DIR})
TARGET_LINK_LIBRARIES(AusweisAppTestHelper Qt5::Network Qt5::Xml Qt5::Widgets Qt5::Test AusweisAppActivation AusweisAppExternalQHttpServer AusweisAppCard AusweisAppNetwork)
TARGET_COMPILE_DEFINITIONS(AusweisAppTestHelper PRIVATE QT_STATICPLUGIN)
