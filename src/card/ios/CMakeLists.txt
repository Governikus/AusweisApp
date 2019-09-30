#####################################################################
# The ReaderManagerPlugin for iOS Nfc.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardIos)

TARGET_LINK_LIBRARIES(AusweisAppCardIos Qt5::Core AusweisAppGlobal AusweisAppCard)
TARGET_COMPILE_DEFINITIONS(AusweisAppCardIos PRIVATE QT_STATICPLUGIN)
