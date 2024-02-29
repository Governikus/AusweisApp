import common.Build

def j = new Build
	(
		name: 'Win64_GNU',
		libraries: 'Win64_GNU',
		label: 'Windows',
		xunit: true,
		weight: 2
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake --preset ci-win')

		batchFile('cmake --build build')

		batchFile('''\
			set PATH=%WORKSPACE%/libs/dist/bin;%PATH%
			set PATH=%WORKSPACE%/build/src;%WORKSPACE%/build/test/helper;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/dist/qml
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
