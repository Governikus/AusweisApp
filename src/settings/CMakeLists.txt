#####################################################################
# The module settings is responsible for user settings.
# They will be stored with QSettings using platform specific format.
#
# Any user changeable settings will be saved and loaded by this
# module.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppSettings)

target_link_libraries(AusweisAppSettings ${Qt}::Core ${Qt}::Network ${Qt}::Concurrent OpenSSL::Crypto AusweisAppGlobal)

if(MAC)
	target_link_libraries(AusweisAppSettings ${OSX_APPKIT} ${OSX_SERVICEMANAGEMENT})
endif()

if(ANDROID)
	target_link_libraries(AusweisAppSettings ${Qt}::AndroidExtras)
endif()
