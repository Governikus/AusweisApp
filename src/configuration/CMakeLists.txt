#####################################################################
# The module configuration is responsible to maintain the setup of
# all dynamic content that can be updated by a configuration file
# on our server.
#
# For example: Configuration of provider and reader driver.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppConfiguration)

TARGET_LINK_LIBRARIES(AusweisAppConfiguration Qt5::Core AusweisAppGlobal AusweisAppFileProvider)
