#####################################################################
# The module file provider is responsible to maintain file updates
# and their dependencies. It will trigger downloads and checks
# if a download is necessary.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppFileProvider)

TARGET_LINK_LIBRARIES(AusweisAppFileProvider Qt5::Core AusweisAppSecureStorage AusweisAppNetwork)
