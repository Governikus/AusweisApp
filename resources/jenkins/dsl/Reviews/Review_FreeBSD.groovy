import common.Review

def j = new Review
	(
		name: 'FreeBSD',
		libraries: ['FreeBSD'],
		label: 'FreeBSD',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true,
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

		shell('cd source; cmake --preset ci-bsd')

		shell('''\
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			cmake --build build
			'''.stripIndent().trim())

		shell('''\
			export QML2_IMPORT_PATH=${WORKSPACE}/libs/build/dist/qml
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
