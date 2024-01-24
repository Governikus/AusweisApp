cmake_minimum_required(VERSION 3.14.0)

include(FetchContent)

if(NOT PACKAGES_DIR)
	set(PACKAGES_DIR $ENV{PACKAGES_DIR})
	if(NOT PACKAGES_DIR)
		set(PACKAGES_DIR ${CMAKE_BINARY_DIR})
	endif()
endif()
message(STATUS "Use PACKAGES_DIR: ${PACKAGES_DIR}")

set(BUILDWRAPPER_ZIP_NAME build-wrapper-linux-x86.zip)
set(BUILDWRAPPER_URL https://sonar.govkg.de/static/cpp/${BUILDWRAPPER_ZIP_NAME})

set(SONARSCANNERCLI_VERSION 5.0.1.3006-linux) # https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/
set(SONARSCANNERCLI_ZIP_NAME sonar-scanner-cli-${SONARSCANNERCLI_VERSION}.zip)
set(SONARSCANNERCLI_URL https://binaries.sonarsource.com/Distribution/sonar-scanner-cli/${SONARSCANNERCLI_ZIP_NAME})
set(SONARSCANNERCLI_HASH 350dbdb517c10fcb3ce70425db95c415b313cad7296c407d416d88f3d50121f8)

set(DEPENDENCYCHECK_VERSION 8.4.0) # https://github.com/jeremylong/DependencyCheck/releases
set(DEPENDENCYCHECK_ZIP_NAME dependency-check-${DEPENDENCYCHECK_VERSION}-release.zip)
set(DEPENDENCYCHECK_URL https://github.com/jeremylong/DependencyCheck/releases/download/v${DEPENDENCYCHECK_VERSION}/${DEPENDENCYCHECK_ZIP_NAME})
set(DEPENDENCYCHECK_HASH 937a6bf8ced9d8494767082c1f588f26ea379324cb089dabb045321e8b0ab01a)

set(MARIADB_CONNECTOR_VERSION 3.2.0)
set(MARIADB_CONNECTOR_ZIP_NAME mariadb-java-client-${MARIADB_CONNECTOR_VERSION}.jar)
set(MARIADB_CONNECTOR_URL https://downloads.mariadb.com/Connectors/java/connector-java-${MARIADB_CONNECTOR_VERSION}/${MARIADB_CONNECTOR_ZIP_NAME})
set(MARIADB_CONNECTOR_HASH adf9df10bc9b2a137def36d6a495812258f430d4a8f7946727c61558e6c73941)

set(SONARQUBETOOLS_DIR ${CMAKE_BINARY_DIR}/sonarqubetools)

file(MAKE_DIRECTORY ${SONARQUBETOOLS_DIR})

function(DOWNLOAD_AND_EXTRACT NAME URL ZIP_NAME HASH EXTRACT_DIR RESULT_DIR EXTRACT)
	if(EXISTS "${SONARQUBETOOLS_DIR}/${RESULT_DIR}" AND EXTRACT)
		message(STATUS "Removing previous ${NAME}")
		file(REMOVE_RECURSE ${SONARQUBETOOLS_DIR}/${RESULT_DIR})
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
		execute_process(COMMAND ${CMAKE_COMMAND} -E tar xf ${PACKAGES_DIR}/${ZIP_NAME} WORKING_DIRECTORY ${SONARQUBETOOLS_DIR})
		if(NOT "${EXTRACT_DIR}" STREQUAL "${RESULT_DIR}")
			file(RENAME ${SONARQUBETOOLS_DIR}/${EXTRACT_DIR} ${SONARQUBETOOLS_DIR}/${RESULT_DIR})
		endif()
	else()
		file(COPY ${PACKAGES_DIR}/${MARIADB_CONNECTOR_ZIP_NAME} DESTINATION ${SONARQUBETOOLS_DIR}/${RESULT_DIR})
	endif()
endfunction()

DOWNLOAD_AND_EXTRACT("Build Wrapper" ${BUILDWRAPPER_URL} ${BUILDWRAPPER_ZIP_NAME} FALSE "build-wrapper-linux-x86" "sonar-build-wrapper" TRUE)
file(CREATE_LINK
	${SONARQUBETOOLS_DIR}/sonar-build-wrapper/libinterceptor-x86_64.so
	${SONARQUBETOOLS_DIR}/sonar-build-wrapper/libinterceptor-haswell.so
	SYMBOLIC
)

DOWNLOAD_AND_EXTRACT("SonarScanner" ${SONARSCANNERCLI_URL} ${SONARSCANNERCLI_ZIP_NAME} ${SONARSCANNERCLI_HASH} "sonar-scanner-${SONARSCANNERCLI_VERSION}" "sonar-scanner" TRUE)

DOWNLOAD_AND_EXTRACT("Dependency Check" ${DEPENDENCYCHECK_URL} ${DEPENDENCYCHECK_ZIP_NAME} ${DEPENDENCYCHECK_HASH} "dependency-check" "dependency-check" TRUE)

DOWNLOAD_AND_EXTRACT("Dependency Check MariaDB Connector" ${MARIADB_CONNECTOR_URL} ${MARIADB_CONNECTOR_ZIP_NAME} ${MARIADB_CONNECTOR_HASH} "dependency-check/lib/" "dependency-check/lib/" FALSE)
