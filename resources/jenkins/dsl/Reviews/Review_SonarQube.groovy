import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'SonarQube',
		label: 'Vanilla',
		artifacts: 'tmp/*.log,tmp/sonar-metadata.txt',
		excludePattern: 'source/**,cache/**',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env("XDG_RUNTIME_DIR", '$WORKSPACE/tmp')
			env("REVIEW_URL", '${review.url}')
			env("SONAR_USER_HOME", '$WORKSPACE/cache/sonar')
		}
	}

	steps
	{
		shell('mkdir -p cache')

		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('cmake -P source/cmake/prepare_sonarqube_env.cmake')

		shell('cd source; cmake --preset ci-linux')

		shell('''\
			cd build;
			$WORKSPACE/sonarqubetools/dependency-check/bin/dependency-check.sh --enableExperimental -f HTML -f JSON --scan $WORKSPACE/source/cmake --noupdate --connectionString=jdbc:mariadb://dependency-check-db.govkg.de/dependencycheck --dbUser=${DEPENDENCY_CHECK_USER} --dbPassword=${DEPENDENCY_CHECK_PASSWORD} --dbDriverName=org.mariadb.jdbc.Driver
			'''.stripIndent().trim())


		shell('$WORKSPACE/sonarqubetools/sonar-build-wrapper/build-wrapper-linux-x86-64 --out-dir build cmake --build build')

		shell('ctest -E qml_ --test-dir build --output-on-failure')

		shell('cmake --build build --target gcovr.sonar')

		shell(strip('''\
			REVIEW_ID=${REVIEW_URL%/};
			REVIEW_ID=${REVIEW_ID##*/};
			cd build;
			$WORKSPACE/sonarqubetools/sonar-scanner/bin/sonar-scanner
			-Dsonar.scanner.metadataFilePath=${WORKSPACE}/tmp/sonar-metadata.txt
			-Dsonar.pullrequest.key=${REVIEW_ID}
			-Dsonar.pullrequest.branch=${REVIEW_URL}
			-Dsonar.pullrequest.base=${MERCURIAL_REVISION_BRANCH}
			-Dsonar.login=${SONARQUBE_TOKEN}
			-Dsonar.qualitygate.wait=true
			-Dsonar.qualitygate.timeout=90
			'''))
	}
}
