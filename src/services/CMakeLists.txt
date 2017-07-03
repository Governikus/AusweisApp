ADD_PLATFORM_LIBRARY(AusweisAppServices)

TARGET_LINK_LIBRARIES(AusweisAppServices Qt5::Core AusweisAppCard AusweisAppNetwork AusweisAppSettings)

IF(NOT ANDROID AND NOT IOS)
	TARGET_LINK_LIBRARIES(AusweisAppServices AusweisAppExternalFervor)
ENDIF()
