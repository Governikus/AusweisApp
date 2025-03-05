#####################################################################
# The module ui provides a lean interface that will be implemented
# by different plugins. It provides a generic API that will be used
# for communication between core and ui.
#####################################################################

add_subdirectory(base)

add_subdirectory(json)
add_subdirectory(automatic)
add_subdirectory(webservice)
add_subdirectory(proxy)

if(TARGET ${Qt}::Gui)
	add_subdirectory(aidl)
	add_subdirectory(scheme)
endif()

if(TARGET ${Qt}::WebSockets)
	add_subdirectory(websocket)
	add_subdirectory(local_ifd)
endif()

if(NOT ANDROID AND INTEGRATED_SDK)
	add_subdirectory(functional)
endif()

if(TARGET ${Qt}::Qml)
	add_subdirectory(qml)
endif()
