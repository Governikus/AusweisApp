#####################################################################
# The module card drivers.
#
# This module provides an interface to scan for connected card
# readers and their specific drivers.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCardDrivers)

target_link_libraries(AusweisAppCardDrivers ${Qt}::Core AusweisAppGlobal AusweisAppConfiguration)

if(WIN32)
	target_link_libraries(AusweisAppCardDrivers ${WIN_DEFAULT_LIBS})
endif()

if(MAC)
	target_link_libraries(AusweisAppCardDrivers ${IOKIT})
endif()

if(TARGET PkgConfig::UDEV)
	target_link_libraries(AusweisAppCardDrivers PkgConfig::UDEV)
endif()
