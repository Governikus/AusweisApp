import common.Build

def j = new Build
	(
		name: 'FreeBSD',
		libraries: 'FreeBSD',
		label: 'FreeBSD',
		xunit: true
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env("QT_PLUGIN_PATH", '$WORKSPACE/libs/dist/plugins')
		}
	}

	steps
	{
		shell('cd source; cmake -Werror=dev --preset ci-bsd')

		shell('''\
			export LD_LIBRARY_PATH=$WORKSPACE/libs/dist/lib:$LD_LIBRARY_PATH
			cmake --build build
			'''.stripIndent().trim())

		shell('''\
			export QML2_IMPORT_PATH=$WORKSPACE/libs/dist/qml
			export LD_LIBRARY_PATH=$WORKSPACE/libs/dist/lib:$LD_LIBRARY_PATH
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
