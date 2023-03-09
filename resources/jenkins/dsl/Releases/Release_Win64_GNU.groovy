import common.Release

def j = new Release
	(
		name: 'Win64_GNU_MSI',
		libraries: ['Win64_GNU'],
		label: 'Windows',
		artifacts: 'libs/build/Toolchain_*,build/*.msi',
		weight: 2
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake --preset ci-win-release')

		batchFile('cmake --build build --target package')

		batchFile('cmake --build build --target package.sign')
	}
}
