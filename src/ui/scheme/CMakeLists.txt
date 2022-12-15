#####################################################################
# The scheme plugin implements for custom scheme eid://
# on Android and iOS.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiScheme)

target_link_libraries(AusweisAppUiScheme ${Qt}::Core ${Qt}::Gui)
target_link_libraries(AusweisAppUiScheme AusweisAppGlobal AusweisAppUi AusweisAppWorkflows)
target_compile_definitions(AusweisAppUiScheme PRIVATE QT_STATICPLUGIN)
