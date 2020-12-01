add_library(AusweisAppTestHelper INTERFACE)

add_subdirectory(common)
target_link_libraries(AusweisAppTestHelper INTERFACE AusweisAppTestHelperCommon)

if(TARGET AusweisAppRemoteDevice)
	add_subdirectory(remote_device)

	# TestModule detection misses indirect dependency currently
	target_link_libraries(AusweisAppTestHelper INTERFACE AusweisAppTestHelperRemoteDevice)
endif()

add_subdirectory(ui)

# TestModule detection misses indirect dependency currently
if(TARGET AusweisAppTestHelperUiWebsocket)
	target_link_libraries(AusweisAppTestHelper INTERFACE AusweisAppTestHelperUiWebsocket)
endif()
