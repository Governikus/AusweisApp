#####################################################################
# The activation plugin for Android's intent mechanism.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppActivationIntent)

target_link_libraries(AusweisAppActivationIntent ${Qt}::Core ${Qt}::Gui AusweisAppGlobal AusweisAppActivation)
target_compile_definitions(AusweisAppActivationIntent PRIVATE QT_STATICPLUGIN)
