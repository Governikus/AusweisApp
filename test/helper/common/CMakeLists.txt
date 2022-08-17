ADD_PLATFORM_LIBRARY(AusweisAppTestHelperCommon)

target_link_libraries(AusweisAppTestHelperCommon ${Qt}::Network ${Qt}::Test AusweisAppExternal::HttpParser)
target_link_libraries(AusweisAppTestHelperCommon AusweisAppActivation AusweisAppCard AusweisAppNetwork AusweisAppCore)
target_link_libraries(AusweisAppTestHelperCommon AusweisAppWorkflowsSelfAuth)

target_compile_definitions(AusweisAppTestHelperCommon PRIVATE QT_STATICPLUGIN)

if(DESKTOP)
	target_link_libraries(AusweisAppTestHelperCommon AusweisAppCardDrivers)
endif()

# Enable QtHooks
if(QT5)
	set(QtCore_PRIVATE_INCLUDE_DIRS "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
elseif(QT6)
	set(QtCore_PRIVATE_INCLUDE_DIRS "${Qt6Core_PRIVATE_INCLUDE_DIRS}")
endif()

target_include_directories(AusweisAppTestHelperCommon SYSTEM PRIVATE ${QtCore_PRIVATE_INCLUDE_DIRS})
