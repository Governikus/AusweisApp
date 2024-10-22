import common.Build

def j = new Build
	(
		name: 'Win64_MSVC',
		libraries: 'Win64_MSVC_dev',
		label: 'MSVC',
		xunit: true
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake')
	}
}
