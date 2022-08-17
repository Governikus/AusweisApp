import common.Build

def j = new Build
	(
		name: 'Win64_GNU_MSI_dev',
		libraries: ['Win64_GNU_dev'],
		label: 'Windows',
		artifacts: 'build/*.msi',
		weight: 2
	).generate(this)


j.with
{
	wrappers
	{
		environmentVariables
		{
			env('PATH', '${COMPILER_${MERCURIAL_REVISION_BRANCH}};$PATH')
		}
	}

	steps
	{
		batchFile('cd source & cmake --preset ci-win-debug')

		batchFile('cmake --build build --target package')

		batchFile('cmake --build build --target package.sign')

		batchFile('cmake -DCMD=CHECK_WIX_WARNING -DFILE=build/_CPack_Packages/win64/WIX/wix.log -P source/cmake/cmd.cmake')
	}
}
