#####################################################################
# The ReaderManagerPlugin for PCSC.
#
# This plugin is a wrapper around different PCSC implementations.
# Supported: Windows, macOS and pcsclite on Unix/Linux.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardPcsc)

target_link_libraries(AusweisAppCardPcsc PkgConfig::PCSC ${Qt}::Core AusweisAppGlobal AusweisAppCard)
target_compile_definitions(AusweisAppCardPcsc PRIVATE QT_STATICPLUGIN)
