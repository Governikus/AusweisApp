import common.Release

def j = new Release
	(
		name: 'Win64_MSVC_MSI',
		libraries: 'Win64_MSVC',
		label: 'MSVC',
		artifacts: 'libs/Toolchain_*,build/*.msi*'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cmake -P source/ci.cmake')
	}
}
