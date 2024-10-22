import common.Review
import common.Build.JobType

def j = new Review
	(
		name: 'Linux_Integrated',
		label: 'Vanilla',
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
