#####################################################################
# The functional plugin implements the ui interface for callbacks.
#
# This plugin can be used as a SDK that will be linked into another
# binary.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiFunctional)

target_link_libraries(AusweisAppUiFunctional ${Qt}::Core AusweisAppUi AusweisAppUiJson AusweisAppGlobal AusweisAppInit)
target_compile_definitions(AusweisAppUiFunctional PRIVATE QT_STATICPLUGIN)

set_target_properties(AusweisAppUiFunctional PROPERTIES PUBLIC_HEADER "AusweisApp2.h")
