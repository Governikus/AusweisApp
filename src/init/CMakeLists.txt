#####################################################################
# The module init is responsible to bootstrap the appplication.
#####################################################################

ADD_PLATFORM_LIBRARY(AusweisAppInit)

TARGET_LINK_LIBRARIES(AusweisAppInit Qt5::Core OpenSSL::Crypto AusweisAppGlobal AusweisAppCore)

IF(NOT INTEGRATED_SDK)
	TARGET_LINK_LIBRARIES(AusweisAppInit Qt5::Gui) # QGuiApplication
	IF(DESKTOP)
		TARGET_LINK_LIBRARIES(AusweisAppInit Qt5::Widgets) # QApplication
	ENDIF()
ENDIF()


FUNCTION(CONFIGURE_CONFIG_H)
	IF(VENDOR_GOVERNIKUS)
		SET(VENDOR_DOMAIN governikus.com)
	ELSE()
		SET(VENDOR_DOMAIN)
	ENDIF()

	IF(VERSION_DVCS)
		SET(VERSION ${VERSION_DVCS})
	ELSE()
		SET(VERSION ${PROJECT_VERSION})
	ENDIF()

	SET(PRODUCT ${PROJECT_NAME})

	CONFIGURE_FILE(config.h.in config.h @ONLY)
ENDFUNCTION()

CONFIGURE_CONFIG_H()
