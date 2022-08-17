#####################################################################
# The module implements a remote IFD ReaderManagerPlugin
# to provide a wrapper of a IFD card reader used for SaC.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppIfdRemote)

target_link_libraries(AusweisAppIfdRemote ${Qt}::Core ${Qt}::WebSockets AusweisAppGlobal AusweisAppIfd)
target_compile_definitions(AusweisAppIfdRemote PRIVATE QT_STATICPLUGIN)
