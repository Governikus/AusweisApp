import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Linux',
		libraries: 'Linux',
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
		shell(strip('''\
			cd build;
			cmake ../source
			-DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCOVERAGE=true
			-DBUILD_SHARED_LIBS=on
			-DSANITIZER=address
			'''))

		shell('''\
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			cd build; make ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('''\
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			export ASAN_OPTIONS=detect_leaks=0
			cd build; ctest ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('cd build; make gcovr')
	}

	publishers
	{
		cobertura('build/gcovr.xml')
	}
}
