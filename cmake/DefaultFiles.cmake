IF(JENKINS_APPCAST)
	SET(REMOTE_CONFIG_URL https://buildautentapp/job/${JENKINS_APPCAST}/lastSuccessfulBuild/artifact CACHE STRING "Remote config download URL" FORCE)
ELSE()
	SET(REMOTE_CONFIG_URL https://appl.governikus-asp.de/ausweisapp2 CACHE STRING "Remote config download URL" FORCE)
ENDIF()

FUNCTION(CONFIGURE_DEFAULT_FILES _destination)
	# Set DEFAULT_PROVIDER_FILE to replace this in config.json.in.
	IF(IOS)
		SET(DEFAULT_PROVIDER_FILE default-providers-ios.json)
	ELSE()
		SET(DEFAULT_PROVIDER_FILE default-providers.json)
	ENDIF()

	IF(JENKINS_APPCAST)
		SET(REMOTE_CONFIG_PATH_APPCAST /build)
		SET(REMOTE_CONFIG_PATH_APPCAST_BETA /build)
		SET(REMOTE_CONFIG_PATH_PROVIDERS /source/resources)
		SET(REMOTE_CONFIG_PATH_DRIVERS /source/resources)
	ELSE()
		SET(REMOTE_CONFIG_PATH_APPCAST )
		SET(REMOTE_CONFIG_PATH_APPCAST_BETA /beta)
		SET(REMOTE_CONFIG_PATH_PROVIDERS )
		SET(REMOTE_CONFIG_PATH_DRIVERS /driver)
	ENDIF()

	# Copy secure storage file, so that the AusweisApp2 can be started from the build directory.
	CONFIGURE_FILE(${RESOURCES_DIR}/config.json.in ${_destination}/config.json @ONLY)

	# The same with default providers
	CONFIGURE_FILE(${RESOURCES_DIR}/${DEFAULT_PROVIDER_FILE} ${_destination}/default-providers.json COPYONLY)

	# The same with default supported devices
	CONFIGURE_FILE(${RESOURCES_DIR}/default-supported-devices.json ${_destination}/default-supported-devices.json COPYONLY)

	# Copy qtlogging.ini file
	CONFIGURE_FILE(${RESOURCES_DIR}/qtlogging.ini ${_destination}/qtlogging.ini COPYONLY)
ENDFUNCTION()
