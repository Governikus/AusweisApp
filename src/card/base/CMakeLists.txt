ADD_PLATFORM_LIBRARY(AusweisAppCard)

target_link_libraries(AusweisAppCard PUBLIC ${Qt}::Core OpenSSL::Crypto AusweisAppGlobal AusweisAppConfiguration)

if(DESKTOP)
	target_link_libraries(AusweisAppCard PUBLIC AusweisAppCardDrivers)
endif()

if(TARGET ${Qt}::Test)
	target_link_libraries(AusweisAppCard PRIVATE debug ${Qt}::Test)
endif()

qt_extract_metatypes(AusweisAppCard) # QTBUG-123574
