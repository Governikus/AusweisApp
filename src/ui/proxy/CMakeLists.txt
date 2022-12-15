#####################################################################
# The proxy plugin implements the reverse proxy interface for a
# a terminal server or other required forwarder.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiProxy)

target_link_libraries(AusweisAppUiProxy ${Qt}::Core)
target_link_libraries(AusweisAppUiProxy AusweisAppGlobal AusweisAppUi AusweisAppNetwork)
target_compile_definitions(AusweisAppUiProxy PRIVATE QT_STATICPLUGIN)
