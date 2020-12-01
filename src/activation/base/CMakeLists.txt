ADD_PLATFORM_LIBRARY(AusweisAppActivation)

target_link_libraries(AusweisAppActivation ${Qt}::Core AusweisAppExternal::HttpParser AusweisAppGlobal AusweisAppNetwork AusweisAppUi)
