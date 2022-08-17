import common.Build

def j = new Build
	(
		name: 'Win64_MSVC',
		libraries: ['Win64_MSVC_dev'],
		label: 'MSVC',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile('''\
			call vcvarsall.bat amd64
			cd source & cmake --preset ci-win
			'''.stripIndent().trim())

		batchFile('''\
			call vcvarsall.bat amd64
			cmake --build build
			'''.stripIndent().trim())

		batchFile('''\
			set PATH=%WORKSPACE%/libs/build/dist/bin;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/build/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/build/dist/qml
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())
	}
}
