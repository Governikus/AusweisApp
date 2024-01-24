ADD_PLATFORM_LIBRARY(AusweisAppTestHelperCommon)

target_link_libraries(AusweisAppTestHelperCommon ${Qt}::Network ${Qt}::Test AusweisAppExternal::HttpParser)
target_link_libraries(AusweisAppTestHelperCommon AusweisAppCardSimulator AusweisAppNetwork AusweisAppCore)
target_link_libraries(AusweisAppTestHelperCommon AusweisAppWorkflowsSelfAuth)

target_compile_definitions(AusweisAppTestHelperCommon PRIVATE QT_STATICPLUGIN)

if(DESKTOP)
	target_link_libraries(AusweisAppTestHelperCommon AusweisAppCardDrivers)
endif()

# Enable QtHooks
target_include_directories(AusweisAppTestHelperCommon SYSTEM PRIVATE ${Qt6Core_PRIVATE_INCLUDE_DIRS})
