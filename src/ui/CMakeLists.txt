#####################################################################
# The module ui provides a lean interface that will be implemented
# by different plugins. It provides a generic API that will be used
# for communication between core and ui.
#####################################################################

add_subdirectory(base)

add_subdirectory(json)
add_subdirectory(aidl)

if(TARGET ${Qt}::WebSockets)
	add_subdirectory(websocket)
endif()

if(NOT ANDROID AND INTEGRATED_SDK)
	add_subdirectory(functional)
endif()

if(TARGET ${Qt}::Gui)
	add_subdirectory(common)
endif()

if(TARGET ${Qt}::Qml)
	add_subdirectory(qml)
endif()
