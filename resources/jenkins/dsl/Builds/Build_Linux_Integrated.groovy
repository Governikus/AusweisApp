import common.Build
import static common.Constants.strip

def j = new Build
	(
		name: 'Linux_Integrated',
		label: 'Vanilla',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell(strip('''\
			cd build;
			cmake ../source
			-DCMAKE_CXX_COMPILER_LAUNCHER=ccache
			-DCMAKE_CXX_COMPILER=clazy
			-DCOVERAGE=true
			-DINTEGRATED_SDK=ON
			-DSANITIZER=on
			'''))

		shell('''\
			cd build; make ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('''\
			export ASAN_OPTIONS=detect_leaks=0,new_delete_type_mismatch=0
			cd build; ctest --output-on-failure ${MAKE_FLAGS}
			'''.stripIndent().trim())

		shell('cd build; make gcovr')
	}

	publishers
	{
		cobertura('build/gcovr.xml')
	}
}
