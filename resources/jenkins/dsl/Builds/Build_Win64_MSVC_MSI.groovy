import common.Build

def j = new Build
	(
		name: 'Win64_MSVC_MSI',
		libraries: 'Win64_MSVC',
		label: 'MSVC',
		artifacts: 'build/*.msi*'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake')
	}
}
