import common.Build

def j = new Build
	(
		name: 'Win64_MSVC_MSI_dev',
		libraries: 'Win64_MSVC_dev',
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
