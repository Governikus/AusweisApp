import common.Build

def j = new Build
	(
		name: 'Win64_GNU_MSI',
		libraries: 'Win64_GNU',
		label: 'Windows',
		artifacts: 'build/*.msi',
		weight: 2
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake --preset ci-win-release')

		batchFile('cmake --build build --target package')

		batchFile('cmake --build build --target package.sign')

		batchFile('cmake -DCMD=CHECK_WIX_WARNING -DFILE=build/_CPack_Packages/win64/WIX/wix.log -P source/cmake/cmd.cmake')
	}
}
