import common.Review

def j = new Review
	(
		name: 'MacOS_Integrated',
		libraries: 'MacOS',
		label: 'MacOS',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		shell('cmake -P source/ci.cmake')
	}
}
