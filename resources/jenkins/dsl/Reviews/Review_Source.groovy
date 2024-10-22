import common.Review

def j = new Review
	(
		name: 'Source',
		label: 'Common',
		artifacts: 'build/*.tar.gz*'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
