#####################################################################
# The module services is responsible to trigger periodic background
# services like checkes for appplication updates or updates for
# configuration files.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppServices)

TARGET_LINK_LIBRARIES(AusweisAppServices Qt5::Core AusweisAppConfiguration AusweisAppFileProvider)
