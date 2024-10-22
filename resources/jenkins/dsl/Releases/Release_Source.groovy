import common.Release

def j = new Release
	(
		name: 'Source',
		label: 'Common',
		artifacts: 'build/*.tar.gz*'
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
