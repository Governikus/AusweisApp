#####################################################################
# The module secure storage is responsible for persistent
# configuration of the released application. It is delivered as a
# read-only file.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppSecureStorage)

TARGET_LINK_LIBRARIES(AusweisAppSecureStorage Qt5::Core Qt5::Network AusweisAppGlobal)
