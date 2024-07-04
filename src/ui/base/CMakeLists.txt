ADD_PLATFORM_LIBRARY(AusweisAppUi)

target_link_libraries(AusweisAppUi ${Qt}::Core ${Qt}::Network AusweisAppGlobal)
qt_extract_metatypes(AusweisAppUi) # QTBUG-123574
