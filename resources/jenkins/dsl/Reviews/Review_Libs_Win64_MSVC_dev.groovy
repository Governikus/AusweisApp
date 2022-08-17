import common.LibraryReview

def j = new LibraryReview
	(
		name: 'Win64_MSVC_dev',
		label: 'MSVC'
	).generate(this)


j.with
{
	customWorkspace('workspace/' + MERCURIAL_REVISION_BRANCH + '_RLW64MD')

	steps
	{
		batchFile('cd source & cmake -DCMD=IMPORT_PATCH -P cmake/cmd.cmake')

		batchFile('''\
			call vcvarsall.bat amd64
			cd source/libs & cmake --preset ci-msvc-debug
			'''.stripIndent().trim())

		batchFile('''\
			call vcvarsall.bat amd64
			cmake --build build --target compress
			'''.stripIndent().trim())
	}
}
