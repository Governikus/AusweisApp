ADD_PLATFORM_LIBRARY(AusweisAppTestHelperCommon)

if(DESKTOP)
	target_include_directories(AusweisAppTestHelperCommon SYSTEM PUBLIC ${PCSC_INCLUDE_DIRS})
endif()

target_link_libraries(AusweisAppTestHelperCommon ${Qt}::Network ${Qt}::Test AusweisAppExternal::HttpParser AusweisAppActivation AusweisAppCard AusweisAppNetwork AusweisAppCore)
target_link_libraries(AusweisAppTestHelperCommon AusweisAppCardDrivers)
target_compile_definitions(AusweisAppTestHelperCommon PRIVATE QT_STATICPLUGIN)

if(TARGET ${Qt}::Gui)
	target_link_libraries(AusweisAppTestHelperCommon AusweisAppUiCommon)
endif()



# Enable QtHooks
if(QT5)
	set(QtCore_PRIVATE_INCLUDE_DIRS "${Qt5Core_PRIVATE_INCLUDE_DIRS}")
elseif(QT6)
	set(QtCore_PRIVATE_INCLUDE_DIRS "${Qt6Core_PRIVATE_INCLUDE_DIRS}")
endif()

target_include_directories(AusweisAppTestHelperCommon SYSTEM PRIVATE ${QtCore_PRIVATE_INCLUDE_DIRS})
