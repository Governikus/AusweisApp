import common.Review
import common.Build.JobType

def j = new Review
	(
		name: 'Configuration',
		libraries: 'Linux',
		label: 'Linux',
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
