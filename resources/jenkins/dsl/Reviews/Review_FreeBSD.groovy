import common.Review
import static common.Constants.strip

def j = new Review
	(
		name: 'FreeBSD',
		libraries: ['FreeBSD'],
		label: 'FreeBSD',
		artifacts: 'tmp/AusweisApp2.*.log',
		allowEmptyArtifacts: true,
		xunit: true,
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
		shell('cd source; python resources/jenkins/import.py')

		shell(strip('''\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DBUILD_SHARED_LIBS=on
			-DSANITIZER=on
			'''))

		shell('''\
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			cd build; make ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('''\
			export QML2_IMPORT_PATH=${WORKSPACE}/libs/build/dist/qml
			export LD_LIBRARY_PATH=$WORKSPACE/libs/build/dist/lib:$LD_LIBRARY_PATH
			export ASAN_OPTIONS=detect_leaks=0,new_delete_type_mismatch=0
			cd build; ctest --output-on-failure ${MAKE_FLAGS}
			'''.stripIndent().trim())
	}
}
