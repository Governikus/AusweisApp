import common.Review

def j = new Review
	(
		name: 'Win64_MSVC',
		libraries: 'Win64_MSVC_dev',
		label: 'MSVC',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake')
	}
}
