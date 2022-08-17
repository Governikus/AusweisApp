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
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell(strip('''\
			cd source;
			cmake -Werror=dev --preset ci-linux
			-DCMAKE_CXX_COMPILER=${Compiler}
			'''))

		shell('''\
			cmake --build build
			'''.stripIndent().trim())

		shell('''\
			export QML2_IMPORT_PATH=${WORKSPACE}/libs/build/dist/qml
			ctest --test-dir build --output-on-failure
			'''.stripIndent().trim())

		shell('''\
			DESTDIR=$WORKSPACE/install cmake --install build
			'''.stripIndent().trim())
	}
}
