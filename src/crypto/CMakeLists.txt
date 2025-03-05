#####################################################################
# The module crypto is responsible for openssl crypto stuff.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppCrypto)

target_link_libraries(AusweisAppCrypto ${Qt}::Core OpenSSL::Crypto ${OSX_SECURITY})
target_link_libraries(AusweisAppCrypto AusweisAppGlobal)
