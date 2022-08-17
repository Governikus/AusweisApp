######################################################################
# The automatic plugin implements fallback mechanism in SDK mode only.
######################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiAutomatic)

target_link_libraries(AusweisAppUiAutomatic ${Qt}::Core AusweisAppUi AusweisAppCard AusweisAppWorkflows AusweisAppGlobal)
target_compile_definitions(AusweisAppUiAutomatic PRIVATE QT_STATICPLUGIN)

if(CONTAINER_SDK)
	set(CONTAINER_SDK_DEFAULT true)
else()
	set(CONTAINER_SDK_DEFAULT false)
endif()

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/metadata.json.in" "${CMAKE_CURRENT_BINARY_DIR}/metadata.json" @ONLY)
