#####################################################################
# The cli plugin implements the ui interface for a command line.
#
# It is an experimental interface that can be controled by a
# terminal/console. It is deprecated and should be replaced by
# the WebSocket plugin.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppUiCli)

TARGET_LINK_LIBRARIES(AusweisAppUiCli Qt5::Core AusweisAppGlobal AusweisAppCore AusweisAppUi AusweisAppActivationWebservice)
TARGET_COMPILE_DEFINITIONS(AusweisAppUiCli PRIVATE QT_STATICPLUGIN)
