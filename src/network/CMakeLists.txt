#####################################################################
# The module network is responsible for all raw network stuff and
# checks for TLS security.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppNetwork)

TARGET_LINK_LIBRARIES(AusweisAppNetwork Qt5::Core Qt5::Network AusweisAppExternal::HttpParser AusweisAppGlobal AusweisAppSecureStorage AusweisAppSettings)
