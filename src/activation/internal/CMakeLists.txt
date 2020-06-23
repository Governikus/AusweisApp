#####################################################################
# The activation plugin for internal usage.
#
# If another module needs to start an authentication it can be
# invoked by the plugin.
# This will be used by internal usage of the SDK.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationInternal)

TARGET_LINK_LIBRARIES(AusweisAppActivationInternal Qt5::Core AusweisAppGlobal AusweisAppActivation)
TARGET_COMPILE_DEFINITIONS(AusweisAppActivationInternal PRIVATE QT_STATICPLUGIN)
