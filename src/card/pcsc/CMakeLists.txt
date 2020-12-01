#####################################################################
# The ReaderManagerPlugin for PCSC.
#
# This plugin is a wrapper around different PCSC implementations.
# Supported: Windows, macOS and pcsclite on Unix/Linux.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardPcsc)

target_include_directories(AusweisAppCardPcsc SYSTEM PUBLIC ${PCSC_INCLUDE_DIRS})
target_link_libraries(AusweisAppCardPcsc ${Qt}::Core AusweisAppGlobal AusweisAppCard ${PCSC_LIBRARIES})
target_compile_definitions(AusweisAppCardPcsc PRIVATE QT_STATICPLUGIN)
