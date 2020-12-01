#####################################################################
# The activation plugin with a local HTTP-WebServer.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationWebservice)

target_link_libraries(AusweisAppActivationWebservice ${Qt}::Core AusweisAppGlobal AusweisAppNetwork AusweisAppActivation)
target_compile_definitions(AusweisAppActivationWebservice PRIVATE QT_STATICPLUGIN)
