ADD_PLATFORM_LIBRARY(AusweisAppSettings)

TARGET_LINK_LIBRARIES(AusweisAppSettings Qt5::Core Qt5::Network OpenSSL::Crypto AusweisAppGlobal)

IF(MAC)
	TARGET_LINK_LIBRARIES(AusweisAppSettings ${OSX_APPKIT})
ENDIF()

IF(ANDROID)
	TARGET_LINK_LIBRARIES(AusweisAppSettings Qt5::AndroidExtras)
ENDIF()
