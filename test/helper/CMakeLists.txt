add_library(AusweisAppTestHelper INTERFACE)

add_subdirectory(common)
target_link_libraries(AusweisAppTestHelper INTERFACE AusweisAppTestHelperCommon)

if(TARGET AusweisAppIfd)
	add_subdirectory(ifd)

	# TestModule detection misses indirect dependency currently
	target_link_libraries(AusweisAppTestHelper INTERFACE AusweisAppTestHelperIfd)
endif()

add_subdirectory(ui)
add_subdirectory(pcsc)

# TestModule detection misses indirect dependency currently
if(TARGET AusweisAppTestHelperUiWebsocket)
	target_link_libraries(AusweisAppTestHelper INTERFACE AusweisAppTestHelperUiWebsocket)
endif()
