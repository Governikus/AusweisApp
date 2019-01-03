#####################################################################
# The module global is responsible for general utilities like
# logging or loader for resources and translations.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppGlobal)

TARGET_LINK_LIBRARIES(AusweisAppGlobal Qt5::Core OpenSSL::Crypto ${OSX_SECURITY})

IF(ANDROID)
	TARGET_LINK_LIBRARIES(AusweisAppGlobal Qt5::AndroidExtras)
ENDIF()
