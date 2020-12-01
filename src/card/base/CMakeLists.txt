ADD_PLATFORM_LIBRARY(AusweisAppCard)

target_link_libraries(AusweisAppCard ${Qt}::Core OpenSSL::Crypto AusweisAppGlobal AusweisAppConfiguration AusweisAppCardDrivers)
