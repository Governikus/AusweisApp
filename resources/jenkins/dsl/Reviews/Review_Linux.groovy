import common.Review
import common.Build.JobType
import static common.Constants.strip

def j = new Review
	(
		name: 'Linux',
		libraries: 'Linux',
		label: 'Linux',
		artifacts: 'tmp/*.log',
		allowEmptyArtifacts: true,
		xunit: true,
		jobType: JobType.Matrix
	).generate(this)


j.with
{
	axes
	{
		label('Compiler', 'g++', 'clang++')
		label('Agent', 'Linux')
	}

	steps
	{
		shell('cmake -P source/ci.cmake -- -DCMAKE_CXX_COMPILER=${Compiler}')
	}
}
