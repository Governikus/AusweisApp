#####################################################################
# The activation plugin for custom scheme eid:// on iOS.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationCustomScheme)

TARGET_LINK_LIBRARIES(AusweisAppActivationCustomScheme Qt5::Core Qt5::Gui AusweisAppGlobal AusweisAppActivation)
TARGET_COMPILE_DEFINITIONS(AusweisAppActivationCustomScheme PRIVATE QT_STATICPLUGIN)
