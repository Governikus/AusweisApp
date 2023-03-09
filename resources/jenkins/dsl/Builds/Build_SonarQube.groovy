import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'SonarQube',
		label: 'Vanilla',
		excludePattern: 'source/**,cache/**',
		artifacts: 'tmp/sonar-metadata.txt',
		xunit: true
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env("XDG_RUNTIME_DIR", '$WORKSPACE/tmp')
			env("SONAR_USER_HOME", '$WORKSPACE/cache/sonar')
		}
	}

	steps
	{
		shell('mkdir -p cache')

		shell('cmake -P source/cmake/prepare_sonarqube_env.cmake')

		shell('cd source; cmake --preset ci-linux')

		shell('''\
			cd build;
			$WORKSPACE/sonarqubetools/dependency-check/bin/dependency-check.sh --enableExperimental -f HTML -f JSON --scan $WORKSPACE/source/cmake --noupdate --connectionString=jdbc:mariadb://dependency-check-db.govkg.de/dependencycheck --dbUser=${DEPENDENCY_CHECK_USER} --dbPassword=${DEPENDENCY_CHECK_PASSWORD} --dbDriverName=org.mariadb.jdbc.Driver
			'''.stripIndent().trim())

		shell('$WORKSPACE/sonarqubetools/sonar-build-wrapper/build-wrapper-linux-x86-64 --out-dir build cmake --build build')

		shell('ctest -LE qml -E Test_ui_qml_UIPlugInQml --test-dir build --output-on-failure')

		shell('cmake --build build --target gcovr.sonar')

		shell(strip('''\
			cd build;
			$WORKSPACE/sonarqubetools/sonar-scanner/bin/sonar-scanner
			-Dsonar.scanner.metadataFilePath=${WORKSPACE}/tmp/sonar-metadata.txt
			-Dsonar.branch.name=${MERCURIAL_REVISION_BRANCH}
			-Dsonar.login=${SONARQUBE_TOKEN}
			-Dsonar.qualitygate.wait=true
			-Dsonar.qualitygate.timeout=90
			'''))
	}
}
