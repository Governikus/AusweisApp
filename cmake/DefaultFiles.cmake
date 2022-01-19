set(REMOTE_CONFIG_URL_PROD https://updates.autentapp.de)
if(JENKINS_APPCAST)
	set(REMOTE_CONFIG_URL https://ausweisapp-ci.govkg.de/job/${JENKINS_APPCAST}/lastSuccessfulBuild/artifact CACHE STRING "Remote config download URL" FORCE)
else()
	set(REMOTE_CONFIG_URL ${REMOTE_CONFIG_URL_PROD} CACHE STRING "Remote config download URL" FORCE)
endif()

function(CONFIGURE_DEFAULT_FILES _destination)
	if(JENKINS_APPCAST)
		set(REMOTE_CONFIG_PATH_APPCAST /build)
		set(REMOTE_CONFIG_PATH_APPCAST_BETA /build)
	else()
		set(REMOTE_CONFIG_PATH_APPCAST )
		set(REMOTE_CONFIG_PATH_APPCAST_BETA /beta)
	endif()

	# Copy secure storage file, so that the AusweisApp2 can be started from the build directory.
	configure_file(${RESOURCES_DIR}/config.json.in ${_destination}/config.json @ONLY)
endfunction()
