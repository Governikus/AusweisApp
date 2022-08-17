import common.Release

def j = new Release
	(
		name: 'Source',
		label: 'Common',
		artifacts: 'build/*.tar.gz'
	).generate(this)


j.with
{
	steps
	{
		shell('cd source; cmake --preset ci-tools')
		shell('cmake --build build --target package_source')
	}
}
