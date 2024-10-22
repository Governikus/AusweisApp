import common.Review

def j = new Review
	(
		name: 'Win64_GNU_MSI',
		libraries: 'Win64_GNU',
		label: 'Windows',
		artifacts: 'build/*.msi',
		weight: 2
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake')
	}
}
