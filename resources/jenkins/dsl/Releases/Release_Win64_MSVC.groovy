import common.Release

def j = new Release
	(
		name: 'Win64_MSVC_MSI',
		libraries: ['Win64_MSVC'],
		label: 'MSVC',
		artifacts: 'libs/build/Toolchain_*,build/*.msi'
	).generate(this)


j.with
{
	steps
	{
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
	}
}
