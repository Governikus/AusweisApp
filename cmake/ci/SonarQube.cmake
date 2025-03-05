set(CACHE_DIR cache)
if(NOT EXISTS ${CACHE_DIR})
	step(${CMAKE_COMMAND} -E make_directory ${CACHE_DIR})
endif()

set(SONARPATH ${WORKSPACE}/sonarqubetools)

step(${CMAKE_COMMAND} -P ${CMAKE_DIR}/prepare_sonarqube_env.cmake)

step(${T_CFG} --preset ci-linux)

step(${SONARPATH}/dependency-check/bin/dependency-check.sh
	--enableExperimental -f HTML -f JSON --scan ${CMAKE_DIR} --noupdate
	--connectionString=jdbc:mariadb://dependency-check-db.govkg.de/dependencycheck
	--dbUser=$ENV{DEPENDENCY_CHECK_USER}
	--dbPassword=$ENV{DEPENDENCY_CHECK_PASSWORD}
	--dbDriverName=org.mariadb.jdbc.Driver
	CHDIR ${T_BUILD_DIR}
)

step(${SONARPATH}/sonar-build-wrapper/build-wrapper-linux-x86-64 --out-dir ${T_BUILD_DIR} ${T_BUILD})

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
	${SONARPATH}/sonar-scanner/bin/sonar-scanner
	-Dsonar.scanner.metadataFilePath=${WORKSPACE}/tmp/sonar-metadata.txt
	${SONAR_CMDLINE}
	-Dsonar.token=$ENV{SONARQUBE_TOKEN}
	-Dsonar.qualitygate.wait=true
	-Dsonar.qualitygate.timeout=90
	CHDIR ${T_BUILD_DIR}
)
