import common.Library

def j = new Library
	(
		name: 'Win64_GNU',
		label: 'Windows',
		weight: 3
	).generate(this)


j.with
{
	customWorkspace('workspace/' + MERCURIAL_REVISION_BRANCH + '_LW64G')

	wrappers
	{
		environmentVariables
		{
			env('MSYS2_PATH_TYPE', 'inherit')
		}
	}

	steps
	{
		batchFile('cd source/libs & cmake --preset ci-gnu-release')

		shell('''\
			#!c:\\msys64\\usr\\bin\\bash --login
			cd /jenkins/${MERCURIAL_REVISION_BRANCH}_LW64G/
			cmake --build build --target openssl
			'''.stripIndent().trim())

		batchFile('cmake --build build --target compress')
	}
}
