#####################################################################
# The webservice plugin implements the ui interface for a webservice.
#
# This provides an HTTP-Server with /eID-Client.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiWebservice)

target_link_libraries(AusweisAppUiWebservice ${Qt}::Core)
target_link_libraries(AusweisAppUiWebservice AusweisAppGlobal AusweisAppUi AusweisAppWorkflows AusweisAppNetwork)
target_compile_definitions(AusweisAppUiWebservice PRIVATE QT_STATICPLUGIN)
