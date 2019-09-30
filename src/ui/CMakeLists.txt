#####################################################################
# The module ui provides a lean interface that will be implemented
# by different plugins. It provides a generic API that will be used
# for communication between core and ui.
#####################################################################

ADD_SUBDIRECTORY(base)
ADD_SUBDIRECTORY(common)

# Use this if we can use QSvgPlugin without Widgets
# IF(TARGET Qt5::Widgets)
# https://bugreports.qt.io/browse/QTBUG-41884
IF(DESKTOP)
	ADD_SUBDIRECTORY(widget)
ENDIF()

ADD_SUBDIRECTORY(json)
ADD_SUBDIRECTORY(aidl)
ADD_SUBDIRECTORY(websocket)

IF(TARGET Qt5::Qml)
	ADD_SUBDIRECTORY(qml)
ENDIF()
