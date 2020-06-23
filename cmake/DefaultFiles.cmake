SET(REMOTE_CONFIG_URL_PROD https://appl.governikus-asp.de/ausweisapp2)
IF(JENKINS_APPCAST)
	SET(REMOTE_CONFIG_URL https://ausweisapp-ci.govkg.de/job/${JENKINS_APPCAST}/lastSuccessfulBuild/artifact CACHE STRING "Remote config download URL" FORCE)
ELSE()
	SET(REMOTE_CONFIG_URL ${REMOTE_CONFIG_URL_PROD} CACHE STRING "Remote config download URL" FORCE)
ENDIF()

FUNCTION(CONFIGURE_DEFAULT_FILES _destination)
	IF(JENKINS_APPCAST)
		SET(REMOTE_CONFIG_PATH_APPCAST /build)
		SET(REMOTE_CONFIG_PATH_APPCAST_BETA /build)
	ELSE()
		SET(REMOTE_CONFIG_PATH_APPCAST )
		SET(REMOTE_CONFIG_PATH_APPCAST_BETA /beta)
	ENDIF()

	# Copy secure storage file, so that the AusweisApp2 can be started from the build directory.
	CONFIGURE_FILE(${RESOURCES_DIR}/config.json.in ${_destination}/config.json @ONLY)
ENDFUNCTION()
