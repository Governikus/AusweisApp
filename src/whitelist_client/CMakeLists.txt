########################################################################
# The module whitelist client is responsible for collecting information
# about the NFC interface on an Android device and sending them to the
# whitelist server.
#######################################################################

ADD_PLATFORM_LIBRARY(AusweisAppWhitelistClient)

TARGET_LINK_LIBRARIES(AusweisAppWhitelistClient Qt5::Core AusweisAppSettings AusweisAppNetwork)
