import common.Review
import common.Build.JobType
import static common.Constants.strip

def j = new Review
	(
		name: 'Linux_Integrated',
		label: 'Vanilla',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell(strip('''\
			cd build;
			cmake -Werror=dev ../source
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_CXX_COMPILER=clazy
			-DINTEGRATED_SDK=ON
			-DSANITIZER=ON
			'''))

		shell('''\
			cd build; make ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('''\
			export ASAN_OPTIONS=detect_leaks=0,new_delete_type_mismatch=0
			cd build; ctest --output-on-failure ${MAKE_FLAGS}
			'''.stripIndent().trim())
	}
}
