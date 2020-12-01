#####################################################################
# The module file provider is responsible to maintain file updates
# and their dependencies. It will trigger downloads and checks
# if a download is necessary.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppFileProvider)

target_link_libraries(AusweisAppFileProvider ${Qt}::Core AusweisAppSecureStorage AusweisAppNetwork)
