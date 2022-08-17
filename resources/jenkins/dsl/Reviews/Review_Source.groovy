import common.Review

def j = new Review
	(
		name: 'Source',
		label: 'Common',
		artifacts: 'build/*.tar.gz'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')
		shell('cd source; cmake --preset ci-tools')
		shell('cmake --build build --target package_source')
	}
}
