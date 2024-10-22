import common.Build

def j = new Build
	(
		name: 'Win64_GNU',
		libraries: 'Win64_GNU',
		label: 'Windows',
		xunit: true,
		weight: 2
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake')
	}
}
