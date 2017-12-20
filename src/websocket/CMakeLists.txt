IF(TARGET Qt5::WebSockets)
	ADD_PLATFORM_LIBRARY(AusweisAppWebSocket)

	TARGET_LINK_LIBRARIES(AusweisAppWebSocket Qt5::Core Qt5::WebSockets AusweisAppJsonApi AusweisAppGlobal)
	TARGET_COMPILE_DEFINITIONS(AusweisAppWebSocket PRIVATE QT_STATICPLUGIN)
ENDIF()
