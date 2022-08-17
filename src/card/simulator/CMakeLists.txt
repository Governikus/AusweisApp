#####################################################################
# The ReaderManagerPlugin for the simulator.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardSimulator)

target_link_libraries(AusweisAppCardSimulator ${Qt}::Core AusweisAppGlobal AusweisAppCard)
target_compile_definitions(AusweisAppCardSimulator PRIVATE QT_STATICPLUGIN)

