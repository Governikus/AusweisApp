#####################################################################
# The module remote devices is responsible for the feature
# "smartphone as card reader". It provides an interface to control
# pairing and connection.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppIfd)

target_link_libraries(AusweisAppIfd ${Qt}::Core ${Qt}::WebSockets AusweisAppCard AusweisAppGlobal AusweisAppSecureStorage AusweisAppNetwork AusweisAppSettings)
