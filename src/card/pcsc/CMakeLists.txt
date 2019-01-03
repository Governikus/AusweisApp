#####################################################################
# The ReaderManagerPlugin for PCSC.
#
# This plugin is a wrapper around different PCSC implementations.
# Supported: Windows, macOS and pcsclite on Unix/Linux.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardPcsc)

TARGET_INCLUDE_DIRECTORIES(AusweisAppCardPcsc SYSTEM PUBLIC ${PCSC_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(AusweisAppCardPcsc Qt5::Core AusweisAppGlobal AusweisAppCard ${PCSC_LIBRARIES})
TARGET_COMPILE_DEFINITIONS(AusweisAppCardPcsc PRIVATE QT_STATICPLUGIN)
