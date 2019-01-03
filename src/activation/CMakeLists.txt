#####################################################################
# The module activation is responsible to enable different kinds of
# eID activation. This interface will be implemented by plugins
# for the specific platform.
#####################################################################

ADD_SUBDIRECTORY(base)

ADD_SUBDIRECTORY(customscheme)
ADD_SUBDIRECTORY(intent)
ADD_SUBDIRECTORY(webservice)
ADD_SUBDIRECTORY(internal)
