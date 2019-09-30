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

ADD_SUBDIRECTORY(base)

IF(DESKTOP)
	ADD_SUBDIRECTORY(pcsc)
ENDIF()

IF(TARGET Qt5::Nfc)
	ADD_SUBDIRECTORY(nfc)
ENDIF()

IF(IOS)
        ADD_SUBDIRECTORY(ios)
ENDIF()

IF(TARGET Qt5::Bluetooth)
	ADD_SUBDIRECTORY(bluetooth)
ENDIF()

ADD_SUBDIRECTORY(drivers)
