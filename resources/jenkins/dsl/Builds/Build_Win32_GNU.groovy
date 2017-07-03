import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Win32_GNU',
		libraries: 'Win32_GNU',
		label: "Windows7_${MERCURIAL_REVISION_BRANCH}",
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile(strip("""\
			cd build &
			cmake ../source -G\"MinGW Makefiles\"
			-DCMAKE_PREFIX_PATH=%WORKSPACE%\\libs\\build\\dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			"""))

		batchFile('cd build & mingw32-make %MAKE_FLAGS%')

		batchFile('''\
			set PATH=%WORKSPACE%/libs/build/dist/bin;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/build/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/build/dist/qml
			cd build & ctest %MAKE_FLAGS%
			'''.stripIndent().trim())
	}
}
