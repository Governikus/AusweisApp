#####################################################################
# The module network is responsible for all raw network stuff and
# checks for TLS security.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppNetwork)

target_link_libraries(AusweisAppNetwork ${Qt}::Core ${Qt}::Network AusweisAppExternal::HttpParser AusweisAppGlobal AusweisAppSecureStorage AusweisAppSettings)
