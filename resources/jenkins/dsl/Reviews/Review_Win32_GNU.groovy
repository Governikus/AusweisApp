import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'Win32_GNU',
		libraries: ['Win32_GNU'],
		label: 'Windows',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true,
		weight: 2
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env('PATH', '${COMPILER_${MERCURIAL_REVISION_BRANCH}};$PATH')
		}
	}

	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile(strip("""\
			cd build &
			cmake -Werror=dev ../source -G\"MinGW Makefiles\"
			-DCMAKE_PREFIX_PATH=%WORKSPACE%\\libs\\build\\dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			"""))

		batchFile('cd build & mingw32-make %MAKE_FLAGS%')

		batchFile('''\
			set PATH=%WORKSPACE%/libs/build/dist/bin;%PATH%
			set PATH=%WORKSPACE%/build/src;%WORKSPACE%/build/test/helper;%PATH%
			set QT_PLUGIN_PATH=%WORKSPACE%/libs/build/dist/plugins
			set QML2_IMPORT_PATH=%WORKSPACE%/libs/build/dist/qml
			cd build & ctest --output-on-failure %MAKE_FLAGS%
			'''.stripIndent().trim())
	}
}
