import common.Review

def j = new Review
	(
		name: 'Formatting',
		libraries: 'Linux',
		label: 'Common'
	).generate(this)


j.with
{
	concurrentBuild()

	steps
	{
		shell('cmake -DPENDING=OFF -P source/ci.cmake')
	}

	publishers
	{
		textFinder('FORMATTING FAILED', '', true, false, true)
	}
}
