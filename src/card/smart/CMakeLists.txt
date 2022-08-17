#####################################################################
# The ReaderManagerPlugin for the Smart-eID card.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardSmart)

target_link_libraries(AusweisAppCardSmart ${Qt}::Core AusweisAppGlobal AusweisAppCard AusweisAppExternal::SmartID)
target_compile_definitions(AusweisAppCardSmart PRIVATE QT_STATICPLUGIN)

