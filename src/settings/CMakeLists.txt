#####################################################################
# The module settings is responsible for user settings.
# They will be stored with QSettings using platform specific format.
#
# Any user changeable settings will be saved and loaded by this
# module.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppSettings)

target_link_libraries(AusweisAppSettings ${Qt}::Core ${Qt}::Network ${Qt}::Concurrent OpenSSL::Crypto AusweisAppGlobal AusweisAppCrypto)
target_compile_definitions(AusweisAppSettings PRIVATE VENDOR_PATH="${VENDOR_PATH}")

if(MACOS)
	target_link_libraries(AusweisAppSettings ${OSX_APPKIT} ${OSX_SERVICEMANAGEMENT})
endif()

if(IOS)
	target_link_libraries(AusweisAppSettings ${IOS_UIKIT})
endif()
