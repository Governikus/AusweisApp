import common.Library

def j = new Library
	(
		name: 'Win64_MSVC_dev',
		label: 'MSVC'
	).generate(this)


j.with
{
	customWorkspace('workspace/' + MERCURIAL_REVISION_BRANCH + '_LW64MD')

	wrappers
	{
		environmentVariables
		{
			env('PATH', '$WORKSPACE/build/dist/bin;$PATH')
		}
	}

	steps
	{
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
