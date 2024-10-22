import common.Review

def j = new Review
	(
		name: 'FreeBSD',
		libraries: 'FreeBSD',
		label: 'FreeBSD',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true,
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
