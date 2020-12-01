#####################################################################
# The module secure storage is responsible for persistent
# configuration of the released application. It is delivered as a
# read-only file.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppSecureStorage)

target_link_libraries(AusweisAppSecureStorage ${Qt}::Core ${Qt}::Network AusweisAppGlobal)
