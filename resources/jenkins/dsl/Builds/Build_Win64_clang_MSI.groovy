import common.Build
import common.Build.JobType

def j = new Build
	(
		name: 'Win64_clang_MSI',
		libraries: 'Win64_MSVC',
		label: 'MSVC',
		artifacts: 'build/*.msi',
		jobType: JobType.Matrix
	).generate(this)


j.with
{
	axes
	{
		label('Compiler', 'clang-cl')
	}

	steps
	{
		batchFile('cmake -P source/ci.cmake -- -DCMAKE_CXX_COMPILER=%Compiler%')
	}
}
