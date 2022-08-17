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
		shell('cd source; cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		shell('cd source; cmake --preset ci-integrated')

		shell('cmake --build build')

		shell('ctest --test-dir build --output-on-failure')
	}
}
