import common.Review

def j = new Review
	(
		name: 'Win64_GNU',
		libraries: 'Win64_GNU',
		label: 'Windows',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
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
