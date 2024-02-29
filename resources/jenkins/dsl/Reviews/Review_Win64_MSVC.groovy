import common.Review

def j = new Review
	(
		name: 'Win64_MSVC',
		libraries: 'Win64_MSVC_dev',
		label: 'MSVC',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile('''\
			call vcvarsall.bat amd64
			cd source & cmake --preset ci-win
			'''.stripIndent().trim())

		batchFile('''\
			call vcvarsall.bat amd64
			cmake --build build
			'''.stripIndent().trim())

		batchFile('''\
			set PATH=%WORKSPACE%/libs/dist/bin;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/dist/qml
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
