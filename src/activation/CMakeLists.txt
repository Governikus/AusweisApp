#####################################################################
# The module activation is responsible to enable different kinds of
# eID activation. This interface will be implemented by plugins
# for the specific platform.
#####################################################################

add_subdirectory(base)

add_subdirectory(webservice)

if(TARGET ${Qt}::Gui)
	add_subdirectory(customscheme)
	add_subdirectory(intent)
endif()
