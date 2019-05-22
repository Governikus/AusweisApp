import common.Review
import common.Build.JobType
import static common.Constants.strip

def j = new Review
	(
		name: 'Linux',
		libraries: ['Linux'],
		label: 'Linux',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true,
		jobType: JobType.Matrix
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

	axes
	{
		label('Compiler', 'g++', 'clang++')
	}

	steps
	{
		shell('cd source; python resources/jenkins/import.py')

		shell(strip('''\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_PREFIX_PATH=${WORKSPACE}/libs/build/dist
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_CXX_COMPILER=${Compiler}
			-DBUILD_SHARED_LIBS=on
			-DSANITIZER=on
			'''))

		shell('''\
			cd build; make ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('''\
			export QML2_IMPORT_PATH=${WORKSPACE}/libs/build/dist/qml
			export ASAN_OPTIONS=detect_leaks=0,new_delete_type_mismatch=0
			cd build; ctest --output-on-failure ${MAKE_FLAGS}
			'''.stripIndent().trim())
	}
}
