if(NOT PACKAGES_DIR)
	set(PACKAGES_DIR $ENV{PACKAGES_DIR})
	if(NOT PACKAGES_DIR)
		set(PACKAGES_DIR ${CMAKE_BINARY_DIR})
	endif()
endif()
message(STATUS "Use PACKAGES_DIR: ${PACKAGES_DIR}")

set(SONARSCANNERCLI_VERSION 7.1.0.4889-linux-x64) # https://binaries.sonarsource.com/?prefix=Distribution/sonar-scanner-cli/
set(SONARSCANNERCLI_ZIP_NAME sonar-scanner-cli-${SONARSCANNERCLI_VERSION}.zip)
set(SONARSCANNERCLI_URL https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/${SONARSCANNERCLI_ZIP_NAME})
set(SONARSCANNERCLI_HASH b4d2a001d65b489f9effe1ea8a78495db1b152f124d7f7b058aad8651c7e1484)

set(DEPENDENCYCHECK_VERSION 12.1.3) # https://github.com/dependency-check/DependencyCheck
set(DEPENDENCYCHECK_ZIP_NAME dependency-check-${DEPENDENCYCHECK_VERSION}-release.zip)
set(DEPENDENCYCHECK_URL https://github.com/dependency-check/DependencyCheck/releases/download/v${DEPENDENCYCHECK_VERSION}/${DEPENDENCYCHECK_ZIP_NAME})
set(DEPENDENCYCHECK_HASH c79149ab46ce24b2c69d4734caa1afa4e62f128eec719733bb8f0eb406bdd0d6)

set(MARIADB_CONNECTOR_VERSION 3.5.3)
set(MARIADB_CONNECTOR_ZIP_NAME mariadb-java-client-${MARIADB_CONNECTOR_VERSION}.jar)
set(MARIADB_CONNECTOR_URL https://downloads.mariadb.com/Connectors/java/connector-java-${MARIADB_CONNECTOR_VERSION}/${MARIADB_CONNECTOR_ZIP_NAME})
set(MARIADB_CONNECTOR_HASH 85c4ba2f221d0dfd439c26affbb294f784960763544263c65aba9c2c76858706)

set(SONARQUBETOOLS_DIR ${WORKSPACE}/sonarqubetools)
step(${CMAKE_COMMAND} -E make_directory ${SONARQUBETOOLS_DIR})

function(DOWNLOAD_AND_EXTRACT NAME URL ZIP_NAME HASH EXTRACT_DIR RESULT_DIR EXTRACT)
	if(EXISTS "${SONARQUBETOOLS_DIR}/${RESULT_DIR}" AND EXTRACT)
		message(STATUS "Removing previous ${NAME}")
		step(${CMAKE_COMMAND} -E rm -rf ${SONARQUBETOOLS_DIR}/${RESULT_DIR})
	endif()

	if(NOT EXISTS "${PACKAGES_DIR}/${ZIP_NAME}" OR NOT HASH)
		message(STATUS "Download ${NAME}: ${ZIP_NAME}")
		file(DOWNLOAD ${URL} ${PACKAGES_DIR}/${ZIP_NAME})
	endif()

	if(HASH)
		file(SHA256 ${PACKAGES_DIR}/${ZIP_NAME} FILE_HASH)
		if(NOT "${FILE_HASH}" STREQUAL "${HASH}")
			message(FATAL_ERROR "${NAME} hash does not match! Current: ${FILE_HASH}, expected: ${HASH}")
		endif()
	endif()

	message(STATUS "Install ${NAME}: ${ZIP_NAME}")
	if(EXTRACT)
		step(${CMAKE_COMMAND} -E tar xf ${PACKAGES_DIR}/${ZIP_NAME} CHDIR ${SONARQUBETOOLS_DIR})
		if(NOT "${EXTRACT_DIR}" STREQUAL "${RESULT_DIR}")
			step(${CMAKE_COMMAND} -E rename ${SONARQUBETOOLS_DIR}/${EXTRACT_DIR} ${SONARQUBETOOLS_DIR}/${RESULT_DIR})
		endif()
	else()
		step(${CMAKE_COMMAND} -E copy_if_different ${PACKAGES_DIR}/${MARIADB_CONNECTOR_ZIP_NAME} ${SONARQUBETOOLS_DIR}/${RESULT_DIR})
	endif()
endfunction()

DOWNLOAD_AND_EXTRACT("SonarScanner" ${SONARSCANNERCLI_URL} ${SONARSCANNERCLI_ZIP_NAME} ${SONARSCANNERCLI_HASH} "sonar-scanner-${SONARSCANNERCLI_VERSION}" "sonar-scanner" TRUE)
DOWNLOAD_AND_EXTRACT("Dependency Check" ${DEPENDENCYCHECK_URL} ${DEPENDENCYCHECK_ZIP_NAME} ${DEPENDENCYCHECK_HASH} "dependency-check" "dependency-check" TRUE)
DOWNLOAD_AND_EXTRACT("Dependency Check MariaDB Connector" ${MARIADB_CONNECTOR_URL} ${MARIADB_CONNECTOR_ZIP_NAME} ${MARIADB_CONNECTOR_HASH} "dependency-check/lib/" "dependency-check/lib/" FALSE)


set(CACHE_DIR "${WORKSPACE}/cache")
if(NOT EXISTS ${CACHE_DIR})
	step(${CMAKE_COMMAND} -E make_directory ${CACHE_DIR})
endif()

step(${T_CFG} --preset ci-linux)

step(${SONARQUBETOOLS_DIR}/dependency-check/bin/dependency-check.sh
	--enableExperimental -f HTML -f JSON --scan ${CMAKE_DIR} --noupdate
	--connectionString=jdbc:mariadb://dependency-check-db.govkg.de/dependencycheck
	--dbUser=$ENV{DEPENDENCY_CHECK_USER}
	--dbPassword=$ENV{DEPENDENCY_CHECK_PASSWORD}
	--dbDriverName=org.mariadb.jdbc.Driver
	CHDIR ${T_BUILD_DIR}
)

step(${T_BUILD})

step(${T_CTEST} -LE qml -E Test_ui_qml_Qml)

step(${T_TARGET} gcovr.sonar)



if(REVIEW)
	set(SONAR_CMDLINE
		-Dsonar.pullrequest.key=$ENV{REVIEWBOARD_REVIEW_ID}
		-Dsonar.pullrequest.branch=$ENV{MERCURIAL_REVISION_BRANCH}
		-Dsonar.pullrequest.base=$ENV{MERCURIAL_REVISION_BRANCH}
	)
else()
	set(SONAR_CMDLINE
		-Dsonar.branch.name=$ENV{MERCURIAL_REVISION_BRANCH}
	)
endif()

step(
	${SONARQUBETOOLS_DIR}/sonar-scanner/bin/sonar-scanner
	-Dsonar.scanner.metadataFilePath=${T_TEMP}/sonar-metadata.txt
	${SONAR_CMDLINE}
	-Dsonar.token=$ENV{SONARQUBE_TOKEN}
	-Dsonar.qualitygate.wait=true
	-Dsonar.qualitygate.timeout=90
	CHDIR ${T_BUILD_DIR}
)
