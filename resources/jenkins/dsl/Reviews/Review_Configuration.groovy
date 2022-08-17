import common.Review
import common.Build.JobType

def j = new Review
	(
		name: 'Configuration',
		libraries: ['Linux'],
		label: 'Linux',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env("QT_PLUGIN_PATH", '$WORKSPACE/libs/build/dist/plugins')
		}
	}

	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('cd source; cmake --preset ci-linux')

		shell('cmake --build build --target ALL_Test_configuration')

		shell('ctest --test-dir build --output-on-failure -R Test_configuration')
		shell('ctest --test-dir build --output-on-failure -L json')
	}
}
