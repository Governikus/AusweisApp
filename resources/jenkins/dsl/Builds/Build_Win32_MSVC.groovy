import common.Build

def j = new Build
	(
		name: 'Win32_MSVC',
		libraries: ['Win32_MSVC_dev'],
		label: 'MSVC',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile('''\
			cd build
			call vcvarsall.bat x86
			cmake ../source -DCMAKE_CXX_COMPILER_LAUNCHER=sccache -DCMAKE_PREFIX_PATH=%WORKSPACE%/libs/build/dist -GNinja
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
