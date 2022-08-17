import common.Build

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
		shell('cd source; cmake --preset ci-integrated')

		shell('cmake --build build')

		shell('ctest --test-dir build --output-on-failure')

		shell('cmake --build build --target gcovr')
	}

	publishers
	{
		cobertura('build/gcovr.xml')
	}
}
