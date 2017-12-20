ADD_PLATFORM_LIBRARY(AusweisAppServices)

TARGET_LINK_LIBRARIES(AusweisAppServices Qt5::Core AusweisAppCard AusweisAppNetwork)

IF(DESKTOP)
    TARGET_LINK_LIBRARIES(AusweisAppServices AusweisAppConfiguration)
ENDIF()
