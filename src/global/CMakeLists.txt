#####################################################################
# The module global is responsible for general utilities like
# logging or loader for resources and translations.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppGlobal)

target_link_libraries(AusweisAppGlobal ${Qt}::Core OpenSSL::Crypto ${OSX_SECURITY})
