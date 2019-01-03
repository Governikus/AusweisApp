#####################################################################
# The module remote devices is responsible for the feature
# "smartphone as card reader". It provides an interface to control
# pairing and connection. Also it implements a ReaderManagerPlugin
# to provide a wrapper of a card reader.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppRemoteDevice)

TARGET_LINK_LIBRARIES(AusweisAppRemoteDevice Qt5::Core Qt5::WebSockets AusweisAppCard AusweisAppGlobal AusweisAppSecureStorage AusweisAppNetwork AusweisAppSettings)
TARGET_COMPILE_DEFINITIONS(AusweisAppRemoteDevice PRIVATE QT_STATICPLUGIN)
