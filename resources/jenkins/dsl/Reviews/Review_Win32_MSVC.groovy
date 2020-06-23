import common.Review

def j = new Review
	(
		name: 'Win32_MSVC',
		libraries: ['Win32_MSVC_dev'],
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
			cd build
			call vcvarsall.bat x86
			cmake -Werror=dev ../source -DCMAKE_CXX_COMPILER=clcache -DCMAKE_PREFIX_PATH=%WORKSPACE%/libs/build/dist -GNinja
			'''.stripIndent().trim())

		batchFile('''\
			cd build
			call vcvarsall.bat x86
			ninja %MAKE_FLAGS%
			'''.stripIndent().trim())

		batchFile('''\
			set PATH=%WORKSPACE%/libs/build/dist/bin;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/build/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/build/dist/qml
			cd build & ctest --output-on-failure %MAKE_FLAGS%
			'''.stripIndent().trim())
	}
}
