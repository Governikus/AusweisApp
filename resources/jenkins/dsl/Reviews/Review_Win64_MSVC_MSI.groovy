import common.Review

def j = new Review
	(
		name: 'Win64_MSVC_MSI',
		libraries: 'Win64_MSVC',
		label: 'MSVC',
		artifacts: 'build/*.msi'
	).generate(this)


j.with
{
	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile('''\
			call vcvarsall.bat amd64
			cd source & cmake --preset ci-win-release
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
