import common.Build

def j = new Build
	(
		name: 'Linux',
		libraries: ['Linux'],
		label: 'Linux',
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
		shell('cd source; cmake --preset ci-linux')

		shell('''\
			cmake --build build
			'''.stripIndent().trim())

		shell('''\
			export QML2_IMPORT_PATH=$WORKSPACE/libs/build/dist/qml
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())

		shell('''\
			DESTDIR=$WORKSPACE/install cmake --install build
			'''.stripIndent().trim())

		shell('cmake --build build --target gcovr')

		shell('cmake --build build --target cloc.report')
	}

	publishers
	{
		cobertura('build/gcovr.xml')

		slocCount
		{
			pattern('build/cloc.xml')
			encoding('UTF-8')
			commentIsCode(false)
			buildsInGraph(0)
			ignoreBuildFailure(false)
		}
	}
}
