#####################################################################
# The activation plugin for Android's intent mechanism.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationIntent)

TARGET_LINK_LIBRARIES(AusweisAppActivationIntent Qt5::Core Qt5::Gui AusweisAppGlobal AusweisAppActivation)
TARGET_COMPILE_DEFINITIONS(AusweisAppActivationIntent PRIVATE QT_STATICPLUGIN)
