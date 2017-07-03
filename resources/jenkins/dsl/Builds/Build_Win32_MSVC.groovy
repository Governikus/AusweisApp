import common.Build

def j = new Build
	(
		name: 'Win32_MSVC',
		libraries: 'Win32_MSVC_dev',
		label: 'MSVC',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile('''\
			cd build
			call vcvarsall.bat
			cmake ../source -DCMAKE_CXX_COMPILER=clcache -DCMAKE_PREFIX_PATH=%WORKSPACE%/libs/build/dist -GNinja
			'''.stripIndent().trim())

		batchFile('''\
			cd build
			call vcvarsall.bat
			ninja %MAKE_FLAGS%
			'''.stripIndent().trim())

		batchFile('''\
			set PATH=%WORKSPACE%/libs/build/dist/bin;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/build/dist/plugins
			cd build & ctest %MAKE_FLAGS%
			'''.stripIndent().trim())
	}
}
