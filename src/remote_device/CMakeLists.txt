#####################################################################
# The module remote devices is responsible for the feature
# "smartphone as card reader". It provides an interface to control
# pairing and connection. Also it implements a ReaderManagerPlugin
# to provide a wrapper of a card reader.
#####################################################################

if(TARGET ${Qt}::WebSockets)
	ADD_PLATFORM_LIBRARY(AusweisAppRemoteDevice)

	target_link_libraries(AusweisAppRemoteDevice ${Qt}::Core ${Qt}::WebSockets AusweisAppCard AusweisAppGlobal AusweisAppSecureStorage AusweisAppNetwork AusweisAppSettings)
	target_compile_definitions(AusweisAppRemoteDevice PRIVATE QT_STATICPLUGIN)
endif()
