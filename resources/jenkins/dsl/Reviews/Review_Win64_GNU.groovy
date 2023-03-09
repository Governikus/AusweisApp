import common.Review

def j = new Review
	(
		name: 'Win64_GNU',
		libraries: ['Win64_GNU'],
		label: 'Windows',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true,
		weight: 2
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile('cd source & cmake --preset ci-win')

		batchFile('cmake --build build')

		batchFile('''\
			set PATH=%WORKSPACE%/libs/build/dist/bin;%PATH%
			set PATH=%WORKSPACE%/build/src;%WORKSPACE%/build/test/helper;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/build/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/build/dist/qml
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
