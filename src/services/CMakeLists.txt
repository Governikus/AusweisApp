#####################################################################
# The module services is responsible to trigger periodic background
# services like checks for application updates or updates for
# configuration files.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppServices)

target_link_libraries(AusweisAppServices ${Qt}::Core AusweisAppConfiguration AusweisAppFileProvider)
