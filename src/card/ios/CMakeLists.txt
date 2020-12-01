#####################################################################
# The ReaderManagerPlugin for iOS Nfc.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardIos)

target_link_libraries(AusweisAppCardIos ${Qt}::Core AusweisAppGlobal AusweisAppCard)
target_compile_definitions(AusweisAppCardIos PRIVATE QT_STATICPLUGIN)
