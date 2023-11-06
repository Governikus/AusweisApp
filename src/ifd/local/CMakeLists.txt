#####################################################################
# The module implements a local IFD ReaderManagerPlugin to provide
# a wrapper of a IFD card reader used for third party apps.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppIfdLocal)

target_link_libraries(AusweisAppIfdLocal ${Qt}::Core ${Qt}::WebSockets AusweisAppGlobal AusweisAppIfd)
target_link_libraries(AusweisAppIfdLocal ${Qt}::CorePrivate)
target_compile_definitions(AusweisAppIfdLocal PRIVATE QT_STATICPLUGIN)
