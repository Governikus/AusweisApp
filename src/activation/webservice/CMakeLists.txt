#####################################################################
# The activation plugin with a local HTTP-WebServer.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationWebservice)

TARGET_LINK_LIBRARIES(AusweisAppActivationWebservice Qt5::Core AusweisAppGlobal AusweisAppNetwork AusweisAppActivation)
TARGET_COMPILE_DEFINITIONS(AusweisAppActivationWebservice PRIVATE QT_STATICPLUGIN)
