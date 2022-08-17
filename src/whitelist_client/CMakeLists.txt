########################################################################
# The module whitelist client is responsible for collecting information
# about the NFC interface on an Android device and sending them to the
# whitelist server.
#######################################################################

ADD_PLATFORM_LIBRARY(AusweisAppWhitelistClient)

target_link_libraries(AusweisAppWhitelistClient ${Qt}::Core AusweisAppSettings AusweisAppNetwork AusweisAppCard)
