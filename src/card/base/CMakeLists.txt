ADD_PLATFORM_LIBRARY(AusweisAppCard)

target_link_libraries(AusweisAppCard ${Qt}::Core OpenSSL::Crypto AusweisAppGlobal AusweisAppConfiguration)

if(DESKTOP)
	target_link_libraries(AusweisAppCard AusweisAppCardDrivers)
endif()
