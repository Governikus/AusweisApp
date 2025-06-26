import common.Review

def j = new Review
	(
		name: 'Formatting',
		libraries: 'Linux',
		label: 'Linux'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cmake -P source/ci.cmake')
	}

	publishers
	{
		textFinder('FORMATTING FAILED', '', true, false, true)
	}
}
