import common.Library

def j = new Library
	(
		name: 'Win64_MSVC',
		label: 'MSVC'
	).generate(this)


j.with
{
	customWorkspace('workspace/' + MERCURIAL_REVISION_BRANCH + '_LW64M')

	steps
	{
		batchFile('''\
			call vcvarsall.bat amd64
			cd source/libs & cmake --preset ci-msvc-release
			'''.stripIndent().trim())

		batchFile('''\
			call vcvarsall.bat amd64
			cmake --build build --target compress
			'''.stripIndent().trim())
	}
}
