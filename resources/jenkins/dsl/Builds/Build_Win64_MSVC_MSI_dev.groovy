import common.Build

def j = new Build
	(
		name: 'Win64_MSVC_MSI_dev',
		libraries: 'Win64_MSVC_dev',
		label: 'MSVC',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile('''\
			call vcvarsall.bat amd64
			cd source & cmake --preset ci-win-debug
			'''.stripIndent().trim())

		batchFile('''\
			call vcvarsall.bat amd64
			cmake --build build --target package
			'''.stripIndent().trim())

		batchFile('''\
			call vcvarsall.bat amd64
			cmake --build build --target package.sign
			'''.stripIndent().trim())

		batchFile('cmake -DCMD=CHECK_WIX_WARNING -DFILE=build/_CPack_Packages/win64/WIX/wix.log -P source/cmake/cmd.cmake')
	}
}
