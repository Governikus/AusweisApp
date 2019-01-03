#####################################################################
# The module settings is responsible for user settings.
# They will be stored with QSettings using platform specific format.
#
# Any user changeable settings will be saved and loaded by this
# module.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppSettings)

TARGET_LINK_LIBRARIES(AusweisAppSettings Qt5::Core Qt5::Network Qt5::Concurrent OpenSSL::Crypto AusweisAppGlobal)

IF(MAC)
	TARGET_LINK_LIBRARIES(AusweisAppSettings ${OSX_APPKIT} ${OSX_SERVICEMANAGEMENT})
ENDIF()

IF(ANDROID)
	TARGET_LINK_LIBRARIES(AusweisAppSettings Qt5::AndroidExtras)
ENDIF()
