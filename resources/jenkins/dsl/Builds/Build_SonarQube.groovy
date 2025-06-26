import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'SonarQube',
		label: 'Vanilla',
		excludePattern: 'source/**,cache/**',
		artifacts: 'tmp/sonar-metadata.txt',
		trigger: '',
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
		shell('cmake -P source/ci.cmake')
	}
}
