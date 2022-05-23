import common.Review
import common.Build.JobType
import static common.Constants.strip

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

		shell(strip('''\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DBUILD_SHARED_LIBS=on
			'''))

		shell('''\
			cd build; make ${MAKE_FLAGS} ALL_Test_configuration
			'''.stripIndent().trim())

		shell('''\
			cd build; ctest --output-on-failure ${MAKE_FLAGS} -R Test_configuration
			'''.stripIndent().trim())

		shell('''\
			cd build; ctest --output-on-failure ${MAKE_FLAGS} -L json
			'''.stripIndent().trim())
	}
}
