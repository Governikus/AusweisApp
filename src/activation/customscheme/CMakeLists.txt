#####################################################################
# The activation plugin for custom scheme eid:// on iOS.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationCustomScheme)

target_link_libraries(AusweisAppActivationCustomScheme ${Qt}::Core ${Qt}::Gui AusweisAppGlobal AusweisAppActivation)
target_compile_definitions(AusweisAppActivationCustomScheme PRIVATE QT_STATICPLUGIN)
