#####################################################################
# The module card is responsible to handle all generic card commands
# (APDU).
#
# It provides an interface that can be implemented by plugins like
# PCSC or Android-NFC called ReaderManagerPlugIn.
# Also it provides a ReaderManager that can be used as a generic
# control interface. It handles access and commands to specific
# reader, cards and commands to the identity card.
#####################################################################

add_subdirectory(base)

if(DESKTOP)
	add_subdirectory(pcsc)
endif()

if(TARGET ${Qt}::Nfc)
	add_subdirectory(nfc)
endif()

if(IOS)
	add_subdirectory(ios)
endif()

add_subdirectory(drivers)
